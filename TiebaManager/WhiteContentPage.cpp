// WhiteContentPage.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "WhiteContentPage.h"
#include "afxdialogex.h"
#include "Global.h"
#include "SettingDlg.h"


// CWhiteContentPage 对话框

IMPLEMENT_DYNAMIC(CWhiteContentPage, CListPage)

CWhiteContentPage::CWhiteContentPage(CWnd* pParent /*=NULL*/)
	: CListPage(pParent)
{

}

CWhiteContentPage::~CWhiteContentPage()
{
}

void CWhiteContentPage::DoDataExchange(CDataExchange* pDX)
{
	CListPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWhiteContentPage, CListPage)
END_MESSAGE_MAP()


// CWhiteContentPage 消息处理程序


BOOL CWhiteContentPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_static.SetWindowText(_T("支持图片地址\\视频地址，当被测文本含有文本时匹配"));
	m_static.ModifyStyle(0, SS_CENTERIMAGE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

BOOL CWhiteContentPage::CheckMatchTooMuch(const CString& text, BOOL isRegex)
{
	return StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST1, text, isRegex) 
		&& StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST2, text, isRegex);
}

BOOL CWhiteContentPage::TestMatch(const CString& test, const CString& text, BOOL isRegex)
{
	return StringIncludes(test, text, isRegex);
}
