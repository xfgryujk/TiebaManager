// ExplorerPage.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "ExplorerPage.h"
#include "afxdialogex.h"


// CExplorerPage 对话框

IMPLEMENT_DYNAMIC(CExplorerPage, CDialog)

CExplorerPage::CExplorerPage(CWnd* pParent /*=NULL*/)
	: CDialog(CExplorerPage::IDD, pParent)
{

}

#pragma region MFC
CExplorerPage::~CExplorerPage()
{
}

void CExplorerPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_static);
	DDX_Control(pDX, IDC_EDIT2, m_edit);
	DDX_Control(pDX, IDC_BUTTON1, m_gotoButton);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CExplorerPage, CDialog)
	ON_WM_CLOSE()
	ON_WM_SIZE()
END_MESSAGE_MAP()
#pragma endregion

// CExplorerPage 消息处理程序

#pragma region UI
// 屏蔽Esc关闭窗口
void CExplorerPage::OnCancel()
{
}

// 屏蔽回车关闭窗口
void CExplorerPage::OnOK()
{
}

// 销毁窗口
void CExplorerPage::OnClose()
{
	DestroyWindow();

	CDialog::OnClose();
}

// 改变尺寸
void CExplorerPage::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_list.m_hWnd == NULL)
		return;

	CRect rect;
	GetClientRect(&rect); // 默认684 * 315
	m_list.SetWindowPos(NULL, 0, 0, rect.Width() - 22, rect.Height() - 56, SWP_NOMOVE | SWP_NOREDRAW);

	Invalidate();
}
#pragma endregion
