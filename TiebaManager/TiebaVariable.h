#pragma once
#include "Setting.h"


// 贴吧、用户信息
class CUserTiebaInfo : public CConfigBase
{
public:
	CString m_forumName;
	CString m_encodedForumName;
	CString m_forumID;
	COption<CString> m_cookie;
	CString m_tbs; // 备用获取tbs地址：http://tieba.baidu.com/dc/common/tbs

	CUserTiebaInfo()
		: CConfigBase("Cookie"),
		m_cookie("Cookie")
	{
		m_options.push_back(&m_cookie);
	}

	BOOL LoadOld(const CString& path)
	{
		gzFile f = gzopen_w(path, "rb");
		if (f == NULL)
			return FALSE;

		ReadText(f, m_cookie);
		gzclose(f);
		return TRUE;
	}
};
extern CUserTiebaInfo g_userTiebaInfo;

// 扫描
extern volatile BOOL g_stopScanFlag;
extern CWinThread* g_scanThread;
// 缓存
class CUserCache : public CConfigBase
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
	COption<map<CString, int> > m_userTrigCount;	// 某用户违规次数，要写入文件
	COption<set<CString> > m_bannedUser;			// 已封的用户，要写入文件
	COption<set<CString> > m_defriendedUser;		// 已拉黑的用户，要写入文件

	CUserCache()
		: CConfigBase("Cache"),
		m_initIgnoredTID("IgnoredTID"),
		m_initIgnoredPID("IgnoredPID"),
		m_initIgnoredLZLID("IgnoredLZLID"),
		m_reply("Reply"),
		m_userTrigCount("TrigCount"),
		m_bannedUser("BannedUser"),
		m_defriendedUser("DefriendedUser")
	{
		m_options.push_back(&m_initIgnoredTID);
		m_options.push_back(&m_initIgnoredPID);
		m_options.push_back(&m_initIgnoredLZLID);
		m_options.push_back(&m_reply);
		m_options.push_back(&m_userTrigCount);
		m_options.push_back(&m_bannedUser);
		m_options.push_back(&m_defriendedUser);
	}

	void PostChange()
	{
		m_ignoredTID = m_initIgnoredTID;
		m_ignoredPID = m_initIgnoredPID;
		m_ignoredLZLID = m_initIgnoredLZLID;
		m_deletedTID.clear();
	}

	BOOL LoadOld(const CString& path);
};
extern CUserCache g_userCache;

// 操作
extern CWinThread* g_confirmThread;
extern CWinThread* g_operateThread;
