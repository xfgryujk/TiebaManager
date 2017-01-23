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

// LoopBan.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "LoopBan.h"

#include <TBMAPI.h>
#include <TBMEvents.h>
#include <TBMCoreEvents.h>

#include <StringHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>

#include <TiebaClawer.h>
#include <TiebaOperate.h>
#include <TBMCoreConfig.h>
#include <TBMOperate.h>

using namespace std::placeholders;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CLoopBan::CLoopBan(HMODULE module) :
	m_module(module)
{
	g_mainDialogPostInitEvent.AddListener(std::bind(&CLoopBan::Init, this), m_module);
}

CLoopBan::~CLoopBan()
{
	Uninit();
}


void CLoopBan::Init()
{
	auto plugin = GetPlugin(m_module);
	if (plugin == NULL)
		return;

	plugin->m_description = _T("循环封插件\r\n")
		                    _T("\r\n")
		                    _T("作者：盗我原号的没J8");
	plugin->m_onConfig = std::bind(&CLoopBan::OnConfig, this);


	// 注册监听
	g_postSetTiebaEvent.AddListener(std::bind(&CLoopBan::OnPostSetTieba, this, _1), m_module);
	g_postBanEvent.AddListener(std::bind(&CLoopBan::OnPostBan, this, _1, _2), m_module);
	
	// 每24小时开始循环封
	std::thread([this]{
		while (true)
		{
			Sleep(24 * 60 * 60 * 1000);
			if (GetTiebaOperate().HasSetTieba())
				std::thread(&CLoopBan::LoopBanThread, this).detach();
		}
	}).detach();
}

void CLoopBan::Uninit()
{
	// 关闭窗口
	if (m_loopBanDlg != NULL)
		m_loopBanDlg->DestroyWindow();

	// 保存配置
	if (GetTiebaOperate().HasSetTieba())
	{
		CString currentUserDir = GetCurrentUserDir();
		m_config.Save(currentUserDir + _T("options2.xml"));
	}
}

void CLoopBan::OnConfig()
{
	if (!GetTiebaOperate().HasSetTieba())
	{
		AfxMessageBox(_T("请先确认贴吧！"), MB_ICONERROR);
		return;
	}

	if (m_loopBanDlg == NULL)
	{
		m_loopBanDlg = new CLoopBanDlg(m_loopBanDlg, m_config);
		m_loopBanDlg->Create(m_loopBanDlg->IDD, CWnd::GetDesktopWindow());
	}
}


void CLoopBan::OnPostSetTieba(const CString& forumName)
{
	// 载入配置
	CString currentUserDir = GetCurrentUserDir();
	m_config.Load(currentUserDir + _T("options2.xml"));

	// 开始循环封
	std::thread(&CLoopBan::LoopBanThread, this).detach();
}

void CLoopBan::OnPostBan(const Operation& op, BOOL succeeded)
{
	// 自动循环封
	if (m_config.m_autoLoopBan)
	{
		auto it = std::find(m_config.m_userList->cbegin(), m_config.m_userList->cend(), op.object->author);
		if (it == m_config.m_userList->cend())
		{
			m_config.m_userList->push_back(op.object->author);
			m_config.m_pidList->push_back(_T(""));
			CString currentUserDir = GetCurrentUserDir();
			DeleteFile(currentUserDir + _T("LoopBanDate.xml"));
		}
	}
}


// 取用户发的帖子ID
static CString GetPIDFromUser(const CString& userName)
{
	CString src = HTTPGet(_T("http://tieba.baidu.com/f/search/ures?ie=utf-8&kw=") + GetTiebaOperate().GetEncodedForumName() 
		+ _T("&qw=&rn=10&un=") + userName + _T("&only_thread=&sm=1&sd=&ed=&pn=1"));
	if (src == NET_TIMEOUT_TEXT)
		return NET_TIMEOUT_TEXT;
	CString pid = GetStringBetween(src, _T("<div class=\"s_post\">"), _T("target=\"_blank\" >"));
	pid = GetStringBetween(pid, _T("?pid="), _T("&"));
	return pid;
}

void CLoopBan::LoopBanThread()
{
	ILog& log = GetLog();
	CString currentUserDir = GetCurrentUserDir();
	CTiebaOperate& tiebaOperate = GetTiebaOperate();
	CTBMOperate& operate = GetOperate();


	class CLoopBanDate : public CConfigBase
	{
	public:
		COption<int> m_year;
		COption<int> m_month;
		COption<int> m_day;

		CLoopBanDate() : CConfigBase("LoopBanDate"),
			m_year("Year"),
			m_month("Month"),
			m_day("Day")
		{
			m_options.push_back(&m_year);
			m_options.push_back(&m_month);
			m_options.push_back(&m_day);
		}
	};

	// 一天内循环封过不再封
	SYSTEMTIME time = {};
	GetLocalTime(&time);
	CLoopBanDate lastTime;
	lastTime.Load(currentUserDir + _T("LoopBanDate.xml"));
	if (time.wDay == lastTime.m_day && time.wMonth == lastTime.m_month && time.wYear == lastTime.m_year)
		return;

	if (!m_config.m_enable)
		return;
	CLoopBanConfig config = m_config; // 复制一份防止在主线程被修改

	BOOL updatePID = FALSE;
	// 循环封
	if (!CoInitializeHelper())
		return;
	for (UINT i = 0; i < config.m_userList->size(); i++)
	{
		CString code;
		if (operate.m_config->m_banClientInterface)
			code = tiebaOperate.BanIDClient((*config.m_userList)[i]); // 用客户端接口封禁
		else
		{
			if ((*config.m_pidList)[i] != _T("")) // 尝试用PID封禁
				code = tiebaOperate.BanID((*config.m_userList)[i], (*config.m_pidList)[i]);
			if ((*config.m_pidList)[i] == _T("") || code != _T("0")) // 尝试不用PID封禁（用户必须为本吧会员）
			{
				code = tiebaOperate.BanID((*config.m_userList)[i]);
				if (code != _T("0")) // 尝试获取新的PID并用PID封禁
				{
					(*config.m_pidList)[i] = GetPIDFromUser((*config.m_userList)[i]);
					updatePID = TRUE;
					code = tiebaOperate.BanID((*config.m_userList)[i], (*config.m_pidList)[i]);
				}
			}
		}

		if (config.m_log)
		{
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<font color=red>封禁 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
					_T("\"bd:%s,\">重试</a>"), (LPCTSTR)(*config.m_userList)[i], (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), 
					(LPCTSTR)(*config.m_userList)[i]);
				log.Log(content);
			}
			else
				log.Log(_T("<font color=red>封禁 </font>") + (*config.m_userList)[i]);
		}

		if (code == _T("0") && i < config.m_userList->size() - 1)
			Sleep((DWORD)(config.m_banInterval * 1000));
	}
	CoUninitialize();

	// 更新PID
	if (updatePID)
		config.Save(currentUserDir + _T("options2.xml")); // 可能会覆盖刚刚在LoopBanDlg修改的设置

	// 更新时间
	*lastTime.m_year = time.wYear;
	*lastTime.m_month = time.wMonth;
	*lastTime.m_day = time.wDay;
	lastTime.Save(currentUserDir + _T("LoopBanDate.xml"));
}
