#pragma once
#include "TiebaManagerCommon.h"
class CEventBus;
class ILog;
class CUserCache;
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
	CUserCache* m_userCache = NULL;
	CTiebaOperate* m_tiebaOperate = NULL;
	CTBMScan* m_scan = NULL;
	CTBMOperate* m_operate = NULL;

	CTBMAPI(CEventBus* eventBus = NULL, CUserCache* userCache = NULL, CTiebaOperate* tiebaOperate = NULL,
		CTBMScan* scan = NULL, CTBMOperate* operate = NULL, ILog* log = NULL);

public:
	static CTBMAPI& GetInstance();

	static CString GetCurrentUserDir();
	static CString GetImgCacheDir();

	CEventBus* GetEventBus() { return m_eventBus; }
	ILog* GetLog() { return m_log; }
	CUserCache* GetUserCache() { return m_userCache; }
	CTiebaOperate* GetTiebaOperate() { return m_tiebaOperate; }
	CTBMScan* GetScan() { return m_scan; }
	CTBMOperate* GetOperate() { return m_operate; }
};
