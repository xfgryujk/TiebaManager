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

#include "stdafx.h"
#include "PluginManager.h"


CPluginManager::~CPluginManager()
{
	UnloadAll();
}

BOOL CPluginManager::Load(const CString& path)
{
	HMODULE handle = LoadLibrary(path);
	if (handle == NULL)
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

	unique_ptr<CPlugin> plugin(new CPlugin(handle, name));
	if (!plugin->Load())
	{
		FreeLibrary(handle);
		return FALSE;
	}

	m_plugins.push_back(std::move(plugin));
	return TRUE;
}

BOOL CPluginManager::LoadDir(const CString& dir)
{
	BOOL res = TRUE;
	CFileFind fileFind;
	BOOL flag = fileFind.FindFile(dir + _T("//*.dll"));
	while (flag)
	{
		flag = fileFind.FindNextFile();
		if (!Load(fileFind.GetFilePath()))
			res = FALSE;
	}
	return res;
}

BOOL CPluginManager::Unload(int index)
{
	if (!m_plugins[index]->Unload())
		return FALSE;
	if (!FreeLibrary(m_plugins[index]->m_handle))
		return FALSE;
	m_plugins.erase(m_plugins.begin() + index);
	return TRUE;
}

BOOL CPluginManager::UnloadAll()
{
	BOOL res = TRUE;
	for (int i = m_plugins.size() - 1; i >= 0; i--)
	if (!Unload(i))
		res = FALSE;
	return res;
}

const vector<unique_ptr<CPlugin> >& CPluginManager::GetPlugins()
{
	return m_plugins;
}
