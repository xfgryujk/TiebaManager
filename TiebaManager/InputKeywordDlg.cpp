// InputKeywordDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "InputKeywordDlg.h"


// CInputKeywordDlg 对话框

IMPLEMENT_DYNAMIC(CInputKeywordDlg, CInputDlg)

CInputKeywordDlg::CInputKeywordDlg(const CString& title, CString& content, BOOL* isRegex, BOOL* forceToConfirm, CWnd* pParent /*=NULL*/)
	: CInputDlg(title, content, isRegex, TRUE, IDD, pParent),
	m_forceToConfirm(forceToConfirm)
{

}

CInputKeywordDlg::~CInputKeywordDlg()
{
}

void CInputKeywordDlg::DoDataExchange(CDataExchange* pDX)
{
	CInputDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK2, m_forceToConfirmCheck);
}


BEGIN_MESSAGE_MAP(CInputKeywordDlg, CInputDlg)
END_MESSAGE_MAP()


// CInputKeywordDlg 消息处理程序

// 初始化
BOOL CInputKeywordDlg::OnInitDialog()
{
	CInputDlg::OnInitDialog();

	if (m_forceToConfirm != NULL)
		m_forceToConfirmCheck.SetCheck(*m_forceToConfirm);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 确定
void CInputKeywordDlg::OnOK()
{
	if (m_forceToConfirm != NULL)
		*m_forceToConfirm = m_forceToConfirmCheck.GetCheck();

	CInputDlg::OnOK();
}
