#include "stdafx.h"
#include "TiebaVariable.h"
#include "TBMOperate.h"
#include <StringHelper.h>


// CookieÎÄ¼þ
CCookieConfig g_cookieConfig;

// É¨Ãè
volatile BOOL g_stopScanFlag = FALSE;
CWinThread* g_scanThread = NULL;
CUserCache g_userCache;

// ²Ù×÷
CWinThread* g_confirmThread = NULL;
CWinThread* g_operateThread = NULL;
