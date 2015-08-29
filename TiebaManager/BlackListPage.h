#pragma once
#include "ListPage.h"


// CBlackListPage 对话框

class CBlackListPage : public CListPage
{
	DECLARE_DYNAMIC(CBlackListPage)

public:
	CBlackListPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBlackListPage();

// 对话框数据
	enum { IDD = IDD_LIST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusEdit1();

protected:
	virtual BOOL CheckMatchTooMuch(const CString& text, BOOL isRegex);
	virtual void PostChangeList();
	virtual BOOL TestMatch(const CString& test, const CString& text, BOOL isRegex);
};
