#include "stdafx.h"
#define KEYWORD_EXPORT
#include "Setting.h"
using tinyxml2::XMLElement;
#include "TiebaManagerDlg.h"
#include "TiebaVariable.h"
#include "ScanImage.h"
#include <StringHelper.h>
#include "MiscHelper.h"
#include "BlackListPage.h"


// 配置文件路径
CString		GLOBAL_CONFIG_PATH = _T("\\options.xml");	// 程序运行时初始化
CString		USER_PROFILE_PATH;							// 确定贴吧时初始化
CString		OPTIONS_PATH = _T("Option\\");
CString		USERS_PATH = _T("\\User\\");				// 程序运行时初始化
CString		CURRENT_USER_PATH;							// 确定贴吧时初始化
CString		COOKIE_PATH;								// 确定贴吧时初始化
CString		CACHE_PATH;									// 确定贴吧时初始化

CGlobalConfig g_globalConfig;
CUserConfig g_userConfig;

// 方案
CPlan g_plan;


// 方案类实现
#undef HELPER_API
#define HELPER_API __declspec(dllexport)

DECLEAR_READ(CPlan::Keyword)
{
	const XMLElement* optionNode = root.FirstChildElement(thiz.m_name);
	if (optionNode == NULL)
	{
		thiz.UseDefault();
		return root;
	}

	COption<BOOL> isRegex("IsRegex");
	COption<CString> text("Text");
	COption<BOOL> forceToConfirm("ForceToConfirm");
	COption<int> trigCount("TrigCount");
	isRegex << *optionNode;
	text << *optionNode;
	forceToConfirm << *optionNode;
	trigCount << *optionNode;

	thiz.m_value.Set(isRegex, text);
	thiz.m_value.forceToConfirm = forceToConfirm;
	thiz.m_value.trigCount = trigCount;

	if (!thiz.IsValid(thiz.m_value))
		thiz.UseDefault();
	return root;
}

DECLEAR_WRITE(CPlan::Keyword)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(thiz.m_name);
	root.LinkEndChild(optionNode);

	COption<BOOL> isRegex("IsRegex");
	*isRegex = thiz.m_value.isRegex;
	isRegex >> *optionNode;
	COption<CString> text("Text");
	*text = thiz.m_value.text;
	text >> *optionNode;
	COption<BOOL> forceToConfirm("ForceToConfirm");
	*forceToConfirm = thiz.m_value.forceToConfirm;
	forceToConfirm >> *optionNode;
	COption<int> trigCount("TrigCount");
	*trigCount = thiz.m_value.trigCount;
	trigCount >> *optionNode;
	return root;
}

DEFINE_READ_VECTOR(CPlan::Keyword)

DEFINE_WRITE_VECTOR(CPlan::Keyword)

#undef HELPER_API
#define HELPER_API __declspec(dllimport)

CPlan::CPlan()
	: CConfigBase("Plan"),
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


// 保存当前账号配置
void SaveCurrentUserProfile()
{
	// 创建目录
	CreateDir(USERS_PATH + g_globalConfig.m_currentUser);

	// 当前账号配置
	g_userConfig.Save(USER_PROFILE_PATH);

	// Cookie
	g_userTiebaInfo.Save(COOKIE_PATH);

	// 历史回复、忽略ID等
	g_userCache.Save(CACHE_PATH);
}

// 设置当前账号
void SetCurrentUser(const CString& userName, BOOL save)
{
	// 保存当前账号配置
	if (save)
		SaveCurrentUserProfile();

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
	g_userTiebaInfo.Load(COOKIE_PATH);

	// 历史回复、忽略ID等
	g_userCache.Load(CACHE_PATH);
}
