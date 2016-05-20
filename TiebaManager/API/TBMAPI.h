/*
Copyright (C) 2015  xfgryujk
http://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

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
