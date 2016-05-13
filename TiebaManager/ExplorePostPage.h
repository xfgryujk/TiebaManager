#pragma once
#include "ExplorerPage.h"
struct PostInfo;


// CExploreThreadPage 对话框

class CExplorePostPage : public CExplorerPage
{
	DECLARE_DYNAMIC(CExplorePostPage)

public:
	CExplorePostPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExplorePostPage();

// 对话框数据
	enum { IDD = IDD_EXPLORER_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void OnBnClickedButton1();
	afx_msg void OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);


public:
	CString m_tid;
	vector<PostInfo> m_posts;
};
