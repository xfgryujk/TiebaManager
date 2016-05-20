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


// TiebaManager.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CTiebaManagerApp: 
// 有关此类的实现，请参阅 TiebaManager.cpp
//
class CGlobalConfig;
class CUserConfig;
class CCookieConfig;
class CPlan;
class CUserCache;

class CTiebaOperate;
class CTBMScan;
class CTBMOperate;

class CTBMScanListeners;
class CTBMOperateListeners;

class CEventBus;
class CTBMAPI;
class CPluginManager;

class CTiebaManagerApp : public CWinApp
{
public:
	CTiebaManagerApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	unique_ptr<CGlobalConfig> m_globalConfig;
	unique_ptr<CUserConfig> m_userConfig;
	unique_ptr<CCookieConfig> m_cookieConfig;
	unique_ptr<CPlan> m_plan;
	unique_ptr<CUserCache> m_userCache;

	unique_ptr<CTiebaOperate> m_tiebaOperate;
	unique_ptr<CTBMScan> m_scan;
	unique_ptr<CTBMOperate> m_operate;

	unique_ptr<CTBMScanListeners> m_scanListeners;
	unique_ptr<CTBMOperateListeners> m_operateListeners;

	unique_ptr<CEventBus> m_tbmEventBus;
	unique_ptr<CTBMAPI> m_tbmApi;
	unique_ptr<CPluginManager> m_pluginManager;

	void init();

	DECLARE_MESSAGE_MAP()
};

extern CTiebaManagerApp theApp;