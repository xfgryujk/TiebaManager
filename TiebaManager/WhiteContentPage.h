#pragma once
#include "RegListPage.h"


// CWhiteContentPage 对话框

class CWhiteContentPage : public CRegListPage
{
	DECLARE_DYNAMIC(CWhiteContentPage)

public:
	CWhiteContentPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWhiteContentPage();

// 对话框数据
	enum { IDD = IDD_REG_LIST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	virtual BOOL CheckMatchTooMuch(const CString& text, BOOL isRegex);
	virtual BOOL TestMatch(const CString& test, const CString& text, BOOL isRegex);
};
