#pragma once
#include <vector>
using std::vector;
#include <regex>
using std::wregex;


// 配置文件路径
extern CString	PROFILE_PATH;
const CString	OPTIONS_PATH = _T("Option\\");
const TCHAR		COOKIE_PATH[] = _T("ck.tb");
const TCHAR		REPLY_PATH[] = _T("reply.tb");

struct RegexText
{
	BOOL isRegex;
	CString text;
	wregex regexp;
};


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
extern vector<RegexText>	g_keywords;		// 违规内容
extern vector<RegexText>	g_blackList;	// 屏蔽用户
extern vector<CString>		g_whiteList;	// 信任用户
extern vector<RegexText>	g_whiteContent;	// 信任内容

extern CCriticalSection g_optionsLock; // 判断违规用的临界区


void ReadOptions(LPCTSTR path);
void WriteOptions(LPCTSTR path);
