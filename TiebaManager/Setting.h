#pragma once
#include <zlib.h>
#include "TypeHelper.h"
#include "Update.h"


// 配置文件路径
extern CString	GLOBAL_CONFIG_PATH;	// 程序运行时初始化
extern CString	USER_PROFILE_PATH;	// 确定贴吧时初始化
extern CString	OPTIONS_PATH;
extern CString	USERS_PATH;			// 确定贴吧时初始化
extern CString	CURRENT_USER_PATH;	// 确定贴吧时初始化
extern CString	COOKIE_PATH;		// 确定贴吧时初始化
extern CString	CACHE_PATH;			// 确定贴吧时初始化


// Option、Config类声明 //////////////////////////////////////////////////////////////
namespace tinyxml2{ class XMLElement; }

class COptionBase
{
public:
	const CStringA m_name;

	COptionBase(const CStringA& name) : m_name(name) {}
	virtual ~COptionBase() {}

	virtual void UseDefault() = 0;
	virtual tinyxml2::XMLElement& operator << (tinyxml2::XMLElement& root) = 0;
	virtual tinyxml2::XMLElement& operator >> (tinyxml2::XMLElement& root) const = 0;
};

template <class T>
class COption : public COptionBase
{
public:
	const T m_default;
	T m_value;
	typedef BOOL(*IsValidFunc)(const T& value);
	const IsValidFunc IsValid;

	COption(const CStringA& name, IsValidFunc _isValid = [](const T&){ return TRUE; })
		: COptionBase(name), m_default(), IsValid(_isValid)
	{}
	COption(const CStringA& name, const T& _default, IsValidFunc _isValid = [](const T&){ return TRUE; })
		: COptionBase(name), m_default(_default), IsValid(_isValid)
	{}

	bool operator == (const COption&) const = delete;
	operator const T& () const{ return m_value; }
	operator T& (){ return m_value; }
	const T& operator * () const{ return m_value; }
	T& operator * (){ return m_value; }
	const T* operator -> () const{ return &m_value; }
	T* operator -> (){ return &m_value; }
	void UseDefault(){ m_value = m_default; }

	tinyxml2::XMLElement& operator << (tinyxml2::XMLElement& root);
	tinyxml2::XMLElement& operator >> (tinyxml2::XMLElement& root) const;
};

class CConfigBase
{
protected:
	vector<COptionBase*> m_options;

public:
	const LPCSTR m_name;

	CConfigBase(LPCSTR name) : m_name(name) {}
	virtual ~CConfigBase() {}

	virtual BOOL LoadOld(const CString& path) { return FALSE; }; // 万恶的历史遗留问题
	virtual BOOL Load(const CString& path);
	virtual BOOL Save(const CString& path) const;
	virtual void UseDefault();
	virtual void OnChange() {}
	virtual void PostChange() {}
};

// 全局配置
class CGlobalConfig : public CConfigBase
{
public:
	COption<BOOL> m_firstRun;			// 第一次运行
	COption<BOOL> m_firstRunAfterUpdate;// 更新后第一次运行
	COption<CString> m_currentUser;		// 当前账号
	COption<BOOL> m_autoUpdate;			// 自动更新

	CGlobalConfig()
		: CConfigBase("Global"),
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

	BOOL LoadOld(const CString& path)
	{
		*m_firstRun = GetPrivateProfileInt(_T("Setting"), _T("FirstRun"), TRUE, path) != FALSE;
		TCHAR buffer[260];
		GetPrivateProfileString(_T("Setting"), _T("UserName"), _T("[NULL]"), buffer, _countof(buffer), path);
		*m_currentUser = buffer;
		*m_autoUpdate = GetPrivateProfileInt(_T("Setting"), _T("AutoUpdate"), TRUE, path) != FALSE;
		return TRUE;
	}
};
extern CGlobalConfig g_globalConfig;

// 用户配置
class CUserConfig : public CConfigBase
{
public:
	COption<CString> m_plan;		// 当前方案
	COption<CString> m_forumName;	// 贴吧名

	CUserConfig()
		: CConfigBase("User"),
		m_plan("Plan", _T("默认")),
		m_forumName("ForumName")
	{
		m_options.push_back(&m_plan);
		m_options.push_back(&m_forumName);
	}

	BOOL LoadOld(const CString& path)
	{
		GetPrivateProfileString(_T("Setting"), _T("Option"), _T("默认"), m_plan->GetBuffer(MAX_PATH), MAX_PATH, path);
		m_plan->ReleaseBuffer();
		GetPrivateProfileString(_T("Setting"), _T("ForumName"), _T(""), m_forumName->GetBuffer(MAX_PATH), MAX_PATH, path);
		m_forumName->ReleaseBuffer();
		return TRUE;
	}
};
extern CUserConfig g_userConfig;

// 方案
class CPlan : public CConfigBase
{
public:
	CCriticalSection m_optionsLock; // 方案临界区
	BOOL m_updateImage; // 读取后更新违规图片

	COption<int>		m_scanInterval;			// 扫描间隔
	COption<BOOL>		m_onlyScanTitle;		// 只扫描标题
	COption<int>		m_scanPageCount;		// 扫描最后页数
	COption<BOOL>		m_briefLog;				// 只输出删帖封号
	COption<int>		m_threadCount;			// 线程数
	COption<BOOL>		m_autoSaveLog;			// 自动保存日志
	COption<int>		m_illegalLevel;			// 违规等级
	COption<BOOL>		m_delete;				// 删帖
	COption<BOOL>		m_banID;				// 封ID
	COption<BOOL>		m_defriend;				// 拉黑
	COption<float>		m_deleteInterval;		// 删帖间隔
	COption<int>		m_banDuration;			// 封禁时长
	COption<CString>	m_banReason;			// 封号原因
	COption<int>		m_banTrigCount;			// 封禁违规次数
	COption<int>		m_defriendTrigCount;	// 拉黑违规次数
	COption<BOOL>		m_confirm;				// 操作前提示
	COption<vector<RegexText> >	m_keywords;		// 违规内容
	vector<NameImage>			m_images;		// 违规图片
	COption<CString>	m_imageDir;				// 违规图片目录
	COption<double>		m_SSIMThreshold;		// 阈值
	COption<vector<RegexText> >	m_blackList;	// 屏蔽用户
	COption<set<CString> >		m_whiteList;	// 信任用户
	COption<vector<RegexText> >	m_whiteContent;	// 信任内容
	COption<set<CString> >		m_trustedThread;// 信任主题

	CPlan();
	BOOL LoadOld(const CString& path);
	void OnChange(){ m_optionsLock.Lock(); }
	void PostChange();
};
extern CPlan g_plan;


// 读字符串
inline BOOL ReadText(const gzFile& f, CString& text)
{
	int size;
	if (gzread(f, &size, sizeof(int)) != sizeof(int) || size < 0 || size > 100000) // 字符串长度
	{
		text = _T("");
		return FALSE;
	}
	if (gzread(f, text.GetBuffer(size), size * sizeof(TCHAR)) != size * sizeof(TCHAR)) // 字符串
	{
		text.ReleaseBuffer(0);
		return FALSE;
	}
	text.ReleaseBuffer(size);
	return TRUE;
}
// 写字符串
inline void WriteText(const gzFile& f, const CString& text)
{
	int size = text.GetLength();
	gzwrite(f, &size, sizeof(int)); // 字符串长度
	gzwrite(f, (LPCTSTR)text, size * sizeof(TCHAR)); // 字符串
}

// 保存当前账号配置
void SaveCurrentUserProfile();
// 设置当前账号
void SetCurrentUser(const CString& userName, BOOL save);
