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
