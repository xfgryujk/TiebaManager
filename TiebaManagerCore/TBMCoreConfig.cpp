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

#include "stdafx.h"
#include <TBMCoreConfig.h>


CTBMCoreConfig::CTBMCoreConfig(CStringA name) : CConfigBase(name),
	m_scanInterval		("ScanInterval",		5,		InRange<int, 0, 600>),
	m_onlyScanTitle		("OnlyScanTitle",		FALSE),
	m_scanPageCount		("ScanPageCount",		1,		GreaterThan<int, 1>),
	m_briefLog			("BriefLog",			FALSE),
	m_threadCount		("ThreadCount",			2,		InRange<int, 1, 16>),

	m_delete			("Delete",				TRUE),
	m_banID				("BanID",				FALSE),
	m_defriend			("Defriend",			FALSE),
	m_deleteInterval	("DeleteInterval",		2.5f,	[](const float& value){ return 0.0f <= value && value <= 60.0f; }/*InRange<float, 0.0f, 60.0f>*/),
	m_banDuration		("BanDuration",			1,		[](const int& value){ return value == 1 || value == 3 || value == 10; }),
	m_banReason			("BanReason",			_T("")),
	m_banTrigCount		("BanTrigCount",		1,		GreaterThan<int, 1>),
	m_defriendTrigCount	("DefriendTrigCount",	5,		GreaterThan<int, 1>),
	m_confirm			("Confirm",				TRUE),
	m_wapBanInterface	("WapBanInterface",		FALSE)
{
	m_options.push_back(&m_scanInterval);
	m_options.push_back(&m_onlyScanTitle);
	m_options.push_back(&m_scanPageCount);
	m_options.push_back(&m_briefLog);
	m_options.push_back(&m_threadCount);

	m_options.push_back(&m_delete);
	m_options.push_back(&m_banID);
	m_options.push_back(&m_defriend);
	m_options.push_back(&m_deleteInterval);
	m_options.push_back(&m_banDuration);
	m_options.push_back(&m_banReason);
	m_options.push_back(&m_banTrigCount);
	m_options.push_back(&m_defriendTrigCount);
	m_options.push_back(&m_confirm);
	m_options.push_back(&m_wapBanInterface);
}

CUserCache::CUserCache() : CConfigBase("Cache"),
	m_initIgnoredTID("IgnoredTID"),
	m_initIgnoredPID("IgnoredPID"),
	m_initIgnoredLZLID("IgnoredLZLID"),
	m_reply("Reply"),
	m_lastAuthor("LastAuthor"),
	m_userTrigCount("TrigCount"),
	m_bannedUser("BannedUser"),
	m_defriendedUser("DefriendedUser")
{
	m_options.push_back(&m_initIgnoredTID);
	m_options.push_back(&m_initIgnoredPID);
	m_options.push_back(&m_initIgnoredLZLID);
	m_options.push_back(&m_reply);
	m_options.push_back(&m_lastAuthor);
	m_options.push_back(&m_userTrigCount);
	m_options.push_back(&m_bannedUser);
	m_options.push_back(&m_defriendedUser);
}

void CUserCache::PostChange()
{
	m_ignoredTID = m_initIgnoredTID;
	m_ignoredPID = m_initIgnoredPID;
	m_ignoredLZLID = m_initIgnoredLZLID;
	m_deletedTID.clear();
}
