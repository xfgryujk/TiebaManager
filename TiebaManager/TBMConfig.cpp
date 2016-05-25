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
#include "TBMConfig.h"
using tinyxml2::XMLElement;
#include "TBMConfigPath.h"
#include "Update.h"
#include "ConfigHelper.h"


// 全局配置
CGlobalConfig::CGlobalConfig() : CConfigBase("Global"),
	m_firstRun("FirstRun", TRUE),
	m_firstRunAfterUpdate("FirstRunAfter" + CStringA(UPDATE_CURRENT_VERSION_A), TRUE),
	m_currentUser("UserName", _T("[NULL]"), [](const CString& value)->BOOL{ return value != _T("") && PathFileExists(USERS_DIR_PATH + value + _T("\\ck.xml")); }),
	m_autoUpdate("AutoUpdate", TRUE)
{
	m_options.push_back(&m_firstRun);
	m_options.push_back(&m_firstRunAfterUpdate);
	m_options.push_back(&m_currentUser);
	m_options.push_back(&m_autoUpdate);
}

// 用户配置
CUserConfig::CUserConfig() : CConfigBase("User"),
	m_plan("Plan", _T("默认")),
	m_forumName("ForumName")
{
	m_options.push_back(&m_plan);
	m_options.push_back(&m_forumName);
}

// Cookie文件
CCookieConfig::CCookieConfig() : CConfigBase("Cookie"),
	m_cookie("Cookie")
{
	m_options.push_back(&m_cookie);
}

// 方案
DECLEAR_READ(CPlan::Keyword)
{
	const XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return;
	}

	COption<BOOL> isRegex("IsRegex");
	COption<CString> text("Text");
	COption<BOOL> forceToConfirm("ForceToConfirm");
	COption<int> trigCount("TrigCount");
	isRegex.Read(*optionNode);
	text.Read(*optionNode);
	forceToConfirm.Read(*optionNode);
	trigCount.Read(*optionNode);

	m_value.Set(isRegex, text);
	m_value.forceToConfirm = forceToConfirm;
	m_value.trigCount = trigCount;

	if (!IsValid(m_value))
		UseDefault();
}

DECLEAR_WRITE(CPlan::Keyword)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<BOOL> isRegex("IsRegex");
	*isRegex = m_value.isRegex;
	isRegex.Write(*optionNode);
	COption<CString> text("Text");
	*text = m_value.text;
	text.Write(*optionNode);
	COption<BOOL> forceToConfirm("ForceToConfirm");
	*forceToConfirm = m_value.forceToConfirm;
	forceToConfirm.Write(*optionNode);
	COption<int> trigCount("TrigCount");
	*trigCount = m_value.trigCount;
	trigCount.Write(*optionNode);
}

DEFINE_READ_VECTOR(CPlan::Keyword)

DEFINE_WRITE_VECTOR(CPlan::Keyword)

CPlan::CPlan() : CTBMCoreConfig("Plan"),
	m_autoSaveLog		("AutoSaveLog",			FALSE),
	m_illegalLevel		("IllegalLevel",		0,		[](const int& value)->BOOL{ return 0 <= value && value <= 6; }),
	m_keywords			("IllegalContent", [](const vector<Keyword>& value)->BOOL
											{
												for (const RegexText& i : value)
													if (StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST1, i) 
														&& StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST2, i))
														return FALSE;
												return TRUE;
											}),
	m_imageDir			("IllegalImageDir",		_T("")),
	m_SSIMThreshold		("SSIMThreshold",		2.43f,	[](const double& value)->BOOL{ return 1.0f <= value && value <= 3.0f; }),
	m_blackList			("BlackList", [](const vector<RegexText>& value)->BOOL
										{
											for (const RegexText& i : value)
												if (StringIncludes(MATCH_TOO_MUCH_USERNAME_TEST1, i) 
													&& StringIncludes(MATCH_TOO_MUCH_USERNAME_TEST2, i))
													return FALSE;
											return TRUE;
										}),
	m_whiteList			("WhiteList"),
	m_whiteContent		("TrustedContent"),
	m_trustedThread		("TrustedThread")
{
	m_updateImage = TRUE;
	m_options.push_back(&m_autoSaveLog);
	m_options.push_back(&m_illegalLevel);
	m_options.push_back(&m_keywords);
	m_options.push_back(&m_imageDir);
	m_options.push_back(&m_SSIMThreshold);
	m_options.push_back(&m_blackList);
	m_options.push_back(&m_whiteList);
	m_options.push_back(&m_whiteContent);
	m_options.push_back(&m_trustedThread);
}

void CPlan::OnChange()
{
	m_optionsLock.Lock();
}

void CPlan::PostChange()
{
	if (m_updateImage)
	{
		m_updateImage = FALSE;
		CConfigHelper::ReadImages(m_imageDir, m_images); 
	}
	m_optionsLock.Unlock();
}
