#pragma once
#include <zlib.h>
#include "TypeHelper.h"


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
	const LPCSTR m_name;

	COptionBase(LPCSTR name) : m_name(name) {}
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

	COption(LPCSTR name, IsValidFunc _isValid = [](const T&){ return TRUE; })
		: COptionBase(name), m_default(), IsValid(_isValid)
	{}
	COption(LPCSTR name, const T& _default, IsValidFunc _isValid = [](const T&){ return TRUE; })
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
};

// 全局配置
class CGlobalConfig : public CConfigBase
{
public:
	COption<BOOL> m_firstRun;			// 第一次运行
	COption<CString> m_currentUser;		// 当前账号
	COption<BOOL> m_autoUpdate;			// 自动更新

	CGlobalConfig()
		: CConfigBase("Global"),
		m_firstRun("FirstRun", TRUE),
		m_currentUser("UserName", _T("[NULL]"), [](const CString& value)->BOOL{ return value != _T(""); }),
		m_autoUpdate("AutoUpdate", TRUE)
	{
		m_options.push_back(&m_firstRun);
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
extern int		g_scanInterval;		// 扫描间隔
extern BOOL		g_banID;			// 封ID
extern int		g_banDuration;		// 封禁时长
extern int		g_banTrigCount;		// 封禁违规次数
extern BOOL		g_onlyScanTitle;	// 只扫描标题
extern float	g_deleteInterval;	// 删帖间隔
extern BOOL		g_confirm;			// 操作前提示
extern int		g_scanPageCount;	// 扫描最后页数
extern BOOL		g_briefLog;			// 只输出删帖封号
extern BOOL		g_delete;			// 删帖
extern int		g_threadCount;		// 线程数
extern CString	g_banReason;		// 封号原因
extern CString	g_imageDir;			// 违规图片目录
extern double	g_SSIMThreshold;	// 阈值
extern BOOL		g_defriend;			// 拉黑
extern int		g_defriendTrigCount; // 拉黑违规次数
extern BOOL		g_autoSaveLog;		// 自动保存日志
extern vector<RegexText>	g_keywords;		// 违规内容
extern vector<RegexText>	g_blackList;	// 屏蔽用户
extern set<CString>			g_whiteList;	// 信任用户
extern vector<RegexText>	g_whiteContent;	// 信任内容
extern vector<NameImage>	g_images;		// 违规图片
extern set<CString>			g_trustedThread;// 信任主题

extern CCriticalSection g_optionsLock; // 方案临界区


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
void ReadOptions(LPCTSTR path);
void WriteOptions(LPCTSTR path);
// 保存当前账号配置
void SaveCurrentUserProfile();
// 设置当前账号
void SetCurrentUser(const CString& userName);
