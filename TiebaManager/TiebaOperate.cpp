#include "stdafx.h"
#include "TiebaOperate.h"
#include <queue>
using std::queue;
#include "TiebaCollect.h"
#include "TiebaScan.h"
#include "StringHelper.h"
#include "NetworkHelper.h"
#include "Setting.h"
#include "TiebaManagerDlg.h"
#include "ConfirmDlg.h"
#include <Mmsystem.h>


extern volatile BOOL g_stopScanFlag;


queue<Operation> g_operationQueue; // 操作队列
static CCriticalSection g_operationQueueLock;
CWinThread* g_operateThread = NULL;


// 添加操作
void AddOperation(const CString& msg, TBObject object, const CString& tid, const CString& title,
	const CString& floor, const CString& pid, const CString& author, const CString& authorID,
	const CString& authorPortrait, int pos, int length)
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
	tmp.authorID = authorID;
	tmp.authorPortrait = authorPortrait;
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
		if (!g_delete && !g_banID && !g_defriend)
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
				casePost : g_initIgnoredPID.insert(_ttoi64(op.pid));
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

		// 增加违规次数
		auto countIt = g_userTrigCount.find(op.author);
		BOOL hasHistory = countIt != g_userTrigCount.end();
		int count = hasHistory ? (countIt->second + 1) : 1;
		if (hasHistory)
			countIt->second = count;
		else
			g_userTrigCount[op.author] = 1;

		// 封禁
		if (g_banID && count >= g_banTrigCount && g_bannedUser.find(op.author) == g_bannedUser.end()) // 达到封禁违规次数且未封
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
				dlg->Log(_T("<font color=red>封禁 </font>") + op.author + _T("<font color=red> 失败！(获取帖子ID失败)</font>")
						 , pDocument);
			}
			else
			{
				CString code = BanID(op.author, op.pid);
				if (code != _T("0"))
				{
					CString content;
					content.Format(_T("<font color=red>封禁 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
								   _T("\"bd:%s,%s\">重试</a>"), op.author, code, GetTiebaErrorText(code), op.pid, op.author);
					dlg->Log(content, pDocument);
				}
				else
				{
					sndPlaySound(_T("封号.wav"), SND_ASYNC | SND_NODEFAULT);
					g_bannedUser.insert(op.author);
					dlg->Log(_T("<font color=red>封禁 </font>") + op.author, pDocument);
				}
			}
		}

		// 拉黑
		if (g_defriend && count >= g_defriendTrigCount && g_defriendedUser.find(op.author) == g_defriendedUser.end()) // 达到拉黑违规次数且未拉黑
		{
			CString code = Defriend(op.authorID);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<font color=red>拉黑 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
							   _T("\"df:%s\">重试</a>"), op.author, code, GetTiebaErrorText(code), op.authorID);
				dlg->Log(content, pDocument);
			}
			else
			{
				sndPlaySound(_T("封号.wav"), SND_ASYNC | SND_NODEFAULT);
				g_defriendedUser.insert(op.author);
				dlg->Log(_T("<font color=red>拉黑 </font>") + op.author, pDocument);
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
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a><font color=red> 删除失败！错误代码：%s(%s)</font><a href=")
							   _T("\"dt:%s\">重试</a>"), op.tid, HTMLEscape(op.title), code, GetTiebaErrorText(code), op.tid);
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
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a> %s楼<font color=red> 删除失败！错误代码：%s(%s)</font>")
							   _T("<a href=\"dp:%s,%s\">重试</a>"), op.tid, HTMLEscape(op.title), op.floor, code, GetTiebaErrorText(code), 
							   op.tid, op.pid);
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
				content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a> %s楼回复<font color=red> 删除失败！错误代码：")
							   _T("%s(%s)</font><a href=\"dl:%s,%s\">重试</a>"), op.tid, HTMLEscape(op.title), op.floor, code,
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
		return _T("-65536");
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

// 拉黑，返回错误代码
CString Defriend(LPCTSTR userID)
{
	CString src = HTTPPost(_T("http://tieba.baidu.com/bawu2/platform/addBlack"), _T("ie=utf-8&tbs=") + g_tbs
		+ _T("&user_id=") + userID + _T("&word=") + g_encodedForumName);
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
	if (errorCode == _T("-65536"))
		return _T("超时");
	if (errorCode == _T("-1"))
		return _T("权限不足");
	if (errorCode == _T("4"))
		return _T("参数校验失败");
	if (errorCode == _T("11"))
		return _T("度娘抽了");
	if (errorCode == _T("14") || errorCode == _T("12"))
		return _T("已被系统封禁");
	if (errorCode == _T("74"))
		return _T("用户不存在(可能循环封用的帖子被删除，要重新添加)");
	if (errorCode == _T("78"))
		return _T("参数错误");
	if (errorCode == _T("308"))
		return _T("你被封禁或失去权限");
	if (errorCode == _T("871"))
		return _T("高楼不能删");
	if (errorCode == _T("872"))
		return _T("精品贴不能删");
	if (errorCode == _T("890"))
		return _T("贴子已删");
	if (errorCode == _T("4011"))
		return _T("需要验证码(操作太快？)");
	return _T("未知错误");
}
