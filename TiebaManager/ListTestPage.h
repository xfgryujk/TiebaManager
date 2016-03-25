#pragma once
#include "afxwin.h"
#include "ListPage.h"


// CListTestPage 对话框

class CListTestPage : public CListPage
{
	DECLARE_DYNAMIC(CListTestPage)

public:
	CListTestPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CListTestPage();

// 对话框数据
	enum { IDD = IDD_LIST_TEST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedButton7();

protected:
	// 测试匹配
	virtual BOOL TestMatch(int index) = 0;


public:
	CEdit m_testEdit;
	CButton m_testButton;
};

// 用不相关的两个内容做测试
static const TCHAR MATCH_TOO_MUCH_CONTENT_TEST1[] = _T("【吧务导航】删帖查询，帖子申精，吧规，刷豆，刷粉");
static const TCHAR MATCH_TOO_MUCH_CONTENT_TEST2[] = _T("贺岁·番外14·叫兽教你烤地瓜");
static const TCHAR MATCH_TOO_MUCH_USERNAME_TEST1[] = _T("盗我原号的没J8");
static const TCHAR MATCH_TOO_MUCH_USERNAME_TEST2[] = _T("从容人生");
