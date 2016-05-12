#include "stdafx.h"
#include <TBMConfig.h>
using tinyxml2::XMLElement;
#include "Update.h"
#include "ListTestPage.h"
#include "ScanImage.h"


// 配置文件路径
TIEBA_MANAGER_API CString GLOBAL_CONFIG_PATH = _T("\\options.xml");	// 程序运行时初始化
TIEBA_MANAGER_API CString USER_PROFILE_PATH;						// 确定贴吧时初始化
TIEBA_MANAGER_API CString OPTIONS_PATH = _T("Option\\");
TIEBA_MANAGER_API CString USERS_PATH = _T("\\User\\");				// 程序运行时初始化
TIEBA_MANAGER_API CString CURRENT_USER_PATH;						// 确定贴吧时初始化
TIEBA_MANAGER_API CString COOKIE_PATH;								// 确定贴吧时初始化
TIEBA_MANAGER_API CString CACHE_PATH;								// 确定贴吧时初始化


// 全局配置
TIEBA_MANAGER_API CGlobalConfig g_globalConfig;
CGlobalConfig::CGlobalConfig() : CConfigBase("Global"),
	m_firstRun("FirstRun", TRUE),
	m_firstRunAfterUpdate("FirstRunAfter" + CStringA(UPDATE_CURRENT_VERSION_A), TRUE),
	m_currentUser("UserName", _T("[NULL]"), [](const CString& value)->BOOL{ return value != _T("") && PathFileExists(USERS_PATH + value + _T("\\ck.xml")); }),
	m_autoUpdate("AutoUpdate", TRUE)
{
	m_options.push_back(&m_firstRun);
	m_options.push_back(&m_firstRunAfterUpdate);
	m_options.push_back(&m_currentUser);
	m_options.push_back(&m_autoUpdate);
}

// 用户配置
TIEBA_MANAGER_API CUserConfig g_userConfig;
CUserConfig::CUserConfig() : CConfigBase("User"),
	m_plan("Plan", _T("默认")),
	m_forumName("ForumName")
{
	m_options.push_back(&m_plan);
	m_options.push_back(&m_forumName);
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

TIEBA_MANAGER_API CPlan g_plan;
CPlan::CPlan() : CConfigBase("Plan"),
	m_scanInterval		("ScanInterval",		5,		[](const int& value)->BOOL{ return 0 <= value && value <= 600; }),
	m_onlyScanTitle		("OnlyScanTitle",		FALSE),
	m_scanPageCount		("ScanPageCount",		1,		[](const int& value)->BOOL{ return 1 <= value; }),
	m_briefLog			("BriefLog",			FALSE),
	m_threadCount		("ThreadCount",			2,		[](const int& value)->BOOL{ return 1 <= value && value <= 16; }),
	m_autoSaveLog		("AutoSaveLog",			FALSE),
	m_illegalLevel		("IllegalLevel",		0,		[](const int& value)->BOOL{ return 0 <= value && value <= 6; }),
	m_delete			("Delete",				TRUE),
	m_banID				("BanID",				FALSE),
	m_defriend			("Defriend",			FALSE),
	m_deleteInterval	("DeleteInterval",		2.5f,	[](const float& value)->BOOL{ return 0.0f <= value && value <= 60.0f; }),
	m_banDuration		("BanDuration",			1,		[](const int& value)->BOOL{ return value == 1 || value == 3 || value == 10; }),
	m_banReason			("BanReason",			_T("")),
	m_banTrigCount		("BanTrigCount",		1,		[](const int& value)->BOOL{ return 1 <= value; }),
	m_defriendTrigCount	("DefriendTrigCount",	5,		[](const int& value)->BOOL{ return 1 <= value; }),
	m_confirm			("Confirm",				TRUE),
	m_wapBanInterface	("WapBanInterface",		FALSE),
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
	m_options.push_back(&m_scanInterval);
	m_options.push_back(&m_onlyScanTitle);
	m_options.push_back(&m_scanPageCount);
	m_options.push_back(&m_briefLog);
	m_options.push_back(&m_threadCount);
	m_options.push_back(&m_autoSaveLog);
	m_options.push_back(&m_illegalLevel);
	m_options.push_back(&m_delete);
	m_options.push_back(&m_banID);
	m_options.push_back(&m_defriend);
	m_options.push_back(&m_deleteInterval);
	m_options.push_back(&m_banDuration);
	m_options.push_back(&m_banReason);
	m_options.push_back(&m_banTrigCount);
	m_options.push_back(&m_defriendTrigCount);
	m_options.push_back(&m_confirm);
	m_options.push_back(&m_wapBanInterface);
	m_options.push_back(&m_autoLoopBan);
	m_options.push_back(&m_keywords);
	m_options.push_back(&m_imageDir);
	m_options.push_back(&m_SSIMThreshold);
	m_options.push_back(&m_blackList);
	m_options.push_back(&m_whiteList);
	m_options.push_back(&m_whiteContent);
	m_options.push_back(&m_trustedThread);
};

void CPlan::PostChange()
{
	if (m_updateImage)
	{
		m_updateImage = FALSE;
		ReadImages(m_imageDir); 
	}
	m_optionsLock.Unlock();
}
