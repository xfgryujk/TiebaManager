#include "stdafx.h"
#include "ConfigHelper.h"
#include <MiscHelper.h>
#include <TBMConfig.h>
#include "TiebaVariable.h"
#include "ScanImage.h"
#include "TiebaManagerDlg.h"


// 保存当前账号配置
void SaveCurrentUserConfig()
{
	// 创建目录
	CreateDir(USERS_PATH + g_globalConfig.m_currentUser);

	// 当前账号配置
	g_userConfig.Save(USER_PROFILE_PATH);

	// Cookie
	g_cookieConfig.Save(COOKIE_PATH);

	// 历史回复、忽略ID等
	g_userCache.Save(CACHE_PATH);
}

// 设置当前账号
void SetCurrentUser(const CString& userName, BOOL save)
{
	// 保存当前账号配置
	if (save)
		SaveCurrentUserConfig();

	// 设置配置路径
	*g_globalConfig.m_currentUser = userName;
	CURRENT_USER_PATH = USERS_PATH + userName;
	USER_PROFILE_PATH = CURRENT_USER_PATH + _T("\\options.xml");
	COOKIE_PATH = CURRENT_USER_PATH + _T("\\ck.xml");
	CACHE_PATH = CURRENT_USER_PATH + _T("\\cache.xml");

	// 读取设置
	g_userConfig.Load(USER_PROFILE_PATH);
	// 方案
	g_plan.Load(OPTIONS_PATH + g_userConfig.m_plan + _T(".xml"));
	ReadImages(g_plan.m_imageDir);
	// 贴吧名
	((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_forumNameEdit.SetWindowText(*g_userConfig.m_forumName);

	// Cookie
	g_cookieConfig.Load(COOKIE_PATH);

	// 历史回复、忽略ID等
	g_userCache.Load(CACHE_PATH);
}
