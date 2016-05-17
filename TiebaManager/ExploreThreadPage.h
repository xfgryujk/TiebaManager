#pragma once
#include "ExplorerPage.h"
struct ThreadInfo;


// CExploreThreadPage 对话框

class CExploreThreadPage : public CExplorerPage
{
	DECLARE_DYNAMIC(CExploreThreadPage)

public:
	CExploreThreadPage(const CString& forumName, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExploreThreadPage();

// 对话框数据
	enum { IDD = IDD_EXPLORER_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void OnBnClickedButton1();
	afx_msg void OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);


public:
	vector<ThreadInfo> m_threads;

protected:
	CString m_forumName;
};
