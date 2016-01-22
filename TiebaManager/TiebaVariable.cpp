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
CUserCache g_userCache;

// 操作
CWinThread* g_confirmThread = NULL;
CWinThread* g_operateThread = NULL;
