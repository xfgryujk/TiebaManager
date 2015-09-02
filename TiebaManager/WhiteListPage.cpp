// WhiteListPage.cpp : 实现文件
//

#include "stdafx.h"
#include "WhiteListPage.h"
#include "Global.h"


// CWhiteListPage 对话框

IMPLEMENT_DYNAMIC(CWhiteListPage, CListPage)

CWhiteListPage::CWhiteListPage(CWnd* pParent /*=NULL*/)
	: CListPage(pParent)
{

}

CWhiteListPage::~CWhiteListPage()
{
}

void CWhiteListPage::DoDataExchange(CDataExchange* pDX)
{
	CListPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWhiteListPage, CListPage)
END_MESSAGE_MAP()


// CWhiteListPage 消息处理程序


BOOL CWhiteListPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_static.SetWindowText(_T("当被测文本等于文本时匹配(无正则)"));
	m_static.ModifyStyle(0, SS_CENTERIMAGE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

BOOL CWhiteListPage::TestMatch(const CString& test, const CString& text, BOOL isRegex)
{
	return test == text;
}
