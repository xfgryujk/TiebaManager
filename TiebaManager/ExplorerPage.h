#pragma once
#include "afxwin.h"


// CExplorerPage 对话框

class CExplorerPage : public CDialog
{
	DECLARE_DYNAMIC(CExplorerPage)

public:
	CExplorerPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExplorerPage();

// 对话框数据
	enum { IDD = IDD_EXPLORER_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog() = 0;


public:
	CStatic m_static;
	CEdit m_edit;
	CButton m_gotoButton;
	CListCtrl m_list;
};
