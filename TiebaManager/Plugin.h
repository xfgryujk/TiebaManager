#pragma once
#include "PluginFunctions.h"


class CPlugin
{
	friend class CPluginManager;

protected:
	HMODULE m_handle = NULL;

	InitType m_init = NULL;
	UninitType m_uninit = NULL;
	GetDescriptionType m_getDescription = NULL;
	OnConfigType m_onConfig = NULL;


	CPlugin(HMODULE handle = NULL);

	BOOL Load();
	BOOL Unload();

public:
	// 取插件描述
	CString GetDescription();
	// 打开插件设置
	void OnConfig();
};
