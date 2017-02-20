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
#include "Singleton.h"
#include "TiebaClawer.h"
#include <memory>
#include <thread>
#include <mutex>


class TBM_CORE_API CTBMScan final : public Singleton<CTBMScan>
{
	DECL_SINGLETON(CTBMScan);
private:
	CTBMScan() = default;
	~CTBMScan();

public:
	// 开始扫描
	void StartScan(const CString& sPage);
	// 停止扫描，不等待线程结束
	void StopScan();
	// 正在扫描
	BOOL IsScanning();

protected:
	volatile BOOL m_stopScanFlag = FALSE;
	std::unique_ptr<std::thread> m_scanThread;

	std::vector<ThreadInfo> m_threads; // 当前扫描的主题列表
	int m_threadIndex; // 下个要扫描的主题索引
	std::mutex m_threadIndexLock;


	void ScanThreadImage();
	void ScanThread(CString sPage);
	void ScanPostThread(int threadID);
	BOOL ScanPostPage(const ThreadInfo& thread, int page, BOOL hasHistoryReply,
		int ScanedCount, const CString& src, int threadID);
};
