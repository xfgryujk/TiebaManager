#include "stdafx.h"
#include "Setting.h"
#include "TiebaManagerDlg.h"
#include "TiebaVariable.h"
#include "ScanImage.h"


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
int		g_banTrigCount;		// 封禁违规次数
BOOL	g_onlyScanTitle;	// 只扫描标题
float	g_deleteInterval;	// 删帖间隔
BOOL	g_confirm;			// 操作前提示
int		g_scanPageCount;	// 扫描最后页数
BOOL	g_briefLog;			// 只输出删帖封号
BOOL	g_delete;			// 删帖
int		g_threadCount;		// 线程数
CString	g_banReason;		// 封号原因
CString	g_imageDir;			// 违规图片目录
double	g_SSIMThreshold;	// 阈值
BOOL	g_defriend;			// 拉黑
int		g_defriendTrigCount; // 拉黑违规次数
BOOL	g_autoSaveLog;		// 自动保存日志
vector<RegexText>	g_keywords;		// 违规内容
vector<RegexText>	g_blackList;	// 屏蔽用户
set<CString>		g_whiteList;	// 信任用户
vector<RegexText>	g_whiteContent;	// 信任内容
vector<NameImage>	g_images;		// 违规图片
set<CString>		g_trustedThread;// 信任主题

CCriticalSection g_optionsLock; // 方案临界区


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

// 读方案
void ReadOptions(LPCTSTR path)
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

	g_optionsLock.Lock();

	// 违规内容
	ReadRegexTexts(f, g_keywords);

	// 屏蔽用户
	ReadRegexTexts(f, g_blackList);

	// 信任用户
	int intBuf;
	gzread(f, &intBuf, sizeof(int)); // 长度
	for (int i = 0; i < intBuf; i++)
	{
		ReadText(f, strBuf);
		g_whiteList.insert(strBuf);
	}

	// 信任内容
	ReadRegexTexts(f, g_whiteContent);

	gzread(f, &g_scanInterval, sizeof(int));	// 扫描间隔
	gzread(f, &g_banID, sizeof(BOOL));			// 封ID
	gzread(f, &g_banDuration, sizeof(int));		// 封禁时长
	BOOL banIP;
	gzread(f, &banIP, sizeof(BOOL));			// 封IP
	gzread(f, &g_banTrigCount, sizeof(int));	// 封禁违规次数
	gzread(f, &g_onlyScanTitle, sizeof(BOOL));	// 只扫描标题
	gzread(f, &g_deleteInterval, sizeof(float));// 删帖间隔
	gzread(f, &g_confirm, sizeof(BOOL));		// 操作前提示
	gzread(f, &g_scanPageCount, sizeof(int));	// 扫描最后页数
	gzread(f, &g_briefLog, sizeof(BOOL));		// 只输出删帖封号
	if (gzread(f, &g_delete, sizeof(BOOL)) != sizeof(BOOL))			// 删帖
		g_delete = TRUE;
	if (gzread(f, &g_threadCount, sizeof(int)) != sizeof(int))		// 线程数
		g_threadCount = 2;
	ReadText(f, g_banReason);										// 封禁原因
	if (!ReadText(f, g_imageDir))									// 违规图片目录
		g_images.clear();
	else
		ReadImages(g_imageDir);										// 违规图片
	if (gzread(f, &g_SSIMThreshold, sizeof(double)) != sizeof(double))	// 阈值
		g_SSIMThreshold = 2.43;

	// 信任主题
	g_trustedThread.clear();
	if (gzread(f, &intBuf, sizeof(int)) == sizeof(int)) // 长度
		for (int i = 0; i < intBuf; i++)
		{
			ReadText(f, strBuf);
			g_trustedThread.insert(strBuf);
		}

	if (gzread(f, &g_defriend, sizeof(BOOL)) != sizeof(BOOL))		// 拉黑
		g_defriend = FALSE;
	if (gzread(f, &g_defriendTrigCount, sizeof(int)) != sizeof(int)) // 拉黑违规次数
		g_defriendTrigCount = 5;
	if (gzread(f, &g_autoSaveLog, sizeof(BOOL)) != sizeof(BOOL))	// 自动保存日志
		g_autoSaveLog = FALSE;

	g_optionsLock.Unlock();

	gzclose(f);
	return;

UseDefaultOptions:
	g_optionsLock.Lock();
	g_keywords.clear();			// 违规内容
	g_blackList.clear();		// 屏蔽用户
	g_whiteList.clear();		// 信任用户
	g_whiteContent.clear();		// 信任内容
	g_scanInterval = 5;			// 扫描间隔
	g_banID = FALSE;			// 封ID
	g_banDuration = 1;			// 封禁时长
	g_banTrigCount = 1;			// 封禁违规次数
	g_onlyScanTitle = FALSE;	// 只扫描标题
	g_deleteInterval = 2.0f;	// 删帖间隔
	g_confirm = TRUE;			// 操作前提示
	g_scanPageCount = 1;		// 扫描最后页数
	g_briefLog = FALSE;			// 只输出删帖封号
	g_delete = TRUE;			// 删帖
	g_threadCount = 2;			// 线程数
	g_banReason = _T("");		// 封禁原因
	g_imageDir = _T("");		// 违规图片目录
	g_images.clear();			// 违规图片
	g_SSIMThreshold = 2.43;		// 阈值
	g_trustedThread.clear();	// 信任主题
	g_defriend = FALSE;			// 拉黑
	g_defriendTrigCount = 5;	// 拉黑违规次数
	g_autoSaveLog = FALSE;		// 自动保存日志
	g_optionsLock.Unlock();
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
		WriteText(f, i);

	// 信任内容
	WriteRegexTexts(f, g_whiteContent);

	gzwrite(f, &g_scanInterval, sizeof(int));		// 扫描间隔
	gzwrite(f, &g_banID, sizeof(BOOL));				// 封ID
	gzwrite(f, &g_banDuration, sizeof(int));		// 封禁时长
	BOOL banIP = FALSE;
	gzwrite(f, &banIP, sizeof(BOOL));				// 封IP
	gzwrite(f, &g_banTrigCount, sizeof(int));		// 封禁违规次数
	gzwrite(f, &g_onlyScanTitle, sizeof(BOOL));		// 只扫描标题
	gzwrite(f, &g_deleteInterval, sizeof(float));	// 删帖间隔
	gzwrite(f, &g_confirm, sizeof(BOOL));			// 操作前提示
	gzwrite(f, &g_scanPageCount, sizeof(int));		// 扫描最后页数
	gzwrite(f, &g_briefLog, sizeof(BOOL));			// 只输出删帖封号
	gzwrite(f, &g_delete, sizeof(BOOL));			// 删帖
	gzwrite(f, &g_threadCount, sizeof(int));		// 线程数
	WriteText(f, g_banReason);						// 封禁原因
	WriteText(f, g_imageDir);						// 违规图片目录
	gzwrite(f, &g_SSIMThreshold, sizeof(double));	// 阈值

	// 信任主题
	gzwrite(f, &(intBuf = g_trustedThread.size()), sizeof(int)); // 长度
	for (const CString& i : g_trustedThread)
		WriteText(f, i);

	gzwrite(f, &g_defriend, sizeof(BOOL));			// 拉黑
	gzwrite(f, &g_defriendTrigCount, sizeof(int));	// 拉黑违规次数
	gzwrite(f, &g_autoSaveLog, sizeof(BOOL));		// 自动保存日志

	gzclose(f);
}

static inline void ReadIDSet(const gzFile& f, set<__int64>& IDSet)
{
	IDSet.clear();
	int size;
	if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 100000) // 长度
	{
		__int64 id;
		for (int i = 0; i < size; i++)
		{
			gzread(f, &id, sizeof(__int64));
			IDSet.insert(id);
		}
	}
}

static inline void ReadTextSet(const gzFile& f, set<CString>& TextSet)
{
	TextSet.clear();
	int size;
	if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 100000) // 长度
	{
		CString text;
		for (int i = 0; i < size; i++)
		{
			ReadText(f, text);
			TextSet.insert(text);
		}
	}
}

static inline void WriteIDSet(const gzFile& f, const set<__int64>& IDSet)
{
	int len = IDSet.size();
	gzwrite(f, &len, sizeof(int)); // 长度
	for (auto& i : IDSet)
		gzwrite(f, &i, sizeof(__int64)); // ID
}

static inline void WriteTextSet(const gzFile& f, const set<CString>& TextSet)
{
	int len = TextSet.size();
	gzwrite(f, &len, sizeof(int)); // 长度
	for (auto& i : TextSet)
		WriteText(f, i); // 文本
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
		WriteText(f, g_cookie);
		gzclose(f);
	}

	// 保存历史回复、忽略ID等
	f = gzopen_w(CACHE_PATH, "wb");
	if (f != NULL)
	{
		int len;
		// 历史回复
		gzwrite(f, &(len = g_reply.size()), sizeof(int)); // 长度
		for (auto& i : g_reply)
		{
			gzwrite(f, &i.first, sizeof(__int64)); // 主题ID
			gzwrite(f, &i.second, sizeof(int)); // 回复数
		}
		// 忽略ID
		WriteIDSet(f, g_initIgnoredTID);
		WriteIDSet(f, g_initIgnoredPID);
		WriteIDSet(f, g_initIgnoredLZLID);
		// 违规次数
		gzwrite(f, &(len = g_userTrigCount.size()), sizeof(int)); // 长度
		for (auto& i : g_userTrigCount)
		{
			WriteText(f, i.first); // 用户名
			gzwrite(f, &i.second, sizeof(int)); // 违规次数
		}
		// 拉黑用户
		WriteTextSet(f, g_defriendedUser);
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
		ReadText(f, g_cookie);
		gzclose(f);
	}

	// 历史回复、忽略ID等
	f = gzopen_w(CACHE_PATH, "rb");
	if (f != NULL)
	{
		int size;
		// 历史回复
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
		// 忽略ID
		ReadIDSet(f, g_initIgnoredTID);
		g_ignoredTID = g_initIgnoredTID;
		ReadIDSet(f, g_initIgnoredPID);
		g_ignoredPID = g_initIgnoredPID;
		ReadIDSet(f, g_initIgnoredLZLID);
		g_ignoredLZLID = g_initIgnoredLZLID;
		// 违规次数
		if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 100000) // 长度
		{
			CString userName;
			int count;
			for (int i = 0; i < size; i++)
			{
				ReadText(f, userName);
				gzread(f, &count, sizeof(int));
				g_userTrigCount[userName] = count;
			}
		}
		// 拉黑用户
		ReadTextSet(f, g_defriendedUser);
		gzclose(f);
	}
}
