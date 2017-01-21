/*
Copyright (C) 2015  xfgryujk
http://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

// CLockThreadDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "LockThreadDlg.h"
#include "LockThread.h"


extern std::unique_ptr<CLockThread> g_lockThread;

// CLockThreadDlg 对话框

IMPLEMENT_DYNAMIC(CLockThreadDlg, CModelessDlg)

CLockThreadDlg::CLockThreadDlg(CLockThreadDlg*& pThis, CWnd* pParent /*=NULL*/) : CModelessDlg(CLockThreadDlg::IDD, (CModelessDlg**)&pThis, pParent)
{

}

#pragma region MFC
CLockThreadDlg::~CLockThreadDlg()
{
}

void CLockThreadDlg::DoDataExchange(CDataExchange* pDX)
{
	CModelessDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_tidEdit);
	DDX_Control(pDX, IDC_EDIT2, m_pageEdit);
	DDX_Control(pDX, IDC_EDIT7, m_floorEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_startButton);
	DDX_Control(pDX, IDC_BUTTON2, m_stopButton);
}


BEGIN_MESSAGE_MAP(CLockThreadDlg, CModelessDlg)
	ON_BN_CLICKED(IDC_BUTTON1, &CLockThreadDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CLockThreadDlg::OnBnClickedButton2)
END_MESSAGE_MAP()
#pragma endregion

// CLockThreadDlg 消息处理程序

// 初始化
BOOL CLockThreadDlg::OnInitDialog()
{
	CModelessDlg::OnInitDialog();

	if (!g_lockThread->m_stopFlag)
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

// 开始
void CLockThreadDlg::OnBnClickedButton1()
{
	m_tidEdit.EnableWindow(FALSE);
	m_pageEdit.EnableWindow(FALSE);
	m_floorEdit.EnableWindow(FALSE);
	m_startButton.EnableWindow(FALSE);
	m_stopButton.EnableWindow(TRUE);
	
	g_lockThread->StartLockThread();
}

// 停止
void CLockThreadDlg::OnBnClickedButton2()
{
	g_lockThread->StopLockThread();
}
