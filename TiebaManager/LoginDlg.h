#pragma once
#include "afxwin.h"


// CLoginDlg 对话框

class CLoginDlg : public CDialog
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
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnStnClickedStatic4();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);


public:
	CEdit m_userNameEdit;
	CEdit m_passwordEdit;
	CEdit m_verifyCodeEdit;
	CStatic m_verifyCodePicture;
protected:
	CString m_token;
	CString m_verifyStr;
	CImage m_verifyImage;
};
