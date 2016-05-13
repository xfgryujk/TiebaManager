#include "stdafx.h"
#include "TiebaVariable.h"
#include "TBMOperate.h"
#include <StringHelper.h>


// Cookie文件
CCookieConfig g_cookieConfig;

// 扫描
volatile BOOL g_stopScanFlag = FALSE;
CWinThread* g_scanThread = NULL;
CUserCache g_userCache;

// 操作
CWinThread* g_confirmThread = NULL;
CWinThread* g_operateThread = NULL;
