#pragma once
class CEventBase;
class CDefriendDlg;


class CDefriend
{
public:
	bool Init();
	bool Uninit();
	void OnConfig();


	CDefriendDlg* m_defriendDlg = NULL;
};
extern CDefriend g_defriend;
