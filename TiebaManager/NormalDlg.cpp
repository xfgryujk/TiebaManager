// NormalDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "NormalDlg.h"


// CNormalDlg 对话框

IMPLEMENT_DYNAMIC(CNormalDlg, CDialog)

CNormalDlg::CNormalDlg(UINT nIDTemplate, CWnd* pParentWnd)
	: CDialog(nIDTemplate, pParentWnd),
	m_resize(this)
{

}

#pragma region MFC
CNormalDlg::~CNormalDlg()
{
}

void CNormalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNormalDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_SIZE()
END_MESSAGE_MAP()
#pragma endregion

// CNormalDlg 消息处理程序

// 屏蔽Esc关闭窗口
void CNormalDlg::OnCancel()
{
}

// 屏蔽回车关闭窗口
void CNormalDlg::OnOK()
{
}

// 销毁窗口
void CNormalDlg::OnClose()
{
	DestroyWindow();

	CDialog::OnClose();
}

// 改变尺寸
void CNormalDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	m_resize.Resize();
}
