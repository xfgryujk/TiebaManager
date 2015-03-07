// WhiteContentPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "WhiteContentPage.h"
#include "afxdialogex.h"
#include "Global.h"
#include "SettingDlg.h"


// CWhiteContentPage �Ի���

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


// CWhiteContentPage ��Ϣ��������


BOOL CWhiteContentPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_static.SetWindowText(_T("֧��ͼƬ��ַ\\��Ƶ��ַ���������ı������ı�ʱƥ��"));
	m_static.ModifyStyle(0, SS_CENTERIMAGE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
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