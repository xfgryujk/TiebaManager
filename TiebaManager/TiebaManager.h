
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
class CTBMScan;
class CTBMOperate;
class CTBMScanListeners;
class CTBMOperateListeners;

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
	unique_ptr<CTBMScan> m_scan;
	unique_ptr<CTBMOperate> m_operate;
	unique_ptr<CTBMScanListeners> m_scanListeners;
	unique_ptr<CTBMOperateListeners> m_operateListeners;

	void init();

	DECLARE_MESSAGE_MAP()
};

extern CTiebaManagerApp theApp;