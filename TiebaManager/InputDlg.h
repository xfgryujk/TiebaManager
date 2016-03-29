#pragma once
#include "afxwin.h"


// CInputDlg 对话框

class CInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputDlg)

public:
	CInputDlg(const CString& title, CString& content, BOOL* isRegex = NULL, BOOL showRegexCheck = FALSE, UINT nIDTemplate = IDD, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInputDlg();

// 对话框数据
	enum { IDD = IDD_INPUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();


public:
	CEdit m_edit;
	CButton m_regexCheck;

	CString& m_content;
	BOOL* m_isRegex;

protected:
	const CString m_title;
	const BOOL m_showRegexCheck;
};
