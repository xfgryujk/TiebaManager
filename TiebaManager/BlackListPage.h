#pragma once
#include "RegListPage.h"


// CBlackListTestPage 对话框

class CBlackListPage : public CRegListPage
{
	DECLARE_DYNAMIC(CBlackListPage)

public:
	CBlackListPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBlackListPage();

// 对话框数据
	enum { IDD = IDD_LIST_TEST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	virtual BOOL CheckMatchTooMuch(const CString& text, BOOL isRegex);
	virtual void OnAdd(int index);
	virtual BOOL TestMatch(const CString& test, const CString& text, BOOL isRegex);
};
