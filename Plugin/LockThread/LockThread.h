#pragma once
class CEventBase;
class CLockThreadDlg;


class CLockThread
{
public:
	bool Init();
	bool Uninit();
	void OnConfig();

	void StartLockThread();
	void StopLockThread();
	void LockThreadThread();


	CLockThreadDlg* m_lockThreadDlg = NULL;

	unique_ptr<thread> m_lockThreadThread;
	volatile BOOL m_stopFlag = TRUE;
};
extern CLockThread g_lockThread;
