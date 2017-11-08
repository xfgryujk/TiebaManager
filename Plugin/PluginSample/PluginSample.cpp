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

// PluginSample.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "PluginSample.h"

#include <TBMAPI.h>
#include <TBMEvents.h>

#include <TiebaOperate.h>

using namespace std::placeholders;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CPluginSample::CPluginSample(HMODULE module) :
	m_module(module)
{
	g_mainDialogPostInitEvent.AddListener(std::bind(&CPluginSample::Init, this), m_module);
}

CPluginSample::~CPluginSample()
{
	Uninit();
}


// 初始化插件，注册Listener
void CPluginSample::Init()
{
	auto plugin = GetPlugin(m_module);
	if (plugin == NULL)
		return;

	// TODO：修改插件描述
	plugin->m_description = _T("一个插件例程\r\n")
		                    _T("\r\n")
		                    _T("作者：盗我原号的没J8");
	plugin->m_onConfig = std::bind(&CPluginSample::OnConfig, this);


	// 注册监听
	// TODO：添加监听器
	g_postSetTiebaEvent.AddListener(std::bind(&CPluginSample::OnPostSetTieba, this, _1), m_module);
}

// 卸载插件
void CPluginSample::Uninit()
{
	// TODO：卸载插件时释放资源
}

// 插件设置
void CPluginSample::OnConfig()
{
	if (!GetTiebaOperate().HasSetTieba())
	{
		AfxMessageBox(_T("请先确认贴吧！"), MB_ICONERROR);
		return;
	}

	// TODO：在这里打开插件设置窗口
}


// 确认贴吧成功后触发这个事件
void CPluginSample::OnPostSetTieba(const CString& forumName)
{
	// TODO：处理事件
	GetLog().Log(_T("<font color=green>Hello world! 设置了贴吧：</font>") + forumName);
}
