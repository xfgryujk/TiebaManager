#include "stdafx.h"
#include <TBMConfig.h>
using tinyxml2::XMLElement;
#include "Update.h"
#include <TBMConfigPath.h>
#include "ScanImage.h"


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
TIEBA_MANAGER_API DECLEAR_READ(CPlan::Keyword)
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

TIEBA_MANAGER_API DECLEAR_WRITE(CPlan::Keyword)
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

TIEBA_MANAGER_API DEFINE_READ_VECTOR(CPlan::Keyword)

TIEBA_MANAGER_API DEFINE_WRITE_VECTOR(CPlan::Keyword)

CPlan::CPlan() : CTBMCoreConfig("Plan"),
	m_autoSaveLog		("AutoSaveLog",			FALSE),
	m_illegalLevel		("IllegalLevel",		0,		[](const int& value)->BOOL{ return 0 <= value && value <= 6; }),
	m_autoLoopBan		("AutoLoopBan",			FALSE),
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
	m_options.push_back(&m_autoLoopBan);
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
		ReadImages(m_imageDir, m_images); 
	}
	m_optionsLock.Unlock();
}
