#pragma once
#include "NormalDlg.h"
#include "afxwin.h"


// CDefriendPage 对话框

class CDefriendPage : public CNormalDlg
{
	DECLARE_DYNAMIC(CDefriendPage)

public:
	CDefriendPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDefriendPage();

// 对话框数据
	enum { IDD = IDD_DEFRIEND_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
protected:
	static UINT AFX_CDECL DefriendThread(LPVOID _thiz);
	static void DoDefriend(int startPage, int endPage);


protected:
	static CDefriendPage* s_instance;
	static CCriticalSection s_instanceLock;
	static volatile BOOL s_stopFlag;
	static CString s_startPage;
	static CString s_endPage;
	static BOOL s_defriendNewUsers;

public:
	CEdit m_startPageEdit;
	CEdit m_endPageEdit;
	CButton m_startButton;
	CButton m_stopButton;
	CButton m_defriendNewUsersCheck;
	CStatic m_stateStatic;
};
