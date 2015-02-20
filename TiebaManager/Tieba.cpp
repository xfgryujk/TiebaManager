/*
Copyright (C) 2015  xfgryujk
http://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "stdafx.h"
#include "Tieba.h"
#include "Global.h"
#include "Setting.h"
#include <queue>
using std::queue;
#include <Mmsystem.h>


// 贴吧、用户信息
CString g_forumName;
CString g_forumID;
CString g_cookie;
CString g_tbs;

volatile BOOL g_stopScanFlag;
CWinThread* g_scanThread = NULL;
set<__int64> g_ignoredTID; // 不删的主题ID(已扫描)
set<__int64> g_ignoredPID; // 不删的帖子ID(已扫描)
set<__int64> g_ignoredLZLID; // 不删的楼中楼ID(已扫描)
set<__int64> g_deletedTID; // 已删的主题ID
map<__int64, int> g_reply; // 主题的回复数
map<CString, int> g_IDTrigCount; // 某ID违规次数，已封为-1

struct Operation
{
	CString msg;		// 提示消息
	TBObject object;	// 操作对象
	CString tid;		// 主题ID
	CString title;		// 主题标题
	CString floor;		// 楼层
	CString pid;		// 帖子ID
	CString author;		// 帖子作者
};
static queue<Operation> g_operationQueue; // 操作队列
static CCriticalSection g_operationQueueLock;
CWinThread* g_operateThread = NULL;


// 采集 /////////////////////////////////////////////////////////////////////////////////
// 取主题列表
BOOL GetThreads(LPCTSTR forumName, LPCTSTR ignoreThread, vector<ThreadInfo>& threads)
{
	CString src = HTTPGet(_T("http://tieba.baidu.com/f?ie=UTF-8&kw=") + EncodeURI(forumName) 
		+ _T("&tp=0&pn=") + ignoreThread + _T("&apage=1"), FALSE, &g_stopScanFlag);

	CStringArray rawThreads;
	SplitString(rawThreads, src, THREAD_SPLIT);
	if (rawThreads.GetSize() < 2)
	{
		if (src != NET_STOP_TEXT && src != NET_TIMEOUT_TEXT)
			WriteString(_T("forum.txt"), src);
		return FALSE;
	}

	int iThreads;
	// 今日主题
	CString topic = GetStringBetween(rawThreads[0], TOPIC_LEFT, TOPIC_RIGHT);
	if (topic != _T(""))
	{
		threads.resize(rawThreads.GetSize());
		int pos = topic.Find(TOPIC_TID_LEFT);
		threads[0].tid = GetStringBetween(topic, TOPIC_TID_LEFT, TOPIC_TID_RIGHT, pos);
		threads[0].reply = GetStringBetween(topic, TOPIC_REPLY_LEFT, TOPIC_REPLY_RIGHT);
		threads[0].title = GetStringBetween(topic, TOPIC_TITLE_LEFT, TOPIC_TITLE_RIGHT, pos);
		threads[0].preview = GetStringBetween(topic, TOPIC_PREVIEW_LEFT, TOPIC_PREVIEW_RIGHT, pos);
		threads[0].author = _T("");

		iThreads = 1;
	}
	else
	{
		threads.resize(rawThreads.GetSize() - 1);
		iThreads = 0;
	}

	// 普通主题
	rawThreads[rawThreads.GetSize() - 1] = GetStringBefore(rawThreads[rawThreads.GetSize() - 1], THREAD_END);
	for (int iRawThreads = 1; iRawThreads < rawThreads.GetSize(); iRawThreads++, iThreads++)
	{
		threads[iThreads].tid = GetStringBetween(rawThreads[iRawThreads], THREAD_TID_LEFT, THREAD_TID_RIGHT);
		threads[iThreads].reply = GetStringBetween(rawThreads[iRawThreads], THREAD_REPLY_LEFT, THREAD_REPLY_RIGHT);
		threads[iThreads].title = HTMLUnescape(GetStringBetween(rawThreads[iRawThreads], THREAD_TITLE_LEFT, THREAD_TITLE_RIGHT));
		threads[iThreads].preview = HTMLUnescape(GetStringBetween(rawThreads[iRawThreads], THREAD_PREVIEW_LEFT, THREAD_PREVIEW_RIGHT));
		threads[iThreads].author = GetStringBetween(rawThreads[iRawThreads], THREAD_AUTHOR_LEFT, THREAD_AUTHOR_RIGHT);

		//OutputDebugString(_T("\n"));
		//OutputDebugString(rawThreads[iRawThreads]);
		//OutputDebugString(_T("\n----------------------------------"));
	}

	return TRUE;
}

// 取帖子列表
GetPostsResult GetPosts(const CString& tid, const CString& _src, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls)
{
	CString src = _src != _T("") ? _src : HTTPGet(_T("http://tieba.baidu.com/p/") + tid + _T("?pn=") + page, FALSE, &g_stopScanFlag);
	if (src == NET_STOP_TEXT)
		return GET_POSTS_STOP;
	if (src == NET_TIMEOUT_TEXT)
		return GET_POSTS_TIMEOUT;

	CStringArray rawPosts;
	SplitString(rawPosts, src, POST_SPLIT);
	if (rawPosts.GetSize() < 2)
		return GET_POSTS_DELETED;
	
	posts.resize(rawPosts.GetSize() - 1);
	for (int iRawPosts = 1, iPosts = 0; iRawPosts < rawPosts.GetSize(); iRawPosts++, iPosts++)
	{
		posts[iPosts].pid = GetStringBetween(rawPosts[iRawPosts], POST_PID_LEFT, POST_PID_RIGHT);
		posts[iPosts].floor = GetStringBetween(rawPosts[iRawPosts], POST_FLOOR_LEFT, POST_FLOOR_RIGHT);
		posts[iPosts].author = JSUnescape(GetStringBetween(rawPosts[iRawPosts], POST_AUTHOR_LEFT, POST_AUTHOR_RIGHT));
		posts[iPosts].content = GetStringBetween(rawPosts[iRawPosts], POST_CONTENT_LEFT, POST_CONTENT_RIGHT);
		// 去掉首尾空格
		int left = 0;
		while (left < posts[iPosts].content.GetLength() && posts[iPosts].content[left] == _T(' '))
			left++;
		int right = posts[iPosts].content.GetLength() - 1;
		while (right >= left && posts[iPosts].content[right] == _T(' '))
			right--;
		posts[iPosts].content = posts[iPosts].content.Mid(left, right - left + 1);

		//OutputDebugString(_T("\n"));
		//OutputDebugString(rawPosts[iRawThreads]);
		//OutputDebugString(_T("\n----------------------------------"));
	}

	GetLzls(tid, page, posts, lzls);

	return GET_POSTS_SUCCESS;
}

// 取楼中楼列表
void GetLzls(const CString& tid, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls)
{
	time_t timestamp;
	time(&timestamp);
	CString url;
	url.Format(_T("http://tieba.baidu.com/p/totalComment?t=%I64d&tid=%s&fid=%s&pn=%s&see_lz=0"), timestamp, tid, g_forumID, page);
	CString src = HTTPGet(url, FALSE, &g_stopScanFlag);

	lzls.clear();
	PostInfo lzl;
	// 遍历楼层
	for (std::regex_iterator<LPCTSTR> it((LPCTSTR)src, (LPCTSTR)src + src.GetLength(), LZL_FLOOR_REG), end; it != end; it++)
	{
		// 查找该层楼层
		CString pid = (*it)[1].str().c_str(); // 该层PID
		for (PostInfo post : posts)
			if (post.pid == pid)
			{
				lzl.floor = post.floor;
				break;
			}
		
		// 遍历该层楼中楼
		CString floorContent = (*it)[2].str().c_str(); // 该层楼中楼内容
		for (std::regex_iterator<LPCTSTR> it2((LPCTSTR)floorContent, (LPCTSTR)floorContent 
			+ floorContent.GetLength(), LZL_CONTENT_REG), end2; it2 != end2; it2++)
		{
			lzl.pid = (*it2)[1].str().c_str();
			lzl.author = JSUnescape((*it2)[2].str().c_str());
			lzl.content = HTMLUnescape(JSUnescape((*it2)[3].str().c_str()));
			lzls.push_back(lzl);
		}

		//OutputDebugString(_T("\n"));
		//OutputDebugString(floorContent);
		//OutputDebugString(_T("\n----------------------------------"));
	}
}


// 扫描 /////////////////////////////////////////////////////////////////////////////////
// 检查违规
BOOL CheckIllegal(LPCTSTR content, LPCTSTR author, CString& msg)
{
	g_optionsLock.Lock();

	// 信任用户
	for (const CString& whiteList : g_whiteList)
		if (author == whiteList)
		{
			g_optionsLock.Unlock();
			return FALSE;
		}

	// 信任内容
	for (const RegexText& whiteContent : g_whiteContent)
		if (StringIncludes(content, whiteContent))
		{
			g_optionsLock.Unlock();
			return FALSE;
		}
	
	// 违规内容
	for (const RegexText& keyword : g_keywords)
		if (StringIncludes(content, keyword))
		{
			msg = _T("<font color=red> 触发违禁词 </font>") + HTMLEscape(keyword.text);
			g_optionsLock.Unlock();
			return TRUE;
		}

	// 屏蔽用户
	for (const RegexText& blackList : g_blackList)
		if (StringMatchs(author, blackList))
		{
			msg = _T("<font color=red> 触发屏蔽用户 </font>") + HTMLEscape(blackList.text);
			g_optionsLock.Unlock();
			return TRUE;
		}

	g_optionsLock.Unlock();
	return FALSE;
}

// 总扫描线程
UINT AFX_CDECL ScanThread(LPVOID mainDlg)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)mainDlg;
	g_stopScanFlag = FALSE;
	dlg->m_startButton.EnableWindow(FALSE);
	dlg->m_pageEdit.EnableWindow(FALSE);
	dlg->m_stopButton.EnableWindow(TRUE);

	// 开始操作线程
	if (!g_operationQueue.empty() && g_operateThread == NULL)
		g_operateThread = AfxBeginThread(OperateThread, mainDlg);
	
	// 初始化日志文档
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CComPtr<IHTMLDocument2> document;
	//dlg->GetLogDocument(document); // 线程里不能用Write()
	dlg->GetLogDocumentInThread(document);
	CComPtr<IHTMLDocument2>* pDocument = (CComPtr<IHTMLDocument2>*)&(int&)document;

	// 初始化页数
	CString sPage;
	dlg->m_pageEdit.GetWindowText(sPage);
	int iPage = _ttoi(sPage);
	CString ignoreThread; // 忽略前几个主题
	ignoreThread.Format(_T("%d"), (iPage - 1) * 50);

	vector<ThreadInfo> threads;
	CString msg;
	while (!g_stopScanFlag)
	{
		DWORD startTime = GetTickCount();
		dlg->m_stateStatic.SetWindowText(_T("扫描主题中"));
		if (!g_briefLog)
			dlg->Log(_T("<font color=green>本轮扫描开始，使用方案：</font>") + g_currentOption, pDocument);
		
		if (!GetThreads(g_forumName, ignoreThread, threads))
		{
			if (g_stopScanFlag)
				break;
			if (!g_briefLog)
				dlg->Log(_T("<font color=red>获取主题列表失败，重新开始本轮</font>"), pDocument);
			continue;
		}

		// 扫描主题
		for (const ThreadInfo& thread : threads)
		{
			if (g_stopScanFlag)
				break;
			if (CheckIllegal(thread.title + _T("\r\n") + thread.preview, thread.author, msg))
			{
				__int64 tid = _ttoi64(thread.tid);
				if (g_ignoredTID.find(tid) == g_ignoredTID.end())
				{
					AddOperation(thread.title + _T("\r\n") + thread.preview, TBOBJ_THREAD, thread.tid, 
						thread.title, _T("1"), _T(""), thread.author);
					dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">")
						+ HTMLEscape(thread.title) + _T("</a>") + msg, pDocument);
					g_ignoredTID.insert(tid);
				}
			}
		}

		// 扫描帖子
		if (!g_onlyScanTitle)
			for (const ThreadInfo& thread : threads)
			{
				if (g_stopScanFlag)
					break;
				if (g_deletedTID.find(_ttoi64(thread.tid)) == g_deletedTID.end())
					ScanPost(thread, dlg, pDocument);
			}

			dlg->m_stateStatic.SetWindowText(_T("延时中"));
		if (!g_briefLog)
		{
			CString content;
			content.Format(_T("<font color=green>本轮扫描结束，用时%.3f秒</font>"), (float)(GetTickCount() - startTime) / 1000.0f);
			dlg->Log(content, pDocument);
		}

		// 延时
		int count = g_scanInterval * 10;
		for (int i = 0; i < count; i++)
		{
			if (g_stopScanFlag)
				break;
			Sleep(100);
		}
	}

	if (!g_briefLog)
		dlg->Log(_T("<font color=green>扫描结束</font>"), pDocument);
	CoUninitialize();
	dlg->m_stopButton.EnableWindow(FALSE);
	dlg->m_startButton.EnableWindow(TRUE);
	dlg->m_pageEdit.EnableWindow(TRUE);
	dlg->m_stateStatic.SetWindowText(_T("待机中"));

	g_scanThread = NULL;
	return 0;
}

// 扫描帖子
void ScanPost(const ThreadInfo& thread, CTiebaManagerDlg* dlg, CComPtr<IHTMLDocument2>* pDocument)
{
	__int64 tid = _ttoi64(thread.tid);
	int reply = _ttoi(thread.reply);
	auto historyReplyIt = g_reply.find(tid);
	BOOL hasHistoryReply = historyReplyIt != g_reply.end();
	if (hasHistoryReply && reply <= historyReplyIt->second) // 无新回复
		return;

	// 第一页
	CString src = HTTPGet(_T("http://tieba.baidu.com/p/" + thread.tid), FALSE, &g_stopScanFlag);
	if (src == NET_STOP_TEXT)
		return;

	// 帖子页数
	CString pageCount = GetStringBetween(src, PAGE_COUNT_LEFT, PAGE_COUNT_RIGHT);
	if (pageCount == _T(""))
	{
		WriteString(src, _T("page1.txt"));
		if (!g_briefLog)
			dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">") + thread.title 
				+ _T("</a> <font color=red>获取贴子列表失败，暂时跳过</font>"), pDocument);
		return;
	}

	// 扫描帖子页
	int iPageCount = _ttoi(pageCount);
	BOOL res = ScanPostPage(thread.tid, 1, thread.title, hasHistoryReply, 0, src, dlg, pDocument);
	if (iPageCount > 1 && !g_stopScanFlag)
		res = ScanPostPage(thread.tid, iPageCount, thread.title, hasHistoryReply, 0, _T(""), dlg, pDocument);

	// 记录历史回复
	if (res)
	{
		if (hasHistoryReply)
			historyReplyIt->second = reply;
		else
			g_reply[tid] = reply;
	}
}

// 扫描帖子页
BOOL ScanPostPage(const CString& tid, int page, const CString& title, BOOL hasHistoryReply, 
	int ScanedCount, const CString& src, CTiebaManagerDlg* dlg, CComPtr<IHTMLDocument2>* pDocument)
{
	dlg->m_stateStatic.SetWindowText(_T("扫描贴子：") + tid + _T(":1 ") + title);

	CString sPage;
	sPage.Format(_T("%d"), page);
	vector<PostInfo> posts, lzls;
	GetPostsResult res = GetPosts(tid, src, sPage, posts, lzls);
	switch (res)
	{
	case GET_POSTS_TIMEOUT:
	case GET_POSTS_DELETED:
		dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + title
			+ _T("</a> <font color=red>获取贴子列表失败(") + (res == GET_POSTS_TIMEOUT ? _T("超时") : 
			_T("可能已被删")) + _T(")，暂时跳过</font>"), pDocument);
	case GET_POSTS_STOP:
		return FALSE;
	}

	CString msg;
	// 扫描帖子
	for (const PostInfo& post : posts)
	{
		if (g_stopScanFlag)
			return FALSE;
		if (CheckIllegal(post.content, post.author, msg))
		{
			__int64 pid = _ttoi64(post.pid);
			if (g_ignoredPID.find(pid) == g_ignoredPID.end())
			{
				AddOperation(post.content.GetLength() > 700 ? post.content.Left(700) + _T("…………") : post.content,
					post.floor == _T("1") ? TBOBJ_THREAD : TBOBJ_POST, tid, title, post.floor, post.pid, post.author);
				dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) + 
					_T("</a> ") + post.floor + _T("楼") + msg, pDocument);
				g_ignoredPID.insert(pid);
			}
		}
	}

	// 扫描楼中楼
	for (const PostInfo& lzl : lzls)
	{
		if (g_stopScanFlag)
			return FALSE;
		if (CheckIllegal(lzl.content, lzl.author, msg))
		{
			__int64 lzlid = _ttoi64(lzl.pid);
			if (g_ignoredLZLID.find(lzlid) == g_ignoredLZLID.end())
			{
				AddOperation(lzl.content, TBOBJ_LZL, tid, title, lzl.floor, lzl.pid, lzl.author);
				dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) +
					_T("</a> ") + lzl.floor + _T("楼回复") + msg, pDocument);
				g_ignoredLZLID.insert(lzlid);
			}
		}
	}

	// 递归扫描上一页
	if (!hasHistoryReply) // 如果有历史回复前面几页很可能被扫描过了，不递归
	{
		if (++ScanedCount < g_scanPageCount) // 没达到最大扫描页数
		{
			if (--page < 2) // 扫描完
				return TRUE;
			return ScanPostPage(tid, page, title, FALSE, ScanedCount, _T(""), dlg, pDocument);
		}
	}
	return TRUE;
}


// 操作 /////////////////////////////////////////////////////////////////////////////////
// 添加操作
void AddOperation(const CString& msg, TBObject object, const CString& tid, const CString& title,
	const CString& floor, const CString& pid, const CString& author)
{
	Operation tmp;
	tmp.msg = msg;
	tmp.object = object;
	tmp.tid = tid;
	tmp.title = title;
	tmp.floor = floor;
	tmp.pid = pid;
	tmp.author = author;
	g_operationQueueLock.Lock();
	g_operationQueue.push(tmp);
	g_operationQueueLock.Unlock();
	if (g_operateThread == NULL)
		g_operateThread = AfxBeginThread(OperateThread, AfxGetApp()->m_pMainWnd);
}

// 操作线程
UINT AFX_CDECL OperateThread(LPVOID mainDlg)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)mainDlg;

	// 初始化日志文档
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CComPtr<IHTMLDocument2> document;
	dlg->GetLogDocumentInThread(document);
	CComPtr<IHTMLDocument2>* pDocument = (CComPtr<IHTMLDocument2>*)&(int&)document;

	while (!g_operationQueue.empty() && !g_stopScanFlag)
	{
		Operation op = g_operationQueue.front();
		g_operationQueueLock.Lock();
		g_operationQueue.pop();
		g_operationQueueLock.Unlock();

		// 主题已被删
		__int64 tid = _ttoi64(op.tid);
		if (g_deletedTID.find(tid) != g_deletedTID.end())
			continue;
		
		// 确认是否操作
		if (g_confirm)
		{
			if (MessageBox(NULL, op.msg + _T("\r\n作者：") + op.author + _T("\r\n是否处理？"),
				op.title, MB_ICONQUESTION | MB_YESNO) == IDNO)
			{
				if (op.object == TBOBJ_THREAD)
					dlg->Log(_T("<font color=green>忽略 </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a>"), pDocument);
				else
					dlg->Log(_T("<font color=green>忽略 </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor 
						+ (op.object == TBOBJ_POST ? _T("楼") : _T("楼回复")), pDocument);
				continue;
			}
		}

		// 封禁
		if (g_banID || g_banIP)
		{
			auto countIt = g_IDTrigCount.find(op.author);
			BOOL hasHistory = countIt != g_IDTrigCount.end();
			int count = hasHistory ? (countIt->second + 1) : 1;
			if (count < g_trigCount) // 没达到封禁违规次数
			{
				if (count != 0) // 未封
				{
					if (hasHistory)
						countIt->second = count;
					else
						g_IDTrigCount[op.author] = 1;
				}
			}
			else // 达到封禁违规次数
			{
				get_ip_tbs banTBS;
				if (!GetBanTBS(op.tid, op.author, banTBS))
				{
					dlg->Log(_T("<font color=red>封禁 </font>") + op.author + _T("<font color=red> 失败！\
(获取封禁口令号失败)</font><a href=\"BD:") + op.tid + _T(",") + op.author + _T("\">重试</a>"), pDocument);
				}
				else
				{
					BOOL success = TRUE;
					if (g_banID) // 封ID
					{
						CString code = BanID(op.author, banTBS.tbs_ban_user);
						success = code == _T("0");
						if (!success)
						{
							CString content;
							content.Format(_T("<font color=red>封禁 </font>%s<font color=red> 失败！\
错误代码：%s(%s)</font><a href=\"BD:%s,%s\">重试</a>"), op.author, code, GetTiebaErrorText(code), op.tid, op.author);
							dlg->Log(content, pDocument);
						}
					}
					if (g_banIP) // 封IP
					{
						CString code = BanIP(banTBS.ip_int, banTBS.tbs_ban_ip, banTBS.ip_secure_str);
						success = code == _T("0");
						if (!success)
						{
							CString content;
							content.Format(_T("<font color=red>封禁 </font>%s<font color=red>IP 失败！\
错误代码：%s(%s)</font><a href=\"BP:%s,%s\">重试</a>"), op.author, code, GetTiebaErrorText(code), op.tid, op.author);
							dlg->Log(content, pDocument);
						}
					}
					if (success)
					{
						sndPlaySound(_T("封号.wav"), SND_ASYNC | SND_NODEFAULT);
						if (hasHistory)
							countIt->second = -1;
						else
							g_IDTrigCount[op.author] = -1;
						dlg->Log(_T("<font color=red>封禁 </font>") + op.author, pDocument);
					}
				}
			}
		}

		// 删帖
		if (op.object == TBOBJ_THREAD) // 主题
		{
			CString code = DeleteThread(op.tid);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a><font color=red> 删除失败！\
错误代码：%s(%s)</font><a href=\"DT:%s\">重试</a>"), op.tid, HTMLEscape(op.title), code, GetTiebaErrorText(code), op.tid);
				dlg->Log(content, pDocument);
			}
			else
			{
				sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
				g_deletedTID.insert(_ttoi64(op.tid));
				dlg->Log(_T("<font color=red>删除 </font><a href=\"http://tieba.baidu.com/p/") + op.tid 
					+ _T("\">") + HTMLEscape(op.title) + _T("</a>"), pDocument);
				Sleep((DWORD)(g_deleteInterval * 1000));
			}
		}
		else if (op.object == TBOBJ_POST) // 帖子
		{
			CString code = DeletePost(op.tid, op.pid);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a> %s楼<font color=red> 删除失败！\
错误代码：%s(%s)</font><a href=\"DP:%s,%s\">重试</a>"), op.tid, HTMLEscape(op.title), op.floor, code, 
					GetTiebaErrorText(code), op.tid, op.pid);
				dlg->Log(content, pDocument);
			}
			else
			{
				sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
				dlg->Log(_T("<font color=red>删除 </font><a href=\"http://tieba.baidu.com/p/") + op.tid
					+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("楼"), pDocument);
				Sleep((DWORD)(g_deleteInterval * 1000));
			}
		}
		else /*if (op.object == TBOBJ_POST)*/ // 楼中楼
		{
			CString code = DeleteLZL(op.tid, op.pid);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a> %s楼回复<font color=red> 删除失败！\
错误代码：%s(%s)</font><a href=\"DL:%s,%s\">重试</a>"), op.tid, HTMLEscape(op.title), op.floor, code,
								  GetTiebaErrorText(code), op.tid, op.pid);
				dlg->Log(content, pDocument);
			}
			else
			{
				sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
				dlg->Log(_T("<font color=red>删除 </font><a href=\"http://tieba.baidu.com/p/") + op.tid
					+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("楼回复"), pDocument);
				Sleep((DWORD)(g_deleteInterval * 1000));
			}
		}
	}

	g_operateThread = NULL;
	CoUninitialize();
	return 0;
}

// 获取封禁用tbs
BOOL GetBanTBS(LPCTSTR tid, LPCTSTR userName, get_ip_tbs& result)
{
	CString url;
	url.Format(_T("http://tieba.baidu.com/bawu/filter/get_ip_tbs?type=get_ip_tbs&tid=%s&pid=%s\
&user_name=%s&post_id=%s&word=%s&fid=%s&can_prison_ip=true&ie=utf-8"), tid, tid, EncodeURI(userName), 
		tid, EncodeURI(g_forumName), g_forumID);
	CString src = HTTPGet(url);
	CStringArray tmp;
	SplitString(tmp, src, _T(","));
	if (tmp.GetSize() < 6)
		return FALSE;
	result.ip_int = GetStringBetween(tmp[1], _T("\"ip_int\":\""), _T("\""));
	result.tbs_ban_user = GetStringBetween(tmp[2], _T("\"tbs_ban_user\":\""), _T("\""));
	result.tbs_ban_ip = GetStringBetween(tmp[3], _T("\"tbs_ban_ip\":\""), _T("\""));
	result.ip_secure_str = GetStringBetween(tmp[5], _T("\"ip_secure_str\":\""), _T("\""));
	return TRUE;
}

// 封ID，返回错误代码
CString BanID(LPCTSTR userName, LPCTSTR tbs_ban_user)
{
	CString data;
	data.Format(_T("cm=filter_forum_user&user_name=%s&ban_days=%d&word=%s&fid=%s&tbs=%s&ie=utf-8"), 
		EncodeURI(userName), g_banDuration, EncodeURI(g_forumName), g_forumID, tbs_ban_user);
	CString src = HTTPPost(_T("http://tieba.baidu.com/bawu/cm"), data);
	if (src == NET_TIMEOUT_TEXT /*|| src == NET_STOP_TEXT*/)
		return _T("-1");
	return GetStringBetween(src, _T("no\":"), _T(","));
}

// 封IP，返回错误代码 // 一直失败(错误代码3)，估计接口过期了，有空时再修复
CString BanIP(LPCTSTR ip_int, LPCTSTR tbs_ban_ip, LPCTSTR ip_secure_str)
{
	CString data;
	data.Format(_T("cm=filter_forum_ip&user_ip=%s&ip_secure_str=%s&ban_days=1&word=%s&fid=%s&tbs=%s&ie=utf-8"),
		ip_int, ip_secure_str, EncodeURI(g_forumName), g_forumID, tbs_ban_ip);
	CString src = HTTPPost(_T("http://tieba.baidu.com/bawu/cm"), data);
	if (src == NET_TIMEOUT_TEXT /*|| src == NET_STOP_TEXT*/)
		return _T("-1");
	return GetStringBetween(src, _T("no\":"), _T(","));
}

// 删主题，返回错误代码
CString DeleteThread(const CString& tid)
{
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/thread/delete"), _T("kw=") + EncodeURI(g_forumName) 
		+ _T("&fid=") + g_forumID + _T("&tid=") + tid + _T("&ie=utf-8&tbs=") + g_tbs);
	if (src == NET_TIMEOUT_TEXT /*|| src == NET_STOP_TEXT*/)
		return _T("-1");
	return GetStringBetween(src, _T("no\":"), _T(","));
}

// 删帖子，返回错误代码
CString DeletePost(LPCTSTR tid, LPCTSTR pid)
{
	CString data;
	data.Format(_T("commit_fr=pb&ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&is_vipdel=0&pid=%s&is_finf=false"), 
		g_tbs, EncodeURI(g_forumName), g_forumID, tid, pid);
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/post/delete"), data);
	if (src == NET_TIMEOUT_TEXT /*|| src == NET_STOP_TEXT*/)
		return _T("-1");
	return GetStringBetween(src, _T("no\":"), _T(","));
}

// 删楼中楼，返回错误代码
CString DeleteLZL(LPCTSTR tid, LPCTSTR lzlid)
{
	CString data;
	data.Format(_T("ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&pid=%s&is_finf=1"),
		g_tbs, EncodeURI(g_forumName), g_forumID, tid, lzlid);
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/post/delete"), data);
	if (src == NET_TIMEOUT_TEXT /*|| src == NET_STOP_TEXT*/)
		return _T("-1");
	return GetStringBetween(src, _T("no\":"), _T(","));
}

// 取错误文本
CString GetTiebaErrorText(const CString& errorCode)
{
	if (errorCode == _T("890"))
		return _T("贴子已删");
	if (errorCode == _T("-1"))
		return _T("超时");
	if (errorCode == _T("11"))
		return _T("度娘抽了");
	if (errorCode == _T("14") || errorCode == _T("12"))
		return _T("已被系统封禁");
	if (errorCode == _T("308"))
		return _T("你被封禁或失去权限");
	if (errorCode == _T("4"))
		return _T("小吧主只能封1天或小吧主不能封IP");
	if (errorCode == _T("872"))
		return _T("精品贴不能删");
	if (errorCode == _T("871"))
		return _T("高楼不能删");
	return _T("未知错误");
}