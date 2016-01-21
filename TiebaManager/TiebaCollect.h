#pragma once


// 采集贴吧用的常量
#pragma region 用户信息
// 3是fid，7是贴吧名
const wregex FORUM_ID_NAME_REG(_T("id('|\")?:\\s*('|\")?(\\d+)('|\")?,")
							   _T(".*?name('|\")?:\\s*('|\")(.*?)('|\")"));
// 3是用户名
const wregex USER_NAME_REG(_T("name('|\")?:\\s*('|\")(.*?)('|\")"));
const TCHAR _TBS_LEFT[] = _T("PageData.tbs = \"");
const TCHAR _TBS_RIGHT[] = _T("\"");
// 3是tbs
const wregex TBS_REG(_T("tbs('|\")?:\\s*('|\")(.*?)('|\")"));
#pragma endregion


struct ThreadInfo // 主题信息
{
	CString tid;		// 主题ID
	CString reply;		// 回复数
	CString title;		// 标题
	CString preview;	// 预览
	CString author;		// 作者
	CString authorID;	// 作者ID
};

struct PostInfo // 帖子信息
{
	CString pid;		// 帖子ID
	CString floor;		// 楼层
	CString author;		// 作者
	CString authorID;	// 作者ID
	CString authorPortrait;	// 作者头像哈希
	CString content;	// 内容
};


BOOL GetThreads(LPCTSTR forumName, LPCTSTR ignoreThread, vector<ThreadInfo>& threads);
enum GetPostsResult { GET_POSTS_SUCCESS, GET_POSTS_STOP, GET_POSTS_TIMEOUT, GET_POSTS_DELETED };
GetPostsResult GetPosts(const CString& tid, const CString& _src, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls);
void GetLzls(const CString& tid, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls);

CString GetPIDFromUser(const CString& userName);
