#include "stdafx.h"
#include "Plugin.h"


CPlugin::CPlugin(HMODULE handle) :
	m_handle(handle)
{

}

BOOL CPlugin::Load()
{
	if (m_handle == NULL)
		return FALSE;

	m_init = (InitType)GetProcAddress(m_handle, "Init");
	m_uninit = (UninitType)GetProcAddress(m_handle, "Uninit");
	m_getDescription = (GetDescriptionType)GetProcAddress(m_handle, "GetDescription");
	m_onConfig = (OnConfigType)GetProcAddress(m_handle, "OnConfig");
	if (m_init == NULL || m_uninit == NULL)
	{
		m_getDescription = NULL;
		m_onConfig = NULL;
		return FALSE;
	}

	return m_init();
}

BOOL CPlugin::Unload()
{
	return m_uninit == NULL ? FALSE : m_uninit();
}


CString CPlugin::GetDescription()
{
	return m_getDescription == NULL ? _T("") : m_getDescription();
}

void CPlugin::OnConfig()
{
	if (m_onConfig == NULL)
		return;
	m_onConfig();
}
