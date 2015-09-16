#pragma once
#include "afxwin.h"
#include "NormalDlg.h"


// COperatePage 对话框

class COperatePage : public CNormalDlg
{
	DECLARE_DYNAMIC(COperatePage)

public:
	COperatePage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COperatePage();

// 对话框数据
	enum { IDD = IDD_OPERATE_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnEnKillfocusEdit3();
	afx_msg void OnEnKillfocusEdit4();


public:
	CButton m_deleteCheck;
	CButton m_banIDCheck;
	CComboBox m_banDurationCombo;
	CEdit m_banReasonEdit;
	CEdit m_trigCountEdit;
	CEdit m_deleteIntervalEdit;
	CButton m_confirmCheck;
};
