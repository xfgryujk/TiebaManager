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
#include "ConfigHelper.h"

#include <ImageHelper.h>
#include <MiscHelper.h>

#include "TBMConfigPath.h"
#include <TBMEvent.h>


CConfigHelper::CConfigHelper(CGlobalConfig* globalConfig, CUserConfig* userConfig, CCookieConfig* cookieConfig,
	CUserCache* userCache, CPlan* plan, CEventBus* tbmEventBus) :
	m_globalConfig(globalConfig),
	m_userConfig(userConfig),
	m_cookieConfig(cookieConfig),
	m_userCache(userCache),
	m_plan(plan),
	m_tbmEventBus(tbmEventBus)
{

}


// 保存当前账号配置
void CConfigHelper::SaveCurrentUserConfig()
{
	// 创建目录
	CreateDir(USERS_DIR_PATH + m_globalConfig->m_currentUser);

	// 当前账号配置
	m_userConfig->Save(USER_CONFIG_PATH);

	// Cookie
	m_cookieConfig->Save(COOKIE_PATH);

	// 历史回复、忽略ID等
	m_userCache->Save(CACHE_PATH);
}

// 设置当前账号
void CConfigHelper::SetCurrentUser(const CString& userName, BOOL save)
{
	if (!m_tbmEventBus->Post(PreSetCurrentUserEvent, CSetCurrentUserEvent(userName)))
		return;

	// 保存当前账号配置
	if (save)
		SaveCurrentUserConfig();

	// 设置配置路径
	*m_globalConfig->m_currentUser = userName;
	CURRENT_USER_DIR_PATH = USERS_DIR_PATH + userName + _T("\\");
	USER_CONFIG_PATH = CURRENT_USER_DIR_PATH + _T("options.xml");
	COOKIE_PATH = CURRENT_USER_DIR_PATH + _T("ck.xml");
	CACHE_PATH = CURRENT_USER_DIR_PATH + _T("cache.xml");

	// 读取设置
	m_userConfig->Load(USER_CONFIG_PATH);
	// 方案
	m_plan->Load(OPTIONS_DIR_PATH + m_userConfig->m_plan + _T(".xml"));
	ReadImages(m_plan->m_imageDir, m_plan->m_images);

	// Cookie
	m_cookieConfig->Load(COOKIE_PATH);

	// 历史回复、忽略ID等
	m_userCache->Load(CACHE_PATH);

	m_tbmEventBus->Post(PostSetCurrentUserEvent, CSetCurrentUserEvent(userName));
}


// 从目录读取图片到images
void CConfigHelper::ReadImages(const CString& dir, vector<CPlan::NameImage>& images)
{
	vector<CString> imagePath;

	if (dir == _T(""))
	{
		images.clear();
		return;
	}
	CFileFind fileFind;
	static const TCHAR* IMG_EXT[] = { _T("\\*.jpg"), _T("\\*.png"), _T("\\*.gif"), _T("\\*.jpeg"), _T("\\*.bmp") };
	for (int i = 0; i < _countof(IMG_EXT); i++)
	{
		BOOL flag = fileFind.FindFile(dir + IMG_EXT[i]);
		while (flag)
		{
			flag = fileFind.FindNextFile();
			imagePath.push_back(fileFind.GetFilePath());
		}
	}

	images.resize(imagePath.size());
	UINT imgCount = 0;
	for (CString& i : imagePath)
	{
		images[imgCount].name = GetImageName(i);
		if (ReadImage(i, images[imgCount].img))
			imgCount++;
	}
	if (imagePath.size() != imgCount)
	{
		images.resize(imgCount);
		CString msg;
		msg.Format(_T("%u张图片加载失败！"), imagePath.size() - imgCount);
		AfxMessageBox(msg, MB_ICONINFORMATION);
	}
}
