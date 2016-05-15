#pragma once
class CTBMScan;
class CEventBase;


class CTBMScanListeners
{
protected:
	CTBMScan& m_scan;

public:
	CTBMScanListeners(CTBMScan& scan);

	void OnCheckThreadIllegal(CEventBase* event__);
	void OnCheckPostIllegal(CEventBase* event__);
	void OnCheckLzlIllegal(CEventBase* event__);
	void OnCheckThreadImageIllegal(CEventBase* event__);
	void OnCheckPostImageIllegal(CEventBase* event__);
	void OnCheckLzlImageIllegal(CEventBase* event__);

	void OnScanThreadStart(CEventBase* event__);
	void OnScanThreadEnd(CEventBase* event__);
	void OnScanOnceStart(CEventBase* event__);
	void OnScanOnceEnd(CEventBase* event__);
	void OnPreScanAllThreads(CEventBase* event__);
	
	void OnScanPostThreadEnd(CEventBase* event__);
	void OnPreScanThread(CEventBase* event__);

	void OnScanPostPage(CEventBase* event__);

protected:
	CCriticalSection m_stateListLock;
};
