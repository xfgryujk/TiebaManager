#pragma once
#include "ListPage.h"


// CNormalListPage 对话框

class CNormalListPage : public CListPage
{
	DECLARE_DYNAMIC(CNormalListPage)

public:
	CNormalListPage(const CString& inputTitle, CWnd* pParent = NULL);   // 标准构造函数
	CNormalListPage(const CString& inputTitle, UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CNormalListPage();

	// 对话框数据
	enum { IDD = IDD_LIST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg virtual void OnClickedButton4();
	afx_msg virtual void OnClickedButton5();

	virtual BOOL SetItem(int index);
	virtual void ShowList(const vector<CString>& list);
	virtual void ShowList(const set<CString>& list);
	virtual void ApplyList(vector<CString>& list);
	virtual void ApplyList(set<CString>& list);


protected:
	const CString m_inputTitle;
};
