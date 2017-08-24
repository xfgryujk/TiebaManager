/*
Copyright (C) 2011-2017  xfgryujk
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
#include <ConfigFile.h>
#include "LoopBanDlg.h"
#include <memory>
#include <thread>
class Operation;


class CLoopBanConfig : public CConfigBase
{
public:
	COption<BOOL> m_enable;					    // 开启
	COption<BOOL> m_log;					    // 输出日志
	COption<float> m_banInterval;			    // 封禁间隔
	COption<std::vector<CString> > m_userList;	// 用户列表
	COption<std::vector<CString> > m_pidList;	// PID列表
	COption<BOOL> m_autoLoopBan;			    // 自动循环封

	CLoopBanConfig() : CConfigBase("LoopBan"),
		m_enable("Enable", TRUE),
		m_log("Log"),
		m_banInterval("BanInterval", 2.0f, [](const float& value){ return 0.0f <= value && value <= 60.0f; }/*InRange<float, 0.0f, 60.0f>*/),
		m_userList("Name"),
		m_pidList("PID"),
		m_autoLoopBan("AutoLoopBan", FALSE)
	{
		m_options.push_back(&m_enable);
		m_options.push_back(&m_log);
		m_options.push_back(&m_banInterval);
		m_options.push_back(&m_userList);
		m_options.push_back(&m_pidList);
		m_options.push_back(&m_autoLoopBan);
	}

	BOOL Load(const CString& path)
	{
		BOOL res = CConfigBase::Load(path);
		if (m_pidList->size() != m_userList->size())
			m_pidList->resize(m_userList->size());
		return res;
	}
};


class CLoopBan
{
public:
	CLoopBan(HMODULE module);
	~CLoopBan();

	void Init();
	void Uninit();
	void OnConfig();

	void OnPostSetTieba(const CString& forumName);
	void OnPostBan(const Operation& op, BOOL succeeded);

	void LoopBanThread();


	HMODULE m_module;

	CLoopBanConfig m_config;
	CLoopBanDlg* m_loopBanDlg = NULL;
};
