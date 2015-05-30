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
// 这些格式有些贴吧不同
/*const TCHAR FORUM_ID_LEFT[]		= _T("forum_id: \"");
const TCHAR FORUM_ID_RIGHT[]	= _T("\"");
const TCHAR FORUM_NAME1_LEFT[]	= _T("forum_name: '");
const TCHAR FORUM_NAME1_RIGHT[] = _T("'");
const TCHAR FORUM_NAME2_LEFT[]	= _T("\"forum_name\":\"");
const TCHAR FORUM_NAME2_RIGHT[] = _T("\"");
const TCHAR USER_NAME_LEFT[]	= _T("\"user_name\": \"");
const TCHAR USER_NAME_RIGHT[]	= _T("\"");*/
// 3是fid，7是贴吧名
const wregex FORUM_ID_NAME_REG(_T("PageData.forum.*?forum_id('|\")?:\\s*('|\")?(\\d+)('|\")?,\
.*?forum_name('|\")?:\\s*('|\")(.*?)('|\")"));
// 3是用户名
const wregex USER_NAME_REG(_T("PageData.*?user_name('|\")?:\\s*('|\")(.*?)('|\")"));
const TCHAR _TBS_LEFT[]			= _T("PageData.tbs = \"");
const TCHAR _TBS_RIGHT[]		= _T("\"");
#pragma endregion
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
#pragma endregion
#pragma region 楼中楼列表
const wregex LZL_FLOOR_REG(_T("\"(\\d+)\":.*?\"comment_info\":\\[(.*?)\\]"));
const wregex LZL_CONTENT_REG(_T("\"comment_id\":\"(\\d+)\".*?\"username\":\"(.*?)\".*?\"content\":\
\"(.*?[^\\\\])\""));
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

struct get_ip_tbs // 封禁用的tbs
{
	CString ip_int;
	CString tbs_ban_user; // 封ID用
	CString tbs_ban_ip; // 封IP用
	CString ip_secure_str; // 封IP用
};


// 采集 /////////////////////////////////////////////////////////////////////////////////
BOOL GetThreads(LPCTSTR forumName, LPCTSTR ignoreThread, vector<ThreadInfo>& threads);
enum GetPostsResult { GET_POSTS_SUCCESS, GET_POSTS_STOP, GET_POSTS_TIMEOUT, GET_POSTS_DELETED };
GetPostsResult GetPosts(const CString& tid, const CString& _src, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls);
void GetLzls(const CString& tid, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls);

// 扫描 /////////////////////////////////////////////////////////////////////////////////
extern volatile BOOL g_stopScanFlag;
extern CWinThread* g_scanThread;
extern set<__int64> g_ignoredTID; // 不删的主题ID(已扫描)
extern set<__int64> g_ignoredPID; // 不删的帖子ID(已扫描)
extern set<__int64> g_ignoredLZLID; // 不删的楼中楼ID(已扫描)
extern set<__int64> g_deletedTID; // 已删的主题ID
extern map<__int64, int> g_reply; // 主题的回复数
extern map<CString, int> g_IDTrigCount; // 某ID违规次数，已封为-1
BOOL CheckIllegal(LPCTSTR content, LPCTSTR author, CString& msg);
UINT AFX_CDECL ScanThread(LPVOID mainDlg);
void ScanPost(const ThreadInfo& thread, CTiebaManagerDlg* dlg, CComPtr<IHTMLDocument2>* pDocument);
BOOL ScanPostPage(const CString& tid, int page, const CString& title, BOOL hasHistoryReply,
	int ScanedCount, const CString& src, CTiebaManagerDlg* dlg, CComPtr<IHTMLDocument2>* pDocument);

// 操作 /////////////////////////////////////////////////////////////////////////////////
extern CWinThread* g_operateThread;
void AddOperation(const CString& msg, TBObject object, const CString& tid, const CString& title, 
	const CString& floor, const CString& pid, const CString& author);
UINT AFX_CDECL OperateThread(LPVOID mainDlg);
BOOL GetBanTBS(LPCTSTR tid, LPCTSTR userName, get_ip_tbs& result);
CString BanID(LPCTSTR userName, LPCTSTR tbs_ban_user);
//CString BanIP(LPCTSTR ip_int, LPCTSTR tbs_ban_ip, LPCTSTR ip_secure_str);
CString DeleteThread(const CString& tid);
CString DeletePost(LPCTSTR tid, LPCTSTR pid);
CString DeleteLZL(LPCTSTR tid, LPCTSTR lzlid);
CString GetTiebaErrorText(const CString& errorCode);
