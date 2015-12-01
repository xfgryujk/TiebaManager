#include "stdafx.h"
#include "TiebaVariable.h"
#include "TiebaOperate.h"


// 贴吧、用户信息
CString g_forumName;
CString g_encodedForumName;
CString g_forumID;
CString g_cookie;
CString g_tbs;

// 扫描
volatile BOOL g_stopScanFlag = FALSE;
CWinThread* g_scanThread = NULL;
set<__int64> g_initIgnoredTID; // 不删的主题ID(手动忽略)，要写入文件
set<__int64> g_initIgnoredPID; // 不删的帖子ID(手动忽略)，要写入文件
set<__int64> g_initIgnoredLZLID; // 不删的楼中楼ID(手动忽略)，要写入文件
set<__int64> g_ignoredTID; // 不删的主题ID(已扫描且违规)
set<__int64> g_ignoredPID; // 不删的帖子ID(已扫描且违规)
set<__int64> g_ignoredLZLID; // 不删的楼中楼ID(已扫描且违规)
set<__int64> g_deletedTID; // 已删的主题ID
map<__int64, int> g_reply; // 主题的回复数，要写入文件
map<CString, int> g_userTrigCount; // 某用户违规次数，要写入文件
set<CString> g_bannedUser; // 已封的用户
set<CString> g_defriendedUser; // 已拉黑的用户，要写入文件

// 操作
CWinThread* g_confirmThread = NULL;
CWinThread* g_operateThread = NULL;
