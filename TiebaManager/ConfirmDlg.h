#pragma once
#include "afxwin.h"
#include "Tieba.h"


// CConfirmDlg 对话框

class CConfirmDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfirmDlg)

public:
	CConfirmDlg(CWnd* pParent = NULL);   // 标准构造函数
	CConfirmDlg(Operation* operation, CWnd* pParent = NULL);   // 指定内容的构造函数
	virtual ~CConfirmDlg();

// 对话框数据
	enum { IDD = IDD_CONFIRM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();


public:
	CEdit m_contentEdit;
	CStatic m_static;
	CButton m_yesButton;
	CButton m_noButton;

protected:
	Operation* m_operation;
public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
