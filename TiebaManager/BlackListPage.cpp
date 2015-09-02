// BlackListPage.cpp : 实现文件
//

#include "stdafx.h"
#include "BlackListPage.h"
#include "Global.h"
#include "SettingDlg.h"


// CBlackListPage 对话框

IMPLEMENT_DYNAMIC(CBlackListPage, CListPage)

CBlackListPage::CBlackListPage(CWnd* pParent /*=NULL*/)
	: CListPage(pParent)
{

}

CBlackListPage::~CBlackListPage()
{
}

void CBlackListPage::DoDataExchange(CDataExchange* pDX)
{
	CListPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBlackListPage, CListPage)
	ON_EN_KILLFOCUS(IDC_EDIT1, &CBlackListPage::OnKillfocusEdit1)
END_MESSAGE_MAP()


// CBlackListPage 消息处理程序


BOOL CBlackListPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_static.SetWindowText(_T("当被测文本等于文本时匹配\r\n\
注意正则表达式.*匹配所有文本，请不要不加限制使用！！"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

BOOL CBlackListPage::CheckMatchTooMuch(const CString& text, BOOL isRegex)
{
	return StringMatchs(MATCH_TOO_MUCH_USERNAME_TEST1, text, isRegex) 
		&& StringMatchs(MATCH_TOO_MUCH_USERNAME_TEST2, text, isRegex);
}

void CBlackListPage::PostChangeList()
{
	((CSettingDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

BOOL CBlackListPage::TestMatch(const CString& test, const CString& text, BOOL isRegex)
{
	return StringMatchs(test, text, isRegex);
}

// 去掉复制用户名时带的空格
void CBlackListPage::OnKillfocusEdit1()
{
	CString text;
	m_edit.GetWindowText(text);
	text.Replace(_T(" "), _T(""));
	m_edit.SetWindowText(text);
}
