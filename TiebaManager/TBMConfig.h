/*
Copyright (C) 2011-2017  xfgryujk
https://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

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
#include <TBMCoreGlobal.h>
#include <StringHelper.h>
#pragma warning(disable:4819) // OpenCV头文件包含Unicode字符
#include <opencv2\core\mat.hpp>


// 全局配置
class CGlobalConfig : public CConfigBase
{
public:
	COption<BOOL> m_firstRun;				// 第一次运行
	COption<CString> m_currentUser;			// 当前账号
	COption<BOOL> m_autoUpdate;				// 自动更新

	CGlobalConfig();
};

// 用户配置
class CUserConfig : public CConfigBase
{
public:
	COption<CString> m_plan;		// 当前方案
	COption<CString> m_forumName;	// 贴吧名

	CUserConfig();
};

// Cookie文件
class CCookieConfig : public CConfigBase
{
public:
	COption<CString> m_cookie;

	CCookieConfig();
};

// 方案
class CPlan : public CTBMCoreConfig
{
public:
	COption<BOOL>      m_autoSaveLog;       // 自动保存日志

	CPlan::CPlan();
};
