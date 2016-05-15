#pragma once
#include "TiebaManagerCoreCommon.h"
#include <ConfigFile.h>


class TIEBA_MANAGER_CORE_API CTBMCoreConfig : public CConfigBase
{
public:
	COption<int>		m_scanInterval;			// 扫描间隔
	COption<BOOL>		m_onlyScanTitle;		// 只扫描标题
	COption<int>		m_scanPageCount;		// 扫描最后页数
	COption<BOOL>		m_briefLog;				// 只输出删帖封号
	COption<int>		m_threadCount;			// 线程数

	COption<BOOL>		m_delete;				// 删帖
	COption<BOOL>		m_banID;				// 封ID
	COption<BOOL>		m_defriend;				// 拉黑
	COption<float>		m_deleteInterval;		// 删帖间隔
	COption<int>		m_banDuration;			// 封禁时长
	COption<CString>	m_banReason;			// 封号原因
	COption<int>		m_banTrigCount;			// 封禁违规次数
	COption<int>		m_defriendTrigCount;	// 拉黑违规次数
	COption<BOOL>		m_confirm;				// 操作前提示
	COption<BOOL>		m_wapBanInterface;		// 封禁用WAP接口

	CTBMCoreConfig(CStringA name);
};

class TIEBA_MANAGER_CORE_API CUserCache : public CConfigBase
{
public:
	COption<set<__int64> > m_initIgnoredTID;		// 不删的主题ID(手动忽略)，要写入文件
	COption<set<__int64> > m_initIgnoredPID;		// 不删的帖子ID(手动忽略)，要写入文件
	COption<set<__int64> > m_initIgnoredLZLID;		// 不删的楼中楼ID(手动忽略)，要写入文件
	set<__int64> m_ignoredTID;						// 不删的主题ID(已扫描且违规)
	set<__int64> m_ignoredPID;						// 不删的帖子ID(已扫描且违规)
	set<__int64> m_ignoredLZLID;					// 不删的楼中楼ID(已扫描且违规)
	set<__int64> m_deletedTID;						// 已删的主题ID
	COption<map<__int64, int> > m_reply;			// 主题的回复数，要写入文件
	COption<map<__int64, CString> > m_lastAuthor;	// 主题的最后回复作者，要写入文件
	COption<map<CString, int> > m_userTrigCount;	// 某用户违规次数，要写入文件
	COption<set<CString> > m_bannedUser;			// 已封的用户，要写入文件
	COption<set<CString> > m_defriendedUser;		// 已拉黑的用户，要写入文件

	CUserCache();

	void PostChange();
};

class ILog
{
public:
	virtual ~ILog() = default;

	virtual void Log(const CString& content) = 0;
};
