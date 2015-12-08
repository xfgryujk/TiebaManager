#pragma once
#include "afxwin.h"
#include "NormalDlg.h"


// CScanPage 对话框

class CScanPage : public CNormalDlg
{
	DECLARE_DYNAMIC(CScanPage)

public:
	CScanPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CScanPage();

// 对话框数据
	enum { IDD = IDD_SCAN_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnKillfocusEdit1();
	afx_msg void OnEnKillfocusEdit5();
	afx_msg void OnEnChangeEdit5();
	afx_msg void OnEnKillfocusEdit2();


public:
	CEdit m_scanIntervalEdit;
	CButton m_onlyScanTitleCheck;
	CEdit m_scanPageCountEdit;
	CButton m_briefLogCheck;
	CEdit m_threadCountEdit;
	CButton m_autoSaveLogCheck;
};
