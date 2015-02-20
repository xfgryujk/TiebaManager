#pragma once
#include "ListPage.h"


// CKeywordsPage 对话框

class CKeywordsPage : public CListPage
{
	DECLARE_DYNAMIC(CKeywordsPage)

public:
	CKeywordsPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CKeywordsPage();

// 对话框数据
	enum { IDD = IDD_LIST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	virtual BOOL CheckMatchTooMuch(const CString& text, BOOL isRegex);
	virtual void PostChangeList();
	virtual BOOL TestMatch(const CString& test, const CString& text, BOOL isRegex);
};
