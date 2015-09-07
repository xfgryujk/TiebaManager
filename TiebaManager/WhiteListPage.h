#pragma once
#include "ListPage.h"


// CWhiteListTestPage 对话框

class CWhiteListPage : public CListPage
{
	DECLARE_DYNAMIC(CWhiteListPage)

public:
	CWhiteListPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWhiteListPage();

// 对话框数据
	enum { IDD = IDD_LIST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
