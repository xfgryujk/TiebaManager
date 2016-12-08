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
class ILog;
class CTBMCoreConfig;
class CUserCache;
class CTBMOperate;
#include <TiebaClawer.h>


class TIEBA_MANAGER_CORE_API CTBMScan
{
public:
	// 可以注册事件监听，见TBMScanEvent.h
	CEventBus m_eventBus;

	// 记得依赖注入哦
	ILog* m_log = NULL;
	CTBMCoreConfig* m_config = NULL;
	CUserCache* m_userCache = NULL;
	CTBMOperate* m_operate = NULL;


	CTBMScan(CTBMCoreConfig* config = NULL, CUserCache* userCache = NULL, CTBMOperate* operate = NULL, ILog* log = NULL);
	virtual ~CTBMScan();

	// 开始扫描
	void StartScan(const CString& sPage);
	// 停止扫描，不等待线程结束
	void StopScan();
	// 正在扫描
	BOOL IsScanning();

protected:
	volatile BOOL m_stopScanFlag = FALSE;
	unique_ptr<thread> m_scanThread;

	vector<ThreadInfo> m_threads; // 当前扫描的主题列表
	int m_threadIndex; // 下个要扫描的主题索引
	mutex m_threadIndexLock;


	void ScanThreadImage();
	void ScanThread(CString sPage);
	void ScanPostThread(int threadID);
	BOOL ScanPostPage(const ThreadInfo& thread, int page, BOOL hasHistoryReply,
		int ScanedCount, const CString& src, int threadID);
};
