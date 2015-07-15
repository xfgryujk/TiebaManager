#pragma once
#include "TiebaManagerDlg.h"
#include <regex>
using std::wregex;
#include <vector>
using std::vector;
#include <set>
using std::set;
#include <map>
using std::map;


// 贴吧、用户信息
extern CString g_forumName;
extern CString g_encodedForumName;
extern CString g_forumID;
extern CString g_cookie;
extern CString g_tbs; // 备用获取tbs地址：http://tieba.baidu.com/dc/common/tbs


// 采集贴吧用的常量
#pragma region 用户信息
// 3是fid，7是贴吧名
const wregex FORUM_ID_NAME_REG(_T("id('|\")?:\\s*('|\")?(\\d+)('|\")?,\
.*?name('|\")?:\\s*('|\")(.*?)('|\")"));
// 3是用户名
const wregex USER_NAME_REG(_T("name('|\")?:\\s*('|\")(.*?)('|\")"));
const TCHAR _TBS_LEFT[] = _T("PageData.tbs = \"");
const TCHAR _TBS_RIGHT[] = _T("\"");
#pragma endregion


struct ThreadInfo // 主题信息
{
	CString tid;		// 主题ID
	CString reply;		// 回复数
	CString title;		// 标题
	CString preview;	// 预览
	CString author;		// 作者
};

struct PostInfo // 帖子信息
{
	CString pid;		// 帖子ID
	CString floor;		// 楼层
	CString author;		// 作者
	CString content;	// 内容
};

enum TBObject { TBOBJ_THREAD, TBOBJ_POST, TBOBJ_LZL };


// 采集 /////////////////////////////////////////////////////////////////////////////////
BOOL GetThreads(LPCTSTR forumName, LPCTSTR ignoreThread, vector<ThreadInfo>& threads);
enum GetPostsResult { GET_POSTS_SUCCESS, GET_POSTS_STOP, GET_POSTS_TIMEOUT, GET_POSTS_DELETED };
GetPostsResult GetPosts(const CString& tid, const CString& _src, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls);
void GetLzls(const CString& tid, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls);

// 扫描 /////////////////////////////////////////////////////////////////////////////////
extern volatile BOOL g_stopScanFlag;
extern CWinThread* g_scanThread;
extern set<__int64> g_initIgnoredTID; // 不删的主题ID(手动忽略)，要写入文件
extern set<__int64> g_initIgnoredPID; // 不删的帖子ID(手动忽略)，要写入文件
extern set<__int64> g_initIgnoredLZLID; // 不删的楼中楼ID(手动忽略)，要写入文件
extern set<__int64> g_ignoredTID; // 不删的主题ID(已扫描且违规)
extern set<__int64> g_ignoredPID; // 不删的帖子ID(已扫描且违规)
extern set<__int64> g_ignoredLZLID; // 不删的楼中楼ID(已扫描且违规)
extern set<__int64> g_deletedTID; // 已删的主题ID
extern map<__int64, int> g_reply; // 主题的回复数
extern map<CString, int> g_IDTrigCount; // 某ID违规次数，已封为-1
BOOL CheckIllegal(LPCTSTR content, LPCTSTR author, CString& msg);
UINT AFX_CDECL ScanThread(LPVOID mainDlg);
UINT AFX_CDECL ScanPostThread(LPVOID threadID);
BOOL ScanPostPage(const CString& tid, int page, const CString& title, BOOL hasHistoryReply,
	int ScanedCount, const CString& src, int threadID, CTiebaManagerDlg* dlg, CComPtr<IHTMLDocument2>* pDocument);

// 操作 /////////////////////////////////////////////////////////////////////////////////
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
extern CWinThread* g_operateThread;
void AddOperation(const CString& msg, TBObject object, const CString& tid, const CString& title, 
	const CString& floor, const CString& pid, const CString& author);
UINT AFX_CDECL OperateThread(LPVOID mainDlg);
CString BanID(LPCTSTR userName, LPCTSTR pid);
CString DeleteThread(const CString& tid);
CString DeletePost(LPCTSTR tid, LPCTSTR pid);
CString DeleteLZL(LPCTSTR tid, LPCTSTR lzlid);
CString GetTiebaErrorText(const CString& errorCode);
