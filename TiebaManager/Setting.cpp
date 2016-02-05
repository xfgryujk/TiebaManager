#include "stdafx.h"
#include "Setting.h"
#include <tinyxml2.h>
using namespace tinyxml2;
#include "TiebaManagerDlg.h"
#include "TiebaVariable.h"
#include "ScanImage.h"
#include "StringHelper.h"
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

#pragma region COption
// COption实现 ///////////////////////////////////////////////////////////////////////////

// 读基本类型

XMLElement& COption<int>::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
UseDefault:
		UseDefault();
		return root;
	}
	XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	m_value = atoi(value);
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<float>::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
	UseDefault:
		UseDefault();
		return root;
	}
	XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	m_value = (float)atof(value);
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<double>::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
	UseDefault:
		UseDefault();
		return root;
	}
	XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	m_value = atof(value);
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<__int64>::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
UseDefault:
		UseDefault();
		return root;
	}
	XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	m_value = _atoi64(value);
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<CString>::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
UseDefault:
		UseDefault();
		return root;
	}
	XMLNode* content = optionNode->FirstChild();
	if (content == NULL)
		goto UseDefault;
	LPCSTR value = content->ToText()->Value();

	m_value = value;
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<RegexText>::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	COption<BOOL> isRegex("IsRegex");
	COption<CString> text("Text");
	isRegex << *optionNode;
	text << *optionNode;
	
	m_value.Set(isRegex, text);

	if (!IsValid(m_value))
		UseDefault();
	return root;
}

// 读vector

XMLElement& COption<vector<CString> >::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	m_value.clear();
	COption<CString> value("value");
	for (XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
	{
		value << *item;
		m_value.push_back(value);
	}
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<vector<RegexText> >::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	m_value.clear();
	COption<RegexText> value("value");
	for (XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
	{
		value << *item;
		m_value.push_back(value);
	}
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

// 读set

XMLElement& COption<set<__int64> >::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	m_value.clear();
	COption<__int64> value("value");
	for (XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
	{
		value << *item;
		m_value.insert(value);
	}
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<set<CString> >::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	m_value.clear();
	COption<CString> value("value");
	for (XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
	{
		value << *item;
		m_value.insert(value);
	}
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

// 读map

XMLElement& COption<map<__int64, int> >::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	m_value.clear();
	COption<__int64> key("key");
	COption<int> value("value");
	for (XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
	{
		key << *item;
		value << *item;
		m_value[key] = value;
	}
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

XMLElement& COption<map<CString, int> >::operator << (XMLElement& root)
{
	XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return root;
	}

	m_value.clear();
	COption<CString> key("key");
	COption<int> value("value");
	for (XMLElement* item = optionNode->FirstChildElement("item"); item != NULL; item = item->NextSiblingElement("item"))
	{
		key << *item;
		value << *item;
		m_value[key] = value;
	}
	if (!IsValid(m_value))
		UseDefault();
	return root;
}

// 写基本类型

XMLElement& COption<int>::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	char buffer[15];
	_itoa_s(m_value, buffer, _countof(buffer), 10);
	optionNode->LinkEndChild(doc->NewText(buffer));
	return root;
}

XMLElement& COption<float>::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	char buffer[25];
	sprintf_s(buffer, "%f", m_value);
	optionNode->LinkEndChild(doc->NewText(buffer));
	return root;
}

XMLElement& COption<double>::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	char buffer[50];
	sprintf_s(buffer, "%f", m_value);
	optionNode->LinkEndChild(doc->NewText(buffer));
	return root;
}

XMLElement& COption<__int64>::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	char buffer[25];
	_i64toa_s(m_value, buffer, _countof(buffer), 10);
	optionNode->LinkEndChild(doc->NewText(buffer));
	return root;
}

XMLElement& COption<CString>::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	optionNode->LinkEndChild(doc->NewText(CStringA(m_value)));
	return root;
}

XMLElement& COption<RegexText>::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<BOOL> isRegex("IsRegex");
	*isRegex = m_value.isRegex;
	isRegex >> *optionNode;
	COption<CString> text("Text");
	*text = m_value.text;
	text >> *optionNode;
	return root;
}

// 写vector

XMLElement& COption<vector<CString> >::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<CString> value("value");
	for (const CString& i : m_value)
	{
		XMLElement* item = doc->NewElement("item");
		optionNode->LinkEndChild(item);
		*value = i;
		value >> *item;
	}
	return root;
}

XMLElement& COption<vector<RegexText> >::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<RegexText> value("value");
	for (const RegexText& i : m_value)
	{
		XMLElement* item = doc->NewElement("item");
		optionNode->LinkEndChild(item);
		*value = i;
		value >> *item;
	}
	return root;
}

// 写set

XMLElement& COption<set<__int64> >::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<__int64> value("value");
	for (const __int64& i : m_value)
	{
		XMLElement* item = doc->NewElement("item");
		optionNode->LinkEndChild(item);
		*value = i;
		value >> *item;
	}
	return root;
}

XMLElement& COption<set<CString> >::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<CString> value("value");
	for (const CString& i : m_value)
	{
		XMLElement* item = doc->NewElement("item");
		optionNode->LinkEndChild(item);
		*value = i;
		value >> *item;
	}
	return root;
}

// 写map

XMLElement& COption<map<__int64, int> >::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<__int64> key("key");
	COption<int> value("value");
	for (const auto& i : m_value)
	{
		XMLElement* item = doc->NewElement("item");
		optionNode->LinkEndChild(item);
		*key = i.first;
		key >> *item;
		*value = i.second;
		value >> *item;
	}
	return root;
}

XMLElement& COption<map<CString, int> >::operator >> (XMLElement& root) const
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<CString> key("key");
	COption<int> value("value");
	for (const auto& i : m_value)
	{
		XMLElement* item = doc->NewElement("item");
		optionNode->LinkEndChild(item);
		*key = i.first;
		key >> *item;
		*value = i.second;
		value >> *item;
	}
	return root;
}
#pragma endregion

// 配置读写实现 ///////////////////////////////////////////////////////////////////////////

BOOL CConfigBase::Load(const CString& path)
{
	FILE* f = NULL;
	if (_tfopen_s(&f, path, _T("rb")) != 0)
	{
		OnChange();
		CString oldPath = path.Left(path.GetLength() - 3) + _T("tb");
		BOOL res = LoadOld(oldPath);
		Save(path);
		DeleteFile(oldPath);
		PostChange();
		if (!res)
			UseDefault();
		return res;
	}

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(f) != XML_NO_ERROR)
	{
		fclose(f);
		UseDefault();
		return FALSE;
	}
	fclose(f);

	XMLElement* root = doc.FirstChildElement(m_name);
	if (root == NULL)
	{
		UseDefault();
		return FALSE;
	}

	OnChange();
	for (COptionBase* i : m_options)
		*i << *root;
	PostChange();
	return TRUE;
}

BOOL CConfigBase::Save(const CString& path) const
{
	FILE* f = NULL;
	if (_tfopen_s(&f, path, _T("wb")) != 0)
		return FALSE;

	tinyxml2::XMLDocument doc;
	doc.LinkEndChild(doc.NewDeclaration("xml version=\"1.0\" encoding=\"GBK\""));
	tinyxml2::XMLElement* root = doc.NewElement(m_name);
	doc.LinkEndChild(root);

	for (const COptionBase* i : m_options)
		*i >> *root;

	BOOL res = doc.SaveFile(f) == XML_NO_ERROR;
	fclose(f);
	return res;
}

void CConfigBase::UseDefault()
{
	OnChange();
	for (COptionBase* i : m_options)
		i->UseDefault();
	PostChange();
}

// 方案类实现

CPlan::CPlan()
	: CConfigBase("Plan"),
	m_scanInterval		("ScanInterval",		5,		[](const int& value)->BOOL{ return 0 <= value && value <= 600; }),
	m_onlyScanTitle		("OnlyScanTitle",		FALSE),
	m_scanPageCount		("ScanPageCount",		1,		[](const int& value)->BOOL{ return 1 <= value; }),
	m_briefLog			("BriefLog",			FALSE),
	m_threadCount		("ThreadCount",			2,		[](const int& value)->BOOL{ return 1 <= value && value <= 16; }),
	m_autoSaveLog		("AutoSaveLog",			FALSE),
	m_delete			("Delete",				TRUE),
	m_banID				("BanID",				FALSE),
	m_defriend			("Defriend",			FALSE),
	m_deleteInterval	("DeleteInterval",		2.0f,	[](const float& value)->BOOL{ return 0.0f <= value && value <= 60.0f; }),
	m_banDuration		("BanDuration",			1,		[](const int& value)->BOOL{ return value == 1 || value == 3 || value == 10; }),
	m_banReason			("BanReason",			_T("")),
	m_banTrigCount		("BanTrigCount",		1,		[](const int& value)->BOOL{ return 1 <= value; }),
	m_defriendTrigCount	("DefriendTrigCount",	5,		[](const int& value)->BOOL{ return 1 <= value; }),
	m_confirm			("Confirm",				TRUE),
	m_keywords			("IllegalContent", [](const vector<RegexText>& value)->BOOL
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
	m_options.push_back(&m_delete);
	m_options.push_back(&m_banID);
	m_options.push_back(&m_defriend);
	m_options.push_back(&m_deleteInterval);
	m_options.push_back(&m_banDuration);
	m_options.push_back(&m_banReason);
	m_options.push_back(&m_banTrigCount);
	m_options.push_back(&m_defriendTrigCount);
	m_options.push_back(&m_confirm);
	m_options.push_back(&m_keywords);
	m_options.push_back(&m_imageDir);
	m_options.push_back(&m_SSIMThreshold);
	m_options.push_back(&m_blackList);
	m_options.push_back(&m_whiteList);
	m_options.push_back(&m_whiteContent);
	m_options.push_back(&m_trustedThread);
}

static inline void ReadRegexTexts(const gzFile& f, vector<RegexText>& vec)
{
	int intBuf;
	gzread(f, &intBuf, sizeof(int)); // 长度
	vec.resize(intBuf);
	for (RegexText& i : vec)
	{
		gzread(f, &intBuf, sizeof(int)); // 是正则
		i.isRegex = intBuf != 0;
		ReadText(f, i.text);
		i.regexp = i.isRegex ? i.text : _T("");
	}
}

static inline void WriteRegexTexts(const gzFile& f, vector<RegexText>& vec)
{
	int intBuf;
	gzwrite(f, &(intBuf = vec.size()), sizeof(int)); // 长度
	for (const RegexText& i : vec)
	{
		gzwrite(f, &(intBuf = i.isRegex ? 1 : 0), sizeof(int)); // 是正则
		WriteText(f, i.text);
	}
}

BOOL CPlan::LoadOld(const CString& path)
{
	CString strBuf;

	gzFile f = gzopen_w(path, "rb");
	if (f == NULL)
		goto UseDefaultOptions;

	// 头部
	char header[2];
	gzread(f, header, sizeof(header));
	if (header[0] != 'T' || header[1] != 'B')
	{
		gzclose(f);
		goto UseDefaultOptions;
	}

	m_optionsLock.Lock();

	// 违规内容
	ReadRegexTexts(f, m_keywords);

	// 屏蔽用户
	ReadRegexTexts(f, m_blackList);

	// 信任用户
	int intBuf;
	gzread(f, &intBuf, sizeof(int)); // 长度
	for (int i = 0; i < intBuf; i++)
	{
		ReadText(f, strBuf);
		m_whiteList->insert(strBuf);
	}

	// 信任内容
	ReadRegexTexts(f, m_whiteContent);

	gzread(f, &*m_scanInterval, sizeof(int));	// 扫描间隔
	gzread(f, &*m_banID, sizeof(BOOL));			// 封ID
	gzread(f, &*m_banDuration, sizeof(int));	// 封禁时长
	BOOL banIP;
	gzread(f, &banIP, sizeof(BOOL));			// 封IP
	gzread(f, &*m_banTrigCount, sizeof(int));	// 封禁违规次数
	gzread(f, &*m_onlyScanTitle, sizeof(BOOL));	// 只扫描标题
	gzread(f, &*m_deleteInterval, sizeof(float));// 删帖间隔
	gzread(f, &*m_confirm, sizeof(BOOL));		// 操作前提示
	gzread(f, &*m_scanPageCount, sizeof(int));	// 扫描最后页数
	gzread(f, &*m_briefLog, sizeof(BOOL));		// 只输出删帖封号
	if (gzread(f, &*m_delete, sizeof(BOOL)) != sizeof(BOOL))			// 删帖
		*m_delete = TRUE;
	if (gzread(f, &*m_threadCount, sizeof(int)) != sizeof(int))			// 线程数
		*m_threadCount = 2;
	ReadText(f, m_banReason);											// 封禁原因
	if (!ReadText(f, m_imageDir))										// 违规图片目录
		m_images.clear();												// 违规图片
	if (gzread(f, &*m_SSIMThreshold, sizeof(double)) != sizeof(double))	// 阈值
		*m_SSIMThreshold = 2.43;

	// 信任主题
	m_trustedThread->clear();
	if (gzread(f, &intBuf, sizeof(int)) == sizeof(int)) // 长度
	for (int i = 0; i < intBuf; i++)
	{
		ReadText(f, strBuf);
		m_trustedThread->insert(strBuf);
	}

	if (gzread(f, &*m_defriend, sizeof(BOOL)) != sizeof(BOOL))		// 拉黑
		*m_defriend = FALSE;
	if (gzread(f, &*m_defriendTrigCount, sizeof(int)) != sizeof(int)) // 拉黑违规次数
		*m_defriendTrigCount = 5;
	if (gzread(f, &*m_autoSaveLog, sizeof(BOOL)) != sizeof(BOOL))	// 自动保存日志
		*m_autoSaveLog = FALSE;

	m_optionsLock.Unlock();

	gzclose(f);
	return TRUE;

UseDefaultOptions:
	m_optionsLock.Lock();
	m_keywords->clear();		// 违规内容
	m_blackList->clear();		// 屏蔽用户
	m_whiteList->clear();		// 信任用户
	m_whiteContent->clear();	// 信任内容
	*m_scanInterval = 5;		// 扫描间隔
	*m_banID = FALSE;			// 封ID
	*m_banDuration = 1;			// 封禁时长
	*m_banTrigCount = 1;		// 封禁违规次数
	*m_onlyScanTitle = FALSE;	// 只扫描标题
	*m_deleteInterval = 2.0f;	// 删帖间隔
	*m_confirm = TRUE;			// 操作前提示
	*m_scanPageCount = 1;		// 扫描最后页数
	*m_briefLog = FALSE;		// 只输出删帖封号
	*m_delete = TRUE;			// 删帖
	*m_threadCount = 2;			// 线程数
	*m_banReason = _T("");		// 封禁原因
	*m_imageDir = _T("");		// 违规图片目录
	m_images.clear();			// 违规图片
	*m_SSIMThreshold = 2.43f;	// 阈值
	m_trustedThread->clear();	// 信任主题
	*m_defriend = FALSE;		// 拉黑
	*m_defriendTrigCount = 5;	// 拉黑违规次数
	*m_autoSaveLog = FALSE;		// 自动保存日志
	m_optionsLock.Unlock();
	return TRUE;
}

void CPlan::PostChange()
{
	if (m_updateImage)
	{
		m_updateImage = FALSE;
		ReadImages(m_imageDir); 
	}
	m_optionsLock.Unlock();
}
