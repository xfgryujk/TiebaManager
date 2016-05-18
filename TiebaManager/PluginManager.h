#pragma once
#include "Plugin.h"


class CPluginManager
{
public:
	virtual ~CPluginManager();

	BOOL Load(const CString& path);
	BOOL LoadDir(const CString& dir);
	BOOL Unload(int index);
	BOOL UnloadAll();

	const vector<unique_ptr<CPlugin> >& GetPlugins();

protected:
	vector<unique_ptr<CPlugin> > m_plugins;
};
