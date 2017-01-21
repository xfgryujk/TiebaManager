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
#include <memory>

#include "TBMConfig.h"
#include <TiebaOperate.h>
#include <TBMScan.h>
#include <TBMOperate.h>
#include "ConfigHelper.h"
#include "ScanImage.h"
#include "TBMListeners.h"
#include "TiebaManagerDlg.h"


// CTiebaManagerApp: 
// 有关此类的实现，请参阅 TiebaManager.cpp
//

class CTiebaManagerApp : public CWinApp
{
public:
	CTiebaManagerApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	ILog* m_log = NULL;

	std::unique_ptr<CGlobalConfig> m_globalConfig;
	std::unique_ptr<CUserConfig> m_userConfig;
	std::unique_ptr<CCookieConfig> m_cookieConfig;
	std::unique_ptr<CPlan> m_plan;
	std::unique_ptr<CUserCache> m_userCache;

	std::unique_ptr<CTiebaOperate> m_tiebaOperate;
	std::unique_ptr<CTBMScan> m_scan;
	std::unique_ptr<CTBMOperate> m_operate;

	std::unique_ptr<CConfigHelper> m_configHelper;
	std::unique_ptr<CScanImage> m_scanImage;

	std::unique_ptr<CTBMListeners> m_tbmListeners;

private:
	static LONG WINAPI ExceptionHandler(_EXCEPTION_POINTERS* ExceptionInfo);
	void Init();

	DECLARE_MESSAGE_MAP()
};

extern CTiebaManagerApp theApp;