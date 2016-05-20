// CLockThreadDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "LockThreadDlg.h"
#include "LockThread.h"


// CLockThreadDlg 对话框

IMPLEMENT_DYNAMIC(CLockThreadDlg, CNormalDlg)

CLockThreadDlg::CLockThreadDlg(CLockThreadDlg*& pThis, CWnd* pParent /*=NULL*/) : CNormalDlg(CLockThreadDlg::IDD, pParent),
	m_pThis(pThis)
{

}

#pragma region MFC
CLockThreadDlg::~CLockThreadDlg()
{
}

void CLockThreadDlg::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_tidEdit);
	DDX_Control(pDX, IDC_EDIT2, m_pageEdit);
	DDX_Control(pDX, IDC_EDIT7, m_floorEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_startButton);
	DDX_Control(pDX, IDC_BUTTON2, m_stopButton);
}


BEGIN_MESSAGE_MAP(CLockThreadDlg, CNormalDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CLockThreadDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CLockThreadDlg::OnBnClickedButton2)
END_MESSAGE_MAP()
#pragma endregion

// CLockThreadDlg 消息处理程序

// 初始化
BOOL CLockThreadDlg::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	if (!g_lockThread.m_stopFlag)
	{
		m_tidEdit.EnableWindow(FALSE);
		m_pageEdit.EnableWindow(FALSE);
		m_floorEdit.EnableWindow(FALSE);
		m_startButton.EnableWindow(FALSE);
		m_stopButton.EnableWindow(TRUE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

#pragma region UI
// 取消
void CLockThreadDlg::OnCancel()
{
	DestroyWindow();
}

// 关闭窗口
void CLockThreadDlg::OnClose()
{
	DestroyWindow();
}

// 释放this
void CLockThreadDlg::PostNcDestroy()
{
	CNormalDlg::PostNcDestroy();

	m_pThis = NULL;
	delete this;
}
#pragma endregion

// 开始
void CLockThreadDlg::OnBnClickedButton1()
{
	m_tidEdit.EnableWindow(FALSE);
	m_pageEdit.EnableWindow(FALSE);
	m_floorEdit.EnableWindow(FALSE);
	m_startButton.EnableWindow(FALSE);
	m_stopButton.EnableWindow(TRUE);
	
	g_lockThread.StartLockThread();
}

// 停止
void CLockThreadDlg::OnBnClickedButton2()
{
	g_lockThread.StopLockThread();
}
