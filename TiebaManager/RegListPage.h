#pragma once
#include "afxwin.h"
#include "NormalDlg.h"


// CRegListPage 对话框

class CRegListPage : public CNormalDlg
{
	DECLARE_DYNAMIC(CRegListPage)

public:
	CRegListPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRegListPage();

// 对话框数据
	enum { IDD = IDD_REG_LIST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDblclkList1();
	afx_msg void OnClickedButton1();
	afx_msg void OnClickedButton2();
	afx_msg void OnClickedButton3();
	afx_msg void OnClickedButton6();

protected:
	// 检查是否匹配太多
	virtual BOOL CheckMatchTooMuch(const CString& text, BOOL isRegex){ return FALSE; };
	// 添加或修改后调用
	virtual void PostChangeList(){}
	// 测试匹配
	virtual BOOL TestMatch(const CString& test, const CString& text, BOOL isRegex) = 0;


public:
	CListBox m_list;
	CEdit m_edit;
	CButton m_regexCheck;
	CButton m_addButton;
	CButton m_deleteButton;
	CButton m_changeButton;
	CEdit m_testEdit;
	CStatic m_static;
	CButton m_testButton;
protected:
	COLORREF m_staticColor;
};

static const TCHAR IS_REGEX_PREFIX[] = _T("正|");
static const TCHAR NOT_REGEX_PREFIX[] = _T("　|");
static const int REGEX_PREFIX_LENGTH = _countof(IS_REGEX_PREFIX) - 1;

// 用不相关的两个内容做测试
static const TCHAR MATCH_TOO_MUCH_CONTENT_TEST1[] = _T("【吧务导航】删帖查询，帖子申精，吧规，刷豆，刷粉");
static const TCHAR MATCH_TOO_MUCH_CONTENT_TEST2[] = _T("贺岁·番外14·叫兽教你烤地瓜");
static const TCHAR MATCH_TOO_MUCH_USERNAME_TEST1[] = _T("盗我原号的没J8");
static const TCHAR MATCH_TOO_MUCH_USERNAME_TEST2[] = _T("从容人生");
