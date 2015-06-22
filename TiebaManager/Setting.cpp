#include "stdafx.h"
#include "Setting.h"
#include <zlib.h>
#include "Tieba.h"
#include "TiebaManagerDlg.h"


// 配置文件路径
CString		ALL_PROFILE_PATH = _T("\\options.tb");	// 程序运行时初始化
CString		USER_PROFILE_PATH;						// 确定贴吧时初始化
CString		OPTIONS_PATH = _T("Option\\");
CString		USERS_PATH = _T("\\User\\");			// 程序运行时初始化
CString		CURRENT_USER_PATH;						// 确定贴吧时初始化
CString		COOKIE_PATH;							// 确定贴吧时初始化
CString		CACHE_PATH;								// 确定贴吧时初始化

CString	g_currentUser;		// 当前账号
BOOL	g_autoUpdate;		// 自动更新

// 方案
CString	g_currentOption;	// 当前方案
int		g_scanInterval;		// 扫描间隔
BOOL	g_banID;			// 封ID
int		g_banDuration;		// 封禁时长
int		g_trigCount;		// 封禁违规次数
BOOL	g_onlyScanTitle;	// 只扫描标题
float	g_deleteInterval;	// 删帖间隔
BOOL	g_confirm;			// 操作前提示
int		g_scanPageCount;	// 扫描最后页数
BOOL	g_briefLog;			// 只输出删帖封号
BOOL	g_delete;			// 删帖
int		g_threadCount;		// 线程数
vector<RegexText>	g_keywords;		// 违规内容
vector<RegexText>	g_blackList;	// 屏蔽用户
vector<CString>		g_whiteList;	// 信任用户
vector<RegexText>	g_whiteContent;	// 信任内容

CCriticalSection g_optionsLock; // 判断违规用的临界区


static inline void ReadRegexTexts(const gzFile& f, vector<RegexText>& vec)
{
	int intBuf;
	gzread(f, &intBuf, sizeof(int)); // 长度
	vec.resize(intBuf);
	for (RegexText& i : vec)
	{
		gzread(f, &intBuf, sizeof(int)); // 是正则
		i.isRegex = intBuf != 0;
		gzread(f, &intBuf, sizeof(int)); // 字符串长度
		gzread(f, i.text.GetBuffer(intBuf), intBuf * sizeof(TCHAR)); // 字符串
		i.text.ReleaseBuffer(intBuf);
		i.regexp = i.isRegex ? i.text : _T("");
	}
}

// 读方案
void ReadOptions(LPCTSTR path)
{
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

	// 违规内容
	ReadRegexTexts(f, g_keywords);

	// 屏蔽用户
	ReadRegexTexts(f, g_blackList);

	// 信任用户
	int intBuf;
	gzread(f, &intBuf, sizeof(int)); // 长度
	g_whiteList.resize(intBuf);
	for (CString& i : g_whiteList)
	{
		gzread(f, &intBuf, sizeof(int)); // 字符串长度
		gzread(f, i.GetBuffer(intBuf), intBuf * sizeof(TCHAR)); // 字符串
		i.ReleaseBuffer(intBuf);
	}

	// 信任内容
	ReadRegexTexts(f, g_whiteContent);

	gzread(f, &g_scanInterval, sizeof(int));	// 扫描间隔
	gzread(f, &g_banID, sizeof(BOOL));			// 封ID
	gzread(f, &g_banDuration, sizeof(int));		// 封禁时长
	BOOL banIP;
	gzread(f, &banIP, sizeof(BOOL));			// 封IP
	gzread(f, &g_trigCount, sizeof(int));		// 封禁违规次数
	gzread(f, &g_onlyScanTitle, sizeof(BOOL));	// 只扫描标题
	gzread(f, &g_deleteInterval, sizeof(float));// 删帖间隔
	gzread(f, &g_confirm, sizeof(BOOL));		// 操作前提示
	gzread(f, &g_scanPageCount, sizeof(int));	// 扫描最后页数
	gzread(f, &g_briefLog, sizeof(BOOL));		// 只输出删帖封号
	if (gzread(f, &g_delete, sizeof(BOOL)) != sizeof(BOOL))			// 删帖
		g_delete = TRUE;
	if (gzread(f, &g_threadCount, sizeof(int)) != sizeof(int))		// 线程数
		g_threadCount = 2;

	gzclose(f);
	return;

UseDefaultOptions:
	g_keywords.clear();			// 违规内容
	g_blackList.clear();		// 屏蔽用户
	g_whiteList.clear();		// 信任用户
	g_scanInterval = 5;			// 扫描间隔
	g_banID = FALSE;			// 封ID
	g_banDuration = 1;			// 封禁时长
	g_trigCount = 1;			// 封禁违规次数
	g_onlyScanTitle = FALSE;	// 只扫描标题
	g_deleteInterval = 2.0f;	// 删帖间隔
	g_confirm = TRUE;			// 操作前提示
	g_scanPageCount = 1;		// 扫描最后页数
	g_briefLog = FALSE;			// 只输出删帖封号
	g_delete = TRUE;			// 删帖
	g_threadCount = 2;			// 线程数
}

static inline void WriteRegexTexts(const gzFile& f, vector<RegexText>& vec)
{
	int intBuf;
	gzwrite(f, &(intBuf = vec.size()), sizeof(int)); // 长度
	for (const RegexText& i : vec)
	{
		gzwrite(f, &(intBuf = i.isRegex ? 1 : 0), sizeof(int)); // 是正则
		int len = i.text.GetLength();
		gzwrite(f, &(intBuf = len), sizeof(int)); // 字符串长度
		gzwrite(f, (LPCTSTR)i.text, len * sizeof(TCHAR)); // 字符串
	}
}

// 写方案
void WriteOptions(LPCTSTR path)
{
	gzFile f = gzopen_w(path, "wb");
	if (f == NULL)
		return;

	// 头部
	gzwrite(f, "TB", 2);

	int intBuf;

	// 违规内容
	WriteRegexTexts(f, g_keywords);

	// 屏蔽用户
	WriteRegexTexts(f, g_blackList);

	// 信任用户
	gzwrite(f, &(intBuf = g_whiteList.size()), sizeof(int)); // 长度
	for (const CString& i : g_whiteList)
	{
		int len = i.GetLength();
		gzwrite(f, &len, sizeof(int)); // 字符串长度
		gzwrite(f, (LPCTSTR)i, len * sizeof(TCHAR)); // 字符串
	}

	// 信任内容
	WriteRegexTexts(f, g_whiteContent);

	gzwrite(f, &g_scanInterval, sizeof(int));		// 扫描间隔
	gzwrite(f, &g_banID, sizeof(BOOL));				// 封ID
	gzwrite(f, &g_banDuration, sizeof(int));		// 封禁时长
	BOOL banIP = FALSE;
	gzwrite(f, &banIP, sizeof(BOOL));				// 封IP
	gzwrite(f, &g_trigCount, sizeof(int));			// 封禁违规次数
	gzwrite(f, &g_onlyScanTitle, sizeof(BOOL));		// 只扫描标题
	gzwrite(f, &g_deleteInterval, sizeof(float));	// 删帖间隔
	gzwrite(f, &g_confirm, sizeof(BOOL));			// 操作前提示
	gzwrite(f, &g_scanPageCount, sizeof(int));		// 扫描最后页数
	gzwrite(f, &g_briefLog, sizeof(BOOL));			// 只输出删帖封号
	gzwrite(f, &g_delete, sizeof(BOOL));			// 删帖
	gzwrite(f, &g_threadCount, sizeof(int));		// 线程数

	gzclose(f);
}

static inline void ReadIDList(const gzFile& f, set<__int64>& IDList)
{
	int size;
	if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 100000) // 长度
	{
		__int64 id;
		for (int i = 0; i < size; i++)
		{
			gzread(f, &id, sizeof(__int64));
			IDList.insert(id);
		}
	}
}

static inline void WriteIDList(const gzFile& f, const set<__int64>& IDList)
{
	int len = IDList.size();
	gzwrite(f, &len, sizeof(int)); // 长度
	for (auto& i : IDList)
		gzwrite(f, &i, sizeof(__int64)); // ID
}

// 保存当前账号配置
void SaveCurrentUserProfile()
{
	// 创建目录
	if (!PathFileExists(USERS_PATH))
		CreateDirectory(USERS_PATH, NULL);
	if (!PathFileExists(USERS_PATH + g_currentUser))
		CreateDirectory(USERS_PATH + g_currentUser, NULL);

	// 保存Cookie
	gzFile f = gzopen_w(COOKIE_PATH, "wb");
	if (f != NULL)
	{
		int len = g_cookie.GetLength();
		gzwrite(f, &len, sizeof(int)); // 字符串长度
		gzwrite(f, (LPCTSTR)g_cookie, len * sizeof(TCHAR)); // 字符串
		gzclose(f);
	}

	// 保存历史回复、忽略ID
	f = gzopen_w(CACHE_PATH, "wb");
	if (f != NULL)
	{
		int len = g_reply.size();
		gzwrite(f, &len, sizeof(int)); // 长度
		for (auto& i : g_reply)
		{
			gzwrite(f, &i.first, sizeof(__int64)); // 主题ID
			gzwrite(f, &i.second, sizeof(int)); // 回复数
		}
		WriteIDList(f, g_initIgnoredTID);
		WriteIDList(f, g_initIgnoredPID);
		WriteIDList(f, g_initIgnoredLZLID);
		gzclose(f);
	}
}

// 设置当前账号
void SetCurrentUser(LPCTSTR userName)
{
	// 保存当前账号配置
	if (g_currentUser != _T(""))
		SaveCurrentUserProfile();

	// 设置配置路径
	g_currentUser = userName;
	CURRENT_USER_PATH = USERS_PATH + userName;
	USER_PROFILE_PATH = CURRENT_USER_PATH + _T("\\options.tb");
	COOKIE_PATH = CURRENT_USER_PATH + _T("\\ck.tb");
	CACHE_PATH = CURRENT_USER_PATH + _T("\\cache.tb");

	// 读取设置
	TCHAR buffer[260];
	// 方案
	GetPrivateProfileString(_T("Setting"), _T("Option"), _T("默认"), g_currentOption.GetBuffer(MAX_PATH), MAX_PATH, USER_PROFILE_PATH);
	g_currentOption.ReleaseBuffer();
	ReadOptions(OPTIONS_PATH + g_currentOption + _T(".tb"));
	// 贴吧名
	GetPrivateProfileString(_T("Setting"), _T("ForumName"), _T(""), buffer, _countof(buffer), USER_PROFILE_PATH);
	((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_forumNameEdit.SetWindowText(buffer);
	// Cookie
	gzFile f = gzopen_w(COOKIE_PATH, "rb");
	if (f != NULL)
	{
		int size;
		if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 1024 * 1024) // 字符串长度
		{
			gzread(f, g_cookie.GetBuffer(size), size * sizeof(TCHAR)); // 字符串
			g_cookie.ReleaseBuffer(size);
		}
		gzclose(f);
	}

	// 历史回复、忽略ID
	f = gzopen_w(CACHE_PATH, "rb");
	if (f != NULL)
	{
		int size;
		if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 100000) // 长度
		{
			__int64 tid;
			int reply;
			for (int i = 0; i < size; i++)
			{
				gzread(f, &tid, sizeof(__int64));
				gzread(f, &reply, sizeof(int));
				g_reply[tid] = reply;
			}
		}
		ReadIDList(f, g_initIgnoredTID);
		g_ignoredTID = g_initIgnoredTID;
		ReadIDList(f, g_initIgnoredPID);
		g_ignoredPID = g_initIgnoredPID;
		ReadIDList(f, g_initIgnoredLZLID);
		g_ignoredLZLID = g_initIgnoredLZLID;
		gzclose(f);
	}
}
