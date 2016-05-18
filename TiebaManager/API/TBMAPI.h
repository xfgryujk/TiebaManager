#pragma once
#include "TiebaManagerCommon.h"
class CEventBus;
class ILog;
class CTiebaOperate;
class CTBMScan;
class CTBMOperate;


class TIEBA_MANAGER_API CTBMAPI
{
	friend class CTiebaManagerApp;
	friend class CTiebaManagerDlg;

protected:
	// 可以注册事件监听，见TBMEvent.h
	CEventBus* m_eventBus = NULL;
	ILog* m_log = NULL;
	CTiebaOperate* m_tiebaOperate = NULL;
	CTBMScan* m_scan = NULL;
	CTBMOperate* m_operate = NULL;

	CTBMAPI(CEventBus* eventBus = NULL, CTiebaOperate* tiebaOperate = NULL, 
		CTBMScan* scan = NULL, CTBMOperate* operate = NULL, ILog* log = NULL);

public:
	CTBMAPI& GetInstance();

	CEventBus* GetEventBus() { return m_eventBus; }
	ILog* GetLog() { return m_log; }
	CTiebaOperate* GetTiebaOperate() { return m_tiebaOperate; }
	CTBMScan* GetScan() { return m_scan; }
	CTBMOperate* GetOperate() { return m_operate; }
};
