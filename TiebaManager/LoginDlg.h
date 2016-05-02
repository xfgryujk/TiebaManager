#pragma once
#include "NormalDlg.h"
#include "explorer1.h"
#include "afxwin.h"


// CLoginDlg 对话框

class CLoginDlg : public CNormalDlg
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLoginDlg();

// 对话框数据
	enum { IDD = IDD_LOGIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCancel();
	DECLARE_EVENTSINK_MAP()
	void NavigateComplete2Explorer1(LPDISPATCH pDisp, VARIANT* URL);
protected:
	void GetLoginUserName();


public:
	CExplorer1 m_explorer;
	CButton m_useIECookieButton;
	CButton m_loginButton;
	CButton m_cancelButton;

	CString m_cookie;
	CString m_userName;
};
