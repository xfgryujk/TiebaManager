#include "stdafx.h"
#include "TiebaVariable.h"
#include "TiebaOperate.h"
#include "StringHelper.h"


// 贴吧、用户信息
CUserTiebaInfo g_userTiebaInfo;

// 扫描
volatile BOOL g_stopScanFlag = FALSE;
CWinThread* g_scanThread = NULL;
CUserCache g_userCache;

// 操作
CWinThread* g_confirmThread = NULL;
CWinThread* g_operateThread = NULL;


void CUserTiebaInfo::PostChange()
{
	m_bduss = GetStringBetween(m_cookie, _T("BDUSS="), _T(";"));
}
