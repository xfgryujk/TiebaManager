#include "stdafx.h"
#include "TiebaVariable.h"
#include "TiebaOperate.h"


// 贴吧、用户信息
CUserTiebaInfo g_userTiebaInfo;

// 扫描
volatile BOOL g_stopScanFlag = FALSE;
CWinThread* g_scanThread = NULL;
CUserCache g_userCache;

// 操作
CWinThread* g_confirmThread = NULL;
CWinThread* g_operateThread = NULL;


BOOL CUserTiebaInfo::LoadOld(const CString& path)
{
	gzFile f = gzopen_w(path, "rb");
	if (f == NULL)
		return FALSE;
	
	ReadText(f, m_cookie);
	gzclose(f);
	return TRUE;
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

BOOL CUserCache::LoadOld(const CString& path)
{
	gzFile f = gzopen_w(path, "rb");
	if (f == NULL)
		return FALSE;

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
			g_userCache.m_reply.m_value[tid] = reply;
		}
	}
	// 忽略ID
	ReadIDSet(f, g_userCache.m_initIgnoredTID);
	g_userCache.m_ignoredTID = g_userCache.m_initIgnoredTID;
	ReadIDSet(f, g_userCache.m_initIgnoredPID);
	g_userCache.m_ignoredPID = g_userCache.m_initIgnoredPID;
	ReadIDSet(f, g_userCache.m_initIgnoredLZLID);
	g_userCache.m_ignoredLZLID = g_userCache.m_initIgnoredLZLID;
	// 违规次数
	if (gzread(f, &size, sizeof(int)) == sizeof(int) && 0 < size && size < 100000) // 长度
	{
		CString userName;
		int count;
		for (int i = 0; i < size; i++)
		{
			ReadText(f, userName);
			gzread(f, &count, sizeof(int));
			g_userCache.m_userTrigCount.m_value[userName] = count;
		}
	}
	// 拉黑用户
	ReadTextSet(f, g_userCache.m_defriendedUser);
	gzclose(f);
	return TRUE;
}
