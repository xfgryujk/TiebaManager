#include "stdafx.h"
#include <TBMAPI.h>
#include "TiebaManager.h"


CTBMAPI::CTBMAPI(CEventBus* eventBus, CTiebaOperate* tiebaOperate, CTBMScan* scan, CTBMOperate* operate, ILog* log) :
	m_eventBus(eventBus),
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
