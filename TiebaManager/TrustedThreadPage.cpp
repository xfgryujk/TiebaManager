// TrustedThreadPage.cpp : 实现文件
//

#include "stdafx.h"
#include "TrustedThreadPage.h"


// CTrustedThreadPage 对话框

IMPLEMENT_DYNAMIC(CTrustedThreadPage, CDialog)

CTrustedThreadPage::CTrustedThreadPage(CWnd* pParent /*=NULL*/)
	: CDialog(CTrustedThreadPage::IDD, pParent)
{

}

#pragma region MFC
CTrustedThreadPage::~CTrustedThreadPage()
{
}

void CTrustedThreadPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_BUTTON1, m_addButton);
	DDX_Control(pDX, IDC_BUTTON2, m_deleteButton);
	DDX_Control(pDX, IDC_BUTTON3, m_changeButton);
	DDX_Control(pDX, IDC_STATIC1, m_static);
}


BEGIN_MESSAGE_MAP(CTrustedThreadPage, CDialog)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_LBN_DBLCLK(IDC_LIST1, &CTrustedThreadPage::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CTrustedThreadPage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTrustedThreadPage::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTrustedThreadPage::OnBnClickedButton3)
END_MESSAGE_MAP()
#pragma endregion

// CTrustedThreadPage 消息处理程序

#pragma region UI
// 屏蔽Esc关闭窗口
void CTrustedThreadPage::OnCancel()
{
}

// 屏蔽回车关闭窗口
void CTrustedThreadPage::OnOK()
{
}

// 销毁窗口
void CTrustedThreadPage::OnClose()
{
	DestroyWindow();

	CDialog::OnClose();
}

// 控件颜色
HBRUSH CTrustedThreadPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->m_hWnd == m_static.m_hWnd)
		pDC->SetTextColor(RGB(255, 0, 0));

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
#pragma endregion

// 双击列表
void CTrustedThreadPage::OnLbnDblclkList1()
{
	CString tmp;
	m_list.GetText(m_list.GetCurSel(), tmp);
	m_edit.SetWindowText(tmp);
}

// 添加
void CTrustedThreadPage::OnBnClickedButton1()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("内容不能为空！"), MB_ICONERROR);
		return;
	}
	int index = m_list.GetCurSel();
	index = m_list.InsertString(index + 1, text);
	m_list.SetCurSel(index);
}

// 删除
void CTrustedThreadPage::OnBnClickedButton2()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_list.DeleteString(index);
	m_list.SetCurSel(index == 0 ? 0 : index - 1);
}

// 修改
void CTrustedThreadPage::OnBnClickedButton3()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("内容不能为空！"), MB_ICONERROR);
		return;
	}
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_list.DeleteString(index);
	index = m_list.InsertString(index, text);
	m_list.SetCurSel(index);
}
