#pragma once
#include "HelperCommon.h"
#include "afxwin.h"
#include "ListPage.h"


// CListTestPage 对话框

class HELPER_API CListTestPage : public CListPage
{
	DECLARE_DYNAMIC(CListTestPage)

public:
	CListTestPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CListTestPage();

// 对话框数据

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedButton7();

protected:
	// 测试匹配
	virtual BOOL TestMatch(int index) = 0;


public:
	CEdit m_testEdit;
	CButton m_testButton;
};
