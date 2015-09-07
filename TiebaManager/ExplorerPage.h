#pragma once
#include "afxwin.h"
#include "NormalDlg.h"


// CExplorerPage 对话框

class CExplorerPage : public CNormalDlg
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
public:
	virtual BOOL OnInitDialog();


public:
	CStatic m_static;
	CEdit m_edit;
	CButton m_gotoButton;
	CListCtrl m_list;
};
