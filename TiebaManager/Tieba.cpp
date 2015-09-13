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
#include "TiebaManagerDlg.h"
#include "Tieba.h"
#include <queue>
using std::queue;
#include "Global.h"
#include "Setting.h"
#include "ConfirmDlg.h"
#include <Mmsystem.h>
#include "ScanImage.h"


// 贴吧、用户信息
CString g_forumName;
CString g_encodedForumName;
CString g_forumID;
CString g_cookie;
CString g_tbs;


// 采集贴吧用的常量
// 正则表达式太慢所以不用
#pragma region 主题列表
// 今日话题
const TCHAR TOPIC_LEFT[] = _T("<ul id=\"thread_topic");
const TCHAR TOPIC_RIGHT[] = _T("</ul>");
const TCHAR TOPIC_TID_LEFT[] = _T("href=\"http://tieba.baidu.com/p/");
const TCHAR TOPIC_TID_RIGHT[] = _T("\"");
const TCHAR TOPIC_REPLY_LEFT[] = _T("title=\"");
const TCHAR TOPIC_REPLY_RIGHT[] = _T("个回复\"");
const TCHAR TOPIC_TITLE_LEFT[] = _T("title=\"");
const TCHAR TOPIC_TITLE_RIGHT[] = _T("\"");
const TCHAR TOPIC_PREVIEW_LEFT[] = _T("class=\"threadlist_abs\">");
const TCHAR TOPIC_PREVIEW_RIGHT[] = _T("</div>");

// 普通主题
const TCHAR THREAD_SPLIT[] = _T("data-field='{&quot;author_name&quot;:&quot;");
const TCHAR THREAD_END[] = _T("<div id=\"frs_list_pager\"");
const TCHAR THREAD_TID_LEFT[] = _T("&quot;id&quot;:");
const TCHAR THREAD_TID_RIGHT[] = _T(",");
const TCHAR THREAD_REPLY_LEFT[] = _T("&quot;reply_num&quot;:");
const TCHAR THREAD_REPLY_RIGHT[] = _T(",");
const TCHAR THREAD_TITLE_LEFT[] = _T("class=\"j_th_tit\">");
const TCHAR THREAD_TITLE_RIGHT[] = _T("</a>");
const TCHAR THREAD_PREVIEW_LEFT[] = _T("threadlist_abs_onlyline\">");
const TCHAR THREAD_PREVIEW_RIGHT[] = _T("</div>");
const TCHAR THREAD_MEDIA_LEFT[] = _T("<ul class=\"threadlist_media");
const TCHAR THREAD_MEDIA_RIGHT[] = _T("</ul>");
const TCHAR THREAD_AUTHOR_LEFT[] = _T("&quot;author_name&quot;:&quot;");
const TCHAR THREAD_AUTHOR_RIGHT[] = _T("&quot;");
#pragma endregion
#pragma region 帖子列表
const TCHAR PAGE_COUNT_LEFT[] = _T(",\"total_page\":");
const TCHAR PAGE_COUNT_RIGHT[] = _T("}");

const TCHAR POST_SPLIT[] = _T("data-field='{&quot;author&quot;:");
const TCHAR POST_PID_LEFT[] = _T("&quot;post_id&quot;:");
const TCHAR POST_PID_RIGHT[] = _T(",");
const TCHAR POST_FLOOR_LEFT[] = _T("&quot;post_no&quot;:");
const TCHAR POST_FLOOR_RIGHT[] = _T(",");
const TCHAR POST_AUTHOR_LEFT[] = _T("&quot;user_name&quot;:&quot;");
const TCHAR POST_AUTHOR_RIGHT[] = _T("&quot;");
const TCHAR POST_CONTENT_LEFT[] = _T("<cc>");
const TCHAR POST_CONTENT_RIGHT[] = _T("</cc>");
const TCHAR POST_SIGN_LEFT[] = _T("<img class=\"j_user_sign\"");
const TCHAR POST_SIGN_RIGHT[] = _T("/>");
#pragma endregion
#pragma region 楼中楼列表
const wregex LZL_FLOOR_REG(_T("\"(\\d+)\":.*?\"comment_info\":\\[(.*?)during_time\":\\d+\\}\\]"));

const TCHAR LZL_SPLIT[] = _T("{\"thread_id\"");
const TCHAR LZL_PID_LEFT[] = _T("\"comment_id\":\"");
const TCHAR LZL_PID_RIGHT[] = _T("\"");
const TCHAR LZL_AUTHOR_LEFT[] = _T("\"username\":\"");
const TCHAR LZL_AUTHOR_RIGHT[] = _T("\"");
const TCHAR LZL_CONTENT_LEFT[] = _T("\"content\":\"");
const TCHAR LZL_CONTENT_RIGHT[] = _T("\",\"");
#pragma endregion


volatile BOOL g_stopScanFlag = FALSE;
CWinThread* g_scanThread = NULL;
set<__int64> g_initIgnoredTID; // 不删的主题ID(手动忽略)，要写入文件
set<__int64> g_initIgnoredPID; // 不删的帖子ID(手动忽略)，要写入文件
set<__int64> g_initIgnoredLZLID; // 不删的楼中楼ID(手动忽略)，要写入文件
set<__int64> g_ignoredTID; // 不删的主题ID(已扫描且违规)
set<__int64> g_ignoredPID; // 不删的帖子ID(已扫描且违规)
set<__int64> g_ignoredLZLID; // 不删的楼中楼ID(已扫描且违规)
set<__int64> g_deletedTID; // 已删的主题ID
map<__int64, int> g_reply; // 主题的回复数
map<CString, int> g_IDTrigCount; // 某ID违规次数，已封为-1

static vector<ThreadInfo> g_threads; // 当前扫描的主题列表
static int g_threadIndex; // 下个要扫描的主题索引
static CCriticalSection g_threadIndexLock;

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
			WriteString(src, _T("forum.txt"));
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
		threads[iThreads].preview = HTMLUnescape(GetStringBetween(rawThreads[iRawThreads], THREAD_PREVIEW_LEFT, THREAD_PREVIEW_RIGHT))
			+ _T("\r\n") + GetStringBetween2(rawThreads[iRawThreads], THREAD_MEDIA_LEFT, THREAD_MEDIA_RIGHT);
		threads[iThreads].author = JSUnescape(GetStringBefore(rawThreads[iRawThreads], THREAD_AUTHOR_RIGHT));

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
		//posts[iPosts].content = GetStringBetween(rawPosts[iRawPosts], POST_CONTENT_LEFT, POST_CONTENT_RIGHT);
		
		int left = rawPosts[iRawPosts].Find(POST_CONTENT_LEFT) + _tcslen(POST_CONTENT_LEFT);
		left = rawPosts[iRawPosts].Find(_T(">"), left) + 1;
		// 去掉首空格
		while (left < rawPosts[iRawPosts].GetLength() && rawPosts[iRawPosts][left] == _T(' '))
			left++;
		int right = rawPosts[iRawPosts].Find(POST_CONTENT_RIGHT, left + 1);
		// CString不支持反向查找字符串？
		posts[iPosts].content = rawPosts[iRawPosts].Mid(left, right - left);
		LPCTSTR pos = StrRStrI(posts[iPosts].content, NULL, _T("</div>"));
		if (pos != NULL)
		{
			right = ((DWORD)pos - (DWORD)(LPCTSTR)posts[iPosts].content) / sizeof(TCHAR) - 1;
			// 去掉尾空格
			while (right >= 0 && posts[iPosts].content[right] == _T(' '))
				right--;
			posts[iPosts].content = posts[iPosts].content.Left(right + 1);
		}
		// 签名档
		posts[iPosts].content += _T("\r\n") + GetStringBetween2(rawPosts[iRawPosts], POST_SIGN_LEFT, POST_SIGN_RIGHT);

		//OutputDebugString(_T("\n"));
		//OutputDebugString(rawPosts[iRawPosts]);
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
	int iLzls = 0;
	// 遍历楼层
	for (std::regex_iterator<LPCTSTR> it((LPCTSTR)src, (LPCTSTR)src + src.GetLength(), LZL_FLOOR_REG), end; it != end; it++)
	{
		// 查找该层楼层
		CString pid = (*it)[1].str().c_str(); // 该层PID
		CString floor;
		for (PostInfo post : posts)
			if (post.pid == pid)
			{
				floor = post.floor;
				break;
			}
		
		// 遍历该层楼中楼
		CStringArray rawLzls;
		SplitString(rawLzls, (*it)[2].str().c_str(), LZL_SPLIT);
		lzls.resize(lzls.size() + rawLzls.GetSize() - 1);
		for (int iRawLzls = 1; iRawLzls < rawLzls.GetSize(); iRawLzls++, iLzls++)
		{
			lzls[iLzls].pid = GetStringBetween(rawLzls[iRawLzls], LZL_PID_LEFT, LZL_PID_RIGHT);
			lzls[iLzls].floor = floor;
			lzls[iLzls].author = JSUnescape(GetStringBetween(rawLzls[iRawLzls], LZL_AUTHOR_LEFT, LZL_AUTHOR_RIGHT));
			lzls[iLzls].content = HTMLUnescape(JSUnescape(GetStringBetween(rawLzls[iRawLzls], LZL_CONTENT_LEFT, LZL_CONTENT_RIGHT)));
		}

		//OutputDebugString(_T("\n"));
		//OutputDebugString(floorContent);
		//OutputDebugString(_T("\n----------------------------------"));
	}
}


// 扫描 /////////////////////////////////////////////////////////////////////////////////
// 检查违规
BOOL CheckIllegal(LPCTSTR content, LPCTSTR author, CString& msg, int& pos, int& length)
{
	g_optionsLock.Lock();

	// 信任用户
	if (g_whiteList.find(author) != g_whiteList.end())
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
		if (StringIncludes(content, keyword, &pos, &length))
		{
			msg = _T("<font color=red> 触发违禁词 </font>") + HTMLEscape(keyword.text);
			g_optionsLock.Unlock();
			return TRUE;
		}

	// 屏蔽用户
	for (const RegexText& blackList : g_blackList)
		if (StringMatchs(author, blackList))
		{
			pos = 0;
			length = 0;
			msg = _T("<font color=red> 触发屏蔽用户 </font>") + HTMLEscape(blackList.text);
			g_optionsLock.Unlock();
			return TRUE;
		}

	g_optionsLock.Unlock();
	return FALSE;
}

// 扫描主题图片
static inline void ScanThreadImage(CString& msg, CTiebaManagerDlg* dlg, CComPtr<IHTMLDocument2>* pDocument)
{
	for (const ThreadInfo& thread : g_threads)
	{
		if (g_stopScanFlag)
			break;
		__int64 tid = _ttoi64(thread.tid);
		if (g_ignoredTID.find(tid) == g_ignoredTID.end()
			&& CheckImageIllegal(thread.preview, thread.author, GetThreadImage, msg))
		{
			AddOperation(thread.title + _T("\r\n") + thread.preview, TBOBJ_THREAD, thread.tid,
				thread.title, _T("1"), _T(""), thread.author);
			dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">")
				+ HTMLEscape(thread.title) + _T("</a>") + msg, pDocument);
			g_ignoredTID.insert(tid);
		}
	}
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
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		CString code;
		code.Format(_T("CoInitializeEx: 0x%08X"), hr);
		WriteString(code, _T("error.txt"));
	}
	CComPtr<IHTMLDocument2> document;
	dlg->GetLogDocument(document);
	CComPtr<IHTMLDocument2>* pDocument = (CComPtr<IHTMLDocument2>*)&(int&)document;

	// 初始化页数
	CString sPage;
	dlg->m_pageEdit.GetWindowText(sPage);
	int iPage = _ttoi(sPage);
	CString ignoreThread; // 忽略前几个主题
	ignoreThread.Format(_T("%d"), (iPage - 1) * 50);

	CString msg;
	int pos, length;
	while (!g_stopScanFlag)
	{
		DWORD startTime = GetTickCount();
		dlg->m_stateStatic.SetWindowText(_T("扫描主题中"));
		if (!g_briefLog)
			dlg->Log(_T("<font color=green>本轮扫描开始，使用方案：</font>") + g_currentOption, pDocument);
		
		// 获取主题列表
		if (!GetThreads(g_forumName, ignoreThread, g_threads))
		{
			if (g_stopScanFlag)
				break;
			if (!g_briefLog)
				dlg->Log(_T("<font color=red>获取主题列表失败，重新开始本轮</font>"), pDocument);
			continue;
		}

		// 扫描主题
		for (const ThreadInfo& thread : g_threads)
		{
			if (g_stopScanFlag)
				break;
			__int64 tid = _ttoi64(thread.tid);
			if (g_ignoredTID.find(tid) == g_ignoredTID.end()
				&& CheckIllegal(thread.title + _T("\r\n") + thread.preview, thread.author, msg, pos, length))
			{
				AddOperation(thread.title + _T("\r\n") + thread.preview, TBOBJ_THREAD, thread.tid, 
					thread.title, _T("0"), _T(""), thread.author, pos, length);
				dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">")
					+ HTMLEscape(thread.title) + _T("</a>") + msg, pDocument);
				g_ignoredTID.insert(tid);
			}
		}
		// 扫描主题图片
		BOOL scanImage = !g_images.empty();
		if (g_onlyScanTitle && scanImage)
		{
			scanImage = FALSE;
			ScanThreadImage(msg, dlg, pDocument);
		}

		// 扫描帖子
		if (!g_onlyScanTitle)
		{
			dlg->m_stateStatic.SetWindowText(_T("扫描帖子中"));
			g_threadIndex = 0;
			// 创建线程扫描帖子
			int threadCount = g_threadCount; // g_threadCount会变
			CWinThread** threadObjects = new CWinThread*[threadCount];
			HANDLE* threadHandles = new HANDLE[threadCount];
			for (int i = 0; i < threadCount; i++)
			{
				dlg->m_stateList.AddString(_T("准备中"));
				threadObjects[i] = AfxBeginThread(ScanPostThread, (LPVOID)i, 0, 0, CREATE_SUSPENDED);
				threadObjects[i]->m_bAutoDelete = FALSE;
				threadHandles[i] = threadObjects[i]->m_hThread;
				threadObjects[i]->ResumeThread();
			}
			// 等待扫描帖子时扫描主题图片
			if (scanImage)
				ScanThreadImage(msg, dlg, pDocument);
			WaitForMultipleObjects(threadCount, threadHandles, TRUE, INFINITE);

			// 释放线程
			for (int i = 0; i < threadCount; i++)
			{
				//CloseHandle(threadHandles[i]); // 下面这句自动释放？，加上会异常
				delete threadObjects[i];
			}
			delete threadHandles;
			delete threadObjects;
			dlg->m_stateList.ResetContent();
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
	g_stopScanFlag = FALSE;

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

// 扫描帖子线程
UINT AFX_CDECL ScanPostThread(LPVOID _threadID)
{
	int threadID = (int)_threadID;
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd;
	// 初始化日志文档
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CComPtr<IHTMLDocument2> document;
	dlg->GetLogDocument(document);
	CComPtr<IHTMLDocument2>* pDocument = (CComPtr<IHTMLDocument2>*)&(int&)document;

	CString pageCount, src;
	map<__int64, int>::iterator historyReplyIt;
	g_threadIndexLock.Lock();
	while (!g_stopScanFlag && g_threadIndex < (int)g_threads.size())
	{
		ThreadInfo& thread = g_threads[g_threadIndex++];
		g_threadIndexLock.Unlock();
		if (g_deletedTID.find(_ttoi64(thread.tid)) != g_deletedTID.end()) // 已删
			goto next;
		g_optionsLock.Lock();
		if (g_trustedThread.find(thread.tid) != g_trustedThread.end()) // 信任
		{
			g_optionsLock.Unlock();
			goto next;
		}
		g_optionsLock.Unlock();

		__int64 tid = _ttoi64(thread.tid);
		int reply = _ttoi(thread.reply);
		historyReplyIt = g_reply.find(tid);
		BOOL hasHistoryReply = historyReplyIt != g_reply.end();
		if (hasHistoryReply && reply <= historyReplyIt->second) // 无新回复
		{
			historyReplyIt->second = reply;
			goto next;
		}

		// 第一页
		src = HTTPGet(_T("http://tieba.baidu.com/p/" + thread.tid), FALSE, &g_stopScanFlag);
		if (src == NET_STOP_TEXT)
			goto next;
		if (src == NET_TIMEOUT_TEXT)
		{
			if (!g_briefLog)
				dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">") + thread.title
				+ _T("</a> <font color=red>获取贴子列表失败(超时)，暂时跳过</font>"), pDocument);
			goto next;
		}

		// 帖子页数
		pageCount = GetStringBetween(src, PAGE_COUNT_LEFT, PAGE_COUNT_RIGHT);
		if (pageCount == _T(""))
		{
			WriteString(src, _T("thread.txt"));
			if (!g_briefLog)
				dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">") + thread.title
				+ _T("</a> <font color=red>获取贴子列表失败(可能已被删)，暂时跳过</font>"), pDocument);
			goto next;
		}

		// 扫描帖子页
		int iPageCount = _ttoi(pageCount);
		BOOL res = ScanPostPage(thread.tid, 1, thread.title, hasHistoryReply, 0, src, threadID, dlg, pDocument);
		if (iPageCount > 1 && !g_stopScanFlag)
			res = ScanPostPage(thread.tid, iPageCount, thread.title, hasHistoryReply, 0, _T(""), threadID, dlg, pDocument);

		// 记录历史回复
		if (res)
			g_reply[tid] = reply;

next:
		g_threadIndexLock.Lock();
	}
	g_threadIndexLock.Unlock();
	
	CoUninitialize();
	dlg->m_stateList.DeleteString(threadID);
	dlg->m_stateList.InsertString(threadID, _T("线程结束"));
	return 0;
}

// 扫描帖子页
BOOL ScanPostPage(const CString& tid, int page, const CString& title, BOOL hasHistoryReply, 
	int ScanedCount, const CString& src, int threadID, CTiebaManagerDlg* dlg, CComPtr<IHTMLDocument2>* pDocument)
{
	CString sPage;
	sPage.Format(_T("%d"), page);
	dlg->m_stateList.DeleteString(threadID);
	dlg->m_stateList.InsertString(threadID, tid + _T(":") + sPage + _T(" ") + title);

	// 获取帖子列表
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
	int pos, length;
	// 扫描帖子
	for (const PostInfo& post : posts)
	{
		if (g_stopScanFlag)
			return FALSE;
		__int64 pid = _ttoi64(post.pid);
		if (g_ignoredPID.find(pid) == g_ignoredPID.end() 
			&& CheckIllegal(post.content, post.author, msg, pos, length))
		{
			AddOperation(post.content, post.floor == _T("1") ? TBOBJ_THREAD : TBOBJ_POST, 
				tid, title, post.floor, post.pid, post.author, pos, length);
			dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) + 
				_T("</a> ") + post.floor + _T("楼") + msg, pDocument);
			g_ignoredPID.insert(pid);
		}
	}

	// 扫描楼中楼
	for (const PostInfo& lzl : lzls)
	{
		if (g_stopScanFlag)
			return FALSE;
		if (CheckIllegal(lzl.content, lzl.author, msg, pos, length))
		{
			__int64 lzlid = _ttoi64(lzl.pid);
			if (g_ignoredLZLID.find(lzlid) == g_ignoredLZLID.end())
			{
				AddOperation(lzl.content, TBOBJ_LZL, tid, title, lzl.floor, lzl.pid, lzl.author, pos, length);
				dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) +
					_T("</a> ") + lzl.floor + _T("楼回复") + msg, pDocument);
				g_ignoredLZLID.insert(lzlid);
			}
		}
	}

	// 扫描帖子图片
	for (const PostInfo& post : posts)
	{
		if (g_stopScanFlag)
			return FALSE;
		__int64 pid = _ttoi64(post.pid);
		if (g_ignoredPID.find(pid) == g_ignoredPID.end()
			&& CheckImageIllegal(post.content, post.author, GetPostImage, msg))
		{
			AddOperation(post.content, post.floor == _T("1") ? TBOBJ_THREAD : TBOBJ_POST,
				tid, title, post.floor, post.pid, post.author);
			dlg->Log(_T("<a href=\"http://tieba.baidu.com/p/") + tid + _T("\">") + HTMLEscape(title) +
				_T("</a> ") + post.floor + _T("楼") + msg, pDocument);
			g_ignoredPID.insert(pid);
		}
	}

	// 递归扫描上一页
	if (!hasHistoryReply) // 如果有历史回复前面几页很可能被扫描过了，不递归
	{
		if (++ScanedCount < g_scanPageCount) // 没达到最大扫描页数
		{
			if (--page < 2) // 扫描完
				return TRUE;
			return ScanPostPage(tid, page, title, FALSE, ScanedCount, _T(""), threadID, dlg, pDocument);
		}
	}
	return TRUE;
}


// 操作 /////////////////////////////////////////////////////////////////////////////////
// 添加操作
void AddOperation(const CString& msg, TBObject object, const CString& tid, const CString& title,
	const CString& floor, const CString& pid, const CString& author, int pos, int length)
{
	Operation tmp;
	tmp.msg = msg;
	tmp.pos = pos;
	tmp.length = length;
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
	dlg->GetLogDocument(document);
	CComPtr<IHTMLDocument2>* pDocument = (CComPtr<IHTMLDocument2>*)&(int&)document;

	while (!g_operationQueue.empty() && !g_stopScanFlag)
	{
		Operation op = g_operationQueue.front();
		g_operationQueueLock.Lock();
		g_operationQueue.pop();
		g_operationQueueLock.Unlock();

		// 没有操作
		if (!g_delete && !g_banID)
			continue;

		// 主题已被删
		__int64 tid = _ttoi64(op.tid);
		if (g_deletedTID.find(tid) != g_deletedTID.end())
			continue;
		
		// 确认是否操作
		if (g_confirm)
		{
			if (CConfirmDlg(&op).DoModal() == IDCANCEL)
			{
				switch (op.object)
				{
				case TBOBJ_THREAD:
					if (op.floor == _T("1"))
						goto casePost;
					g_initIgnoredTID.insert(tid);
					dlg->Log(_T("<font color=green>忽略 </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a>"), pDocument);
					break;
				case TBOBJ_POST:
casePost:			g_initIgnoredPID.insert(_ttoi64(op.pid));
					dlg->Log(_T("<font color=green>忽略 </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("楼"), pDocument);
					break;
				case TBOBJ_LZL:
					g_initIgnoredLZLID.insert(_ttoi64(op.pid));
					dlg->Log(_T("<font color=green>忽略 </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("楼回复"), pDocument);
					break;
				}
				continue;
			}
		}

		// 封禁
		if (g_banID)
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
				if (op.pid == _T(""))
				{
					vector<PostInfo> posts, lzls;
					GetPosts(op.tid, _T(""), _T("1"), posts, lzls);
					if (posts.size() > 0)
						op.pid = posts[0].pid;
				}
				if (op.pid == _T(""))
				{
					dlg->Log(_T("<font color=red>封禁 </font>") + op.author + _T("<font color=red> 失败！\
(获取帖子ID失败)</font>"), pDocument);
				}
				else
				{
					CString code = BanID(op.author, op.pid);
					if (code != _T("0"))
					{
						CString content;
						content.Format(_T("<font color=red>封禁 </font>%s<font color=red> 失败！\
错误代码：%s(%s)</font><a href=\"BD:%s,%s\">重试</a>"), op.author, code, GetTiebaErrorText(code), op.pid, op.author);
						dlg->Log(content, pDocument);
					}
					else
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
		if (!g_delete)
			continue;
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

// 取错误代码
static inline CString GetOperationErrorCode(const CString& src)
{
	if (src == NET_TIMEOUT_TEXT /*|| src == NET_STOP_TEXT*/)
		return _T("-1");
	CString code = GetStringBetween(src, _T("no\":"), _T(","));
	if (code != _T("0"))
		WriteString(src, _T("operation.txt"));
	return code;
}

// 封ID，返回错误代码
CString BanID(LPCTSTR userName, LPCTSTR pid)
{
	CString data;
	data.Format(_T("day=%d&fid=%s&tbs=%s&ie=gbk&user_name%%5B%%5D=%s&pid%%5B%%5D=%s&reason=%s"), 
		g_banDuration, g_forumID, g_tbs, EncodeURI(userName), pid, g_banReason != _T("") ? g_banReason : _T(" "));
	CString src = HTTPPost(_T("http://tieba.baidu.com/pmc/blockid"), data);
	return GetOperationErrorCode(src);
}

// 删主题，返回错误代码
CString DeleteThread(const CString& tid)
{
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/thread/delete"), _T("kw=") + g_encodedForumName
		+ _T("&fid=") + g_forumID + _T("&tid=") + tid + _T("&ie=utf-8&tbs=") + g_tbs);
	return GetOperationErrorCode(src);
}

// 删帖子，返回错误代码
CString DeletePost(LPCTSTR tid, LPCTSTR pid)
{
	CString data;
	data.Format(_T("commit_fr=pb&ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&is_vipdel=0&pid=%s&is_finf=false"), 
		g_tbs, g_encodedForumName, g_forumID, tid, pid);
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/post/delete"), data);
	return GetOperationErrorCode(src);
}

// 删楼中楼，返回错误代码
CString DeleteLZL(LPCTSTR tid, LPCTSTR lzlid)
{
	CString data;
	data.Format(_T("ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&pid=%s&is_finf=1"),
		g_tbs, g_encodedForumName, g_forumID, tid, lzlid);
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/post/delete"), data);
	return GetOperationErrorCode(src);
}

// 取错误文本
CString GetTiebaErrorText(const CString& errorCode)
{
	if (errorCode == _T("890"))
		return _T("贴子已删");
	if (errorCode == _T("-1"))
		return _T("超时");
	if (errorCode == _T("78"))
		return _T("参数错误");
	if (errorCode == _T("4"))
		return _T("参数校验失败");
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
