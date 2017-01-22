/*
Copyright (C) 2011-2017  xfgryujk
http://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#pragma once
#include "TiebaManagerCoreCommon.h"
#include "ConfigFile.h"


class TBM_CORE_API CTBMCoreConfig : public CConfigBase
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

class TBM_CORE_API CUserCache : public CConfigBase
{
public:
	COption<std::set<__int64> > m_initIgnoredTID;		// 不删的主题ID(手动忽略)，要写入文件
	COption<std::set<__int64> > m_initIgnoredPID;		// 不删的帖子ID(手动忽略)，要写入文件
	COption<std::set<__int64> > m_initIgnoredLZLID;		// 不删的楼中楼ID(手动忽略)，要写入文件
	std::set<__int64> m_ignoredTID;						// 不删的主题ID(已扫描且违规)
	std::set<__int64> m_ignoredPID;						// 不删的帖子ID(已扫描且违规)
	std::set<__int64> m_ignoredLZLID;					// 不删的楼中楼ID(已扫描且违规)
	std::set<__int64> m_deletedTID;						// 已删的主题ID
	COption<std::map<__int64, int> > m_reply;			// 主题的回复数，要写入文件
	COption<std::map<__int64, CString> > m_lastAuthor;	// 主题的最后回复作者，要写入文件
	COption<std::map<CString, int> > m_userTrigCount;	// 某用户违规次数，要写入文件
	COption<std::set<CString> > m_bannedUser;			// 已封的用户，要写入文件
	COption<std::set<CString> > m_defriendedUser;		// 已拉黑的用户，要写入文件

	CUserCache();

	void PostChange();
};

class ILog
{
public:
	virtual ~ILog() = default;

	virtual void Log(const CString& content) = 0;
};
