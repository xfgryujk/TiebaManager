#include "stdafx.h"
#include <TBMCoreConfig.h>


CTBMCoreConfig::CTBMCoreConfig(CStringA name) : CConfigBase(name),
	m_scanInterval		("ScanInterval",		5,		[](const int& value)->BOOL{ return 0 <= value && value <= 600; }),
	m_onlyScanTitle		("OnlyScanTitle",		FALSE),
	m_scanPageCount		("ScanPageCount",		1,		[](const int& value)->BOOL{ return 1 <= value; }),
	m_briefLog			("BriefLog",			FALSE),
	m_threadCount		("ThreadCount",			2,		[](const int& value)->BOOL{ return 1 <= value && value <= 16; }),

	m_delete			("Delete",				TRUE),
	m_banID				("BanID",				FALSE),
	m_defriend			("Defriend",			FALSE),
	m_deleteInterval	("DeleteInterval",		2.5f,	[](const float& value)->BOOL{ return 0.0f <= value && value <= 60.0f; }),
	m_banDuration		("BanDuration",			1,		[](const int& value)->BOOL{ return value == 1 || value == 3 || value == 10; }),
	m_banReason			("BanReason",			_T("")),
	m_banTrigCount		("BanTrigCount",		1,		[](const int& value)->BOOL{ return 1 <= value; }),
	m_defriendTrigCount	("DefriendTrigCount",	5,		[](const int& value)->BOOL{ return 1 <= value; }),
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
