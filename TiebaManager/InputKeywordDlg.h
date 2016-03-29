#pragma once
#include "afxwin.h"
#include "InputDlg.h"


// CInputKeywordDlg 对话框

class CInputKeywordDlg : public CInputDlg
{
	DECLARE_DYNAMIC(CInputKeywordDlg)

public:
	CInputKeywordDlg(const CString& title, CString& content, BOOL* isRegex = NULL, BOOL* forceToConfirm = NULL, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInputKeywordDlg();

	// 对话框数据
	enum { IDD = IDD_INPUT_KEYWORD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();


public:
	CButton m_forceToConfirmCheck;

	BOOL* m_forceToConfirm;
};
