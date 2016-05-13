#pragma once
#include "ExplorerPage.h"
struct PostInfo;


// CExploreThreadPage 对话框

class CExploreLzlPage : public CExplorerPage
{
	DECLARE_DYNAMIC(CExploreLzlPage)

public:
	CExploreLzlPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExploreLzlPage();

// 对话框数据
	enum { IDD = IDD_EXPLORER_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);


public:
	vector<PostInfo> m_lzls;
};
