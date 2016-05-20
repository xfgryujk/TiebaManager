#include "stdafx.h"
#include <TBMAPI.h>
#include "TiebaManager.h"
#include "TBMConfigPath.h"


CTBMAPI::CTBMAPI(CEventBus* eventBus, CUserCache* userCache, CTiebaOperate* tiebaOperate, CTBMScan* scan, CTBMOperate* operate, ILog* log) :
	m_eventBus(eventBus),
	m_userCache(userCache),
	m_tiebaOperate(tiebaOperate),
	m_scan(scan),
	m_operate(operate),
	m_log(log)
{

}

CTBMAPI& CTBMAPI::GetInstance()
{
	return *theApp.m_tbmApi;
}

CString CTBMAPI::GetCurrentUserDir()
{
	return CURRENT_USER_DIR_PATH;
}

CString CTBMAPI::GetImgCacheDir()
{
	return IMG_CACHE_PATH;
}
