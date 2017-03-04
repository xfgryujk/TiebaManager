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

#include "stdafx.h"
#include <TBMAPI.h>

#include "PluginManager.h"
#include "TBMGlobal.h"
#include "TBMConfigPath.h"
#include <TBMCoreImageHelper.h>
#include "ConditionGUI.h"


TBM_API CPlugin* GetPlugin(HMODULE module)
{
	CPlugin* res = NULL;
	auto& plugins = CPluginManager::GetInstance().GetPlugins();
	for (auto& i : plugins)
	{
		if (i.m_module == module)
		{
			res = const_cast<CPlugin*>(&i);
			break;
		}
	}
	return res;
}


TBM_API ILog& GetLog()
{
	return *g_pLog;
}

TBM_API CUserCache& GetUserCache()
{
	return g_userCache;
}

TBM_API CTiebaOperate& GetTiebaOperate()
{
	return g_tiebaOperate;
}

TBM_API CTBMScan& GetScan()
{
	return CTBMScan::GetInstance();
}

TBM_API CTBMOperate& GetOperate()
{
	return CTBMOperate::GetInstance();
}


TBM_API CString GetCurrentUserDir()
{
	return CURRENT_USER_DIR_PATH;
}

TBM_API CString GetImgCacheDir()
{
	return CImageCache::CACHE_PATH;
}


TBM_API BOOL AddConditionGUI(const CString& conditionName, std::function<CConditionParam*(CConditionParam*)> onSetCondition)
{
	return CConditionGUIManager::GetInstance().AddConditionGUI(conditionName, onSetCondition);
}
