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

#include "stdafx.h"
#include "PluginManager.h"
#include "TBMConfigPath.h"
#include <TBMCoreEvents.h>
#include <TBMEvents.h>


CPluginManager::CPluginManager()
{
	Init();
}

CPluginManager::~CPluginManager()
{
	Uninit();
}

void CPluginManager::Init()
{
	LoadDir(PLUGIN_PATH);
}

void CPluginManager::Uninit()
{
	UnloadAll();
}


BOOL CPluginManager::LoadPlugin(const CString& path)
{
	HMODULE module = LoadLibrary(path);
	if (module == NULL)
		return FALSE;

	int left = path.ReverseFind(_T('\\'));
	if (left == -1)
		left = 0;
	else
		left++;
	int right = path.ReverseFind(_T('.'));
	if (right == -1)
		right = path.GetLength();
	CString name = path.Mid(left, right - left);

	m_plugins.resize(m_plugins.size() + 1);
	auto& plugin = m_plugins.back();
	plugin.m_path = path;
	plugin.m_module = module;
	plugin.m_name = name;

	return TRUE;
}

BOOL CPluginManager::LoadDir(const CString& dir)
{
	BOOL res = TRUE;
	CFileFind fileFind;
	BOOL found = fileFind.FindFile(dir + _T("//*.dll"));
	while (found)
	{
		found = fileFind.FindNextFile();
		if (!LoadPlugin(fileFind.GetFilePath()))
			res = FALSE;
	}
	return res;
}

BOOL CPluginManager::UnloadPlugin(int index)
{
	auto& plugin = m_plugins[index];
	if (plugin.m_module != NULL)
	{
		for (auto i : g_tbmCoreEvents)
			i->DeleteListenersOfModule(plugin.m_module);
		for (auto i : g_tbmEvents)
			i->DeleteListenersOfModule(plugin.m_module);
		plugin.m_onConfig = nullptr;

		if (!FreeLibrary(plugin.m_module))
			return FALSE;
	}
	m_plugins.erase(m_plugins.begin() + index);
	return TRUE;
}

BOOL CPluginManager::UnloadAll()
{
	BOOL res = TRUE;
	for (int i = m_plugins.size() - 1; i >= 0; i--)
		res = res && UnloadPlugin(i);
	return res;
}
