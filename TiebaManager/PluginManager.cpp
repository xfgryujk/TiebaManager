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

	unique_ptr<CPlugin> plugin(new CPlugin(handle));
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
