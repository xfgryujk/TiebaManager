#pragma once
#include <set>
using std::set;
#include <map>
using std::map;
class CTiebaManagerDlg;
struct IHTMLDocument2;


extern volatile BOOL g_stopScanFlag;
extern CWinThread* g_scanThread;
extern set<__int64> g_initIgnoredTID; // 不删的主题ID(手动忽略)，要写入文件
extern set<__int64> g_initIgnoredPID; // 不删的帖子ID(手动忽略)，要写入文件
extern set<__int64> g_initIgnoredLZLID; // 不删的楼中楼ID(手动忽略)，要写入文件
extern set<__int64> g_ignoredTID; // 不删的主题ID(已扫描且违规)
extern set<__int64> g_ignoredPID; // 不删的帖子ID(已扫描且违规)
extern set<__int64> g_ignoredLZLID; // 不删的楼中楼ID(已扫描且违规)
extern set<__int64> g_deletedTID; // 已删的主题ID
extern map<__int64, int> g_reply; // 主题的回复数，要写入文件
extern map<CString, int> g_userTrigCount; // 某用户违规次数，要写入文件
extern set<CString> g_bannedUser; // 已封的用户
extern set<CString> g_defriendedUser; // 已拉黑的用户，要写入文件


BOOL CheckIllegal(LPCTSTR content, LPCTSTR author, CString& msg, int& pos, int& length);
UINT AFX_CDECL ScanThread(LPVOID mainDlg);
UINT AFX_CDECL ScanPostThread(LPVOID threadID);
BOOL ScanPostPage(const CString& tid, int page, const CString& title, BOOL hasHistoryReply,
	int ScanedCount, const CString& src, int threadID, CTiebaManagerDlg* dlg, CComPtr<IHTMLDocument2>* pDocument);
