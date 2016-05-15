#pragma once
#include "TiebaManagerCoreCommon.h"
#include <EventHelper.h>
#include <TiebaClawer.h>


enum TIEBA_MANAGER_CORE_API TBMScanEventID
{
	CheckThreadIllegalEvent,		// CCheckThreadIllegalEvent，使用result
	CheckThreadImageIllegalEvent,	// CCheckThreadIllegalEvent，使用result
	CheckPostIllegalEvent,			// CCheckPostIllegalEvent，使用result
	CheckPostImageIllegalEvent,		// CCheckPostIllegalEvent，使用result
	CheckLzlIllegalEvent,			// CCheckPostIllegalEvent，使用result
	CheckLzlImageIllegalEvent,		// CCheckPostIllegalEvent，使用result

	ScanThreadStartEvent,			// CEventBase，总扫描线程开始
	ScanThreadEndEvent,				// CEventBase，总扫描线程结束
	ScanOnceStartEvent,				// CEventBase，一次扫描开始
	ScanOnceEndEvent,				// CEventBase，一次扫描结束
	PreScanAllThreadsEvent,			// CEventBase，准备扫描所有主题

	ScanPostThreadStartEvent,		// CScanPostThreadEvent，扫描帖子线程开始
	ScanPostThreadEndEvent,			// CScanPostThreadEvent，扫描帖子线程结束
	PreScanThreadEvent,				// CPreScanThreadEvent，准备扫描某个主题

	ScanPostPageEvent				// CScanPostPageEvent，扫描帖子页开始
};

class TIEBA_MANAGER_CORE_API CCheckIllegalEventBase : public CEventBase
{
public:
	CString& m_msg;			// 日志里输出的信息
	BOOL& m_forceToConfirm; // 强制确认
	int& m_pos;				// 确认时高亮位置
	int& m_length;			// 确认时高亮长度

	CCheckIllegalEventBase(CString& msg, BOOL& forceToConfirm, int& pos, int& length);
};

class TIEBA_MANAGER_CORE_API CCheckThreadIllegalEvent : public CCheckIllegalEventBase
{
public:
	const ThreadInfo& m_thread;

	CCheckThreadIllegalEvent(const ThreadInfo& thread, CString& msg, BOOL& forceToConfirm, int& pos, int& length);
};

class TIEBA_MANAGER_CORE_API CCheckPostIllegalEvent : public CCheckIllegalEventBase
{
public:
	const PostInfo& m_post;

	CCheckPostIllegalEvent(const PostInfo& m_post, CString& msg, BOOL& forceToConfirm, int& pos, int& length);
};

class TIEBA_MANAGER_CORE_API CScanPostThreadEvent : public CEventBase
{
public:
	const int m_threadID; // 线程ID！！！

	CScanPostThreadEvent(int threadID);
};

class TIEBA_MANAGER_CORE_API CPreScanThreadEvent : public CScanPostThreadEvent
{
public:
	const ThreadInfo& m_thread;

	CPreScanThreadEvent(const ThreadInfo& thread, int threadID);
};

class TIEBA_MANAGER_CORE_API CScanPostPageEvent : public CPreScanThreadEvent
{
public:
	const int m_page;

	CScanPostPageEvent(const ThreadInfo& thread, int threadID, int page);
};
