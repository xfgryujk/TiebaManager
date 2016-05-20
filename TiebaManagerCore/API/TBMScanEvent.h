/*
Copyright (C) 2015  xfgryujk
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
