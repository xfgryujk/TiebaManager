#pragma once
class CEventBase;
class CDefriendDlg;


class CDefriend
{
public:
	bool Init();
	bool Uninit();
	void OnConfig();

	void StartDefriend(const CString& startPage, const CString& endPage, BOOL defriendNewUsers);
	void StopDefriend();
	void DefriendThread(CString startPage, CString endPage, BOOL defriendNewUsers);
	void DoDefriend(int startPage, int endPage);


	CDefriendDlg* m_defriendDlg = NULL;

	unique_ptr<thread> m_defriendThread;
	volatile BOOL m_stopFlag = TRUE;
};
extern CDefriend g_defriend;
