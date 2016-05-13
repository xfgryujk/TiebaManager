#pragma once
#include "afxwin.h"
#include "NormalDlg.h"


// CUsersPage 对话框

class CUsersPage : public CNormalDlg
{
	DECLARE_DYNAMIC(CUsersPage)

public:
	CUsersPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUsersPage();

// 对话框数据
	enum { IDD = IDD_USERS_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();


public:
	CStatic m_currentUserStatic;
	CListBox m_list;
	CButton m_loginButton;
	CButton m_deleteButton;
	CButton m_switchButton;
};
