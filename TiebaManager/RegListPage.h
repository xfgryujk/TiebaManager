#pragma once
#include "ListTestPage.h"
#include "TypeHelper.h"


// CRegListPage 对话框

class CRegListPage : public CListTestPage
{
	DECLARE_DYNAMIC(CRegListPage)

public:
	CRegListPage(const CString& inputTitle, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRegListPage();

	// 对话框数据
	enum { IDD = IDD_LIST_TEST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	virtual BOOL SetItem(int index);
	virtual BOOL Export(const CString& path);
	virtual BOOL Import(const CString& path);
	virtual void ShowList(const vector<RegexText>& list);
	virtual void ApplyList(vector<RegexText>& list);
protected:
	virtual BOOL CheckMatchTooMuch(const CString& text, BOOL isRegex){ return FALSE; };
	virtual BOOL TestMatch(int index);
	virtual BOOL TestMatch(const CString& test, const CString& text, BOOL isRegex) = 0;


protected:
	const CString m_inputTitle;
};

static const TCHAR IS_REGEX_TEXT[] = _T("√");
