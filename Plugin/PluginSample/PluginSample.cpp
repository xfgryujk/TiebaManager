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

// PluginSample.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "PluginSample.h"

#include <TBMAPI.h>
#include <TBMEvent.h>
#include <TBMCoreConfig.h>

#include <TiebaOperate.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CPluginSample g_pluginSample;


// 见TiebaManager\API\PluginFunctions.h

extern "C" __declspec(dllexport) bool __cdecl Init()
{
	return g_pluginSample.Init();
}

extern "C" __declspec(dllexport) bool __cdecl Uninit()
{
	return g_pluginSample.Uninit();
}

extern "C" __declspec(dllexport) LPCWSTR __cdecl GetDescription()
{
	return _T("一个插件例程\r\n")
		_T("\r\n")
		_T("作者：盗我原号的没J8");
}

extern "C" __declspec(dllexport) void __cdecl OnConfig()
{
	g_pluginSample.OnConfig();
}


// 初始化插件，注册Listener
bool CPluginSample::Init()
{
	// 注册监听
	CEventBus* eventBus = CTBMAPI::GetEventBus();
	// 因为listener是std::function，这里可以用函数指针、std::bind、lambda表达式等
	m_onPostSetTiebaID = CTBMAPI::GetEventBus()->AddListener(PostSetTiebaEvent,
		std::bind(&CPluginSample::OnPostSetTieba, this, std::placeholders::_1));

	return true;
}

// 卸载插件，必须卸载所有Listener，否则会崩溃
bool CPluginSample::Uninit()
{
	// 删除监听
	CEventBus* eventBus = CTBMAPI::GetEventBus();
	eventBus->DeleteListener(PostSetTiebaEvent, m_onPostSetTiebaID);

	return true;
}

// 插件设置
void CPluginSample::OnConfig()
{
	if (!CTBMAPI::GetTiebaOperate()->HasSetTieba())
	{
		AfxMessageBox(_T("请先确认贴吧！"), MB_ICONERROR);
		return;
	}

	// 在这里打开插件设置窗口
}


// 确认贴吧成功后触发这个事件
void CPluginSample::OnPostSetTieba(CEventBase* event__)
{
	CSetTiebaEvent* event_ = (CSetTiebaEvent*)event__;
	// 这里可以处理事件，也可以取消事件，当然有些事件不能取消，比如这个PostSetTiebaEvent
	CTBMAPI::GetLog()->Log(_T("<font color=green>Hello world! 设置了贴吧：</font>") + event_->m_forumName);
}
