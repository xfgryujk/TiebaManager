#pragma once
#include <vector>
using std::vector;
#include <regex>
using std::wregex;
#include <zlib.h>
struct ImageFeature;



// 配置文件路径
extern CString	ALL_PROFILE_PATH;	// 程序运行时初始化
extern CString	USER_PROFILE_PATH;	// 确定贴吧时初始化
extern CString	OPTIONS_PATH;
extern CString	USERS_PATH;			// 确定贴吧时初始化
extern CString	CURRENT_USER_PATH;	// 确定贴吧时初始化
extern CString	COOKIE_PATH;		// 确定贴吧时初始化
extern CString	CACHE_PATH;			// 确定贴吧时初始化

struct RegexText
{
	BOOL isRegex;
	CString text;
	wregex regexp;
};

extern CString	g_currentUser;		// 当前账号
extern BOOL		g_autoUpdate;		// 自动更新

// 方案
extern CString	g_currentOption;	// 当前方案
extern int		g_scanInterval;		// 扫描间隔
extern BOOL		g_banID;			// 封ID
extern int		g_banDuration;		// 封禁时长
extern int		g_trigCount;		// 封禁违规次数
extern BOOL		g_onlyScanTitle;	// 只扫描标题
extern float	g_deleteInterval;	// 删帖间隔
extern BOOL		g_confirm;			// 操作前提示
extern int		g_scanPageCount;	// 扫描最后页数
extern BOOL		g_briefLog;			// 只输出删帖封号
extern BOOL		g_delete;			// 删帖
extern int		g_threadCount;		// 线程数
extern CString	g_banReason;		// 封号原因
extern CString g_imageDir;			// 违规图片目录
extern vector<RegexText>	g_keywords;		// 违规内容
extern vector<RegexText>	g_blackList;	// 屏蔽用户
extern vector<CString>		g_whiteList;	// 信任用户
extern vector<RegexText>	g_whiteContent;	// 信任内容
extern vector<ImageFeature>	g_imageFeatures; // 图片特征

extern CCriticalSection g_optionsLock; // 判断违规用的临界区


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
void SetCurrentUser(LPCTSTR userName);
