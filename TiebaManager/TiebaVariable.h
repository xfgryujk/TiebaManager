#pragma once


// 贴吧、用户信息
extern CString g_forumName;
extern CString g_encodedForumName;
extern CString g_forumID;
extern CString g_cookie;
extern CString g_tbs; // 备用获取tbs地址：http://tieba.baidu.com/dc/common/tbs

// 扫描
extern volatile BOOL g_stopScanFlag;
extern CWinThread* g_scanThread;
extern set<__int64> g_initIgnoredTID; // 不删的主题ID(手动忽略)，要写入文件
extern set<__int64> g_initIgnoredPID; // 不删的帖子ID(手动忽略)，要写入文件
extern set<__int64> g_initIgnoredLZLID; // 不删的楼中楼ID(手动忽略)，要写入文件
extern set<__int64> g_ignoredTID; // 不删的主题ID(已扫描且违规)
extern set<__int64> g_ignoredPID; // 不删的帖子ID(已扫描且违规)
extern set<__int64> g_ignoredLZLID; // 不删的楼中楼ID(已扫描且违规)
extern set<__int64> g_deletedTID; // 已删的主题ID
extern map<__int64, int> g_reply; // 主题的回复数，要写入文件
extern map<CString, int> g_userTrigCount; // 某用户违规次数，要写入文件
extern set<CString> g_bannedUser; // 已封的用户
extern set<CString> g_defriendedUser; // 已拉黑的用户，要写入文件

// 操作
extern CWinThread* g_confirmThread;
extern CWinThread* g_operateThread;
