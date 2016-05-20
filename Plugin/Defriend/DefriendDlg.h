#pragma once
#include <NormalDlg.h>
#include "afxwin.h"


// CDefriendDlg 对话框

class CDefriendDlg : public CNormalDlg
{
	DECLARE_DYNAMIC(CDefriendDlg)

public:
	CDefriendDlg(CDefriendDlg*& pThis, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDefriendDlg();

// 对话框数据
	enum { IDD = IDD_DEFRIEND_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	afx_msg void OnClose();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();


public:
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

protected:
	CDefriendDlg*& m_pThis;
};
