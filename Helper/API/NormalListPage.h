#pragma once
#include "HelperCommon.h"
#include "ListPage.h"


// CNormalListPage 对话框

class HELPER_API CNormalListPage : public CListPage
{
	DECLARE_DYNAMIC(CNormalListPage)

public:
	CNormalListPage(const CString& inputTitle, CWnd* pParent = NULL);   // 标准构造函数
	CNormalListPage(const CString& inputTitle, UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CNormalListPage();

	// 对话框数据

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	virtual BOOL SetItem(int index);
	virtual BOOL Export(const CString& path);
	virtual BOOL Import(const CString& path);
	virtual void ShowList(const vector<CString>& list);
	virtual void ShowList(const set<CString>& list);
	virtual void ApplyList(vector<CString>& list);
	virtual void ApplyList(set<CString>& list);


protected:
	const CString m_inputTitle;
};
