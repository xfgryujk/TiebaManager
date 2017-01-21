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

// DefriendDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DefriendDlg.h"
#include "Defriend.h"

#include <StringHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>

#include <TBMAPI.h>
#include <TBMCoreConfig.h>
#include <TiebaOperate.h>


extern std::unique_ptr<CDefriend> g_defriend;

// CDefriendDlg 对话框

CString CDefriendDlg::s_startPage;
CString CDefriendDlg::s_endPage;
BOOL CDefriendDlg::s_defriendNewUsers = FALSE;


IMPLEMENT_DYNAMIC(CDefriendDlg, CModelessDlg)

CDefriendDlg::CDefriendDlg(CDefriendDlg*& pThis, CWnd* pParent /*=NULL*/) :
	CModelessDlg(CDefriendDlg::IDD, (CModelessDlg**)&pThis, pParent)
{
}

#pragma region MFC
CDefriendDlg::~CDefriendDlg()
{
}

void CDefriendDlg::DoDataExchange(CDataExchange* pDX)
{
	CModelessDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_startPageEdit);
	DDX_Control(pDX, IDC_EDIT2, m_endPageEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_startButton);
	DDX_Control(pDX, IDC_BUTTON2, m_stopButton);
	DDX_Control(pDX, IDC_STATIC2, m_stateStatic);
	DDX_Control(pDX, IDC_CHECK1, m_defriendNewUsersCheck);
}


BEGIN_MESSAGE_MAP(CDefriendDlg, CModelessDlg)
	ON_BN_CLICKED(IDC_BUTTON1, &CDefriendDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDefriendDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CHECK1, &CDefriendDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()
#pragma endregion

// CDefriendDlg 消息处理程序

// 初始化
BOOL CDefriendDlg::OnInitDialog()
{
	CModelessDlg::OnInitDialog();

	if (!g_defriend->m_stopFlag)
	{
		m_startPageEdit.EnableWindow(FALSE);
		m_endPageEdit.EnableWindow(FALSE);
		m_defriendNewUsersCheck.EnableWindow(FALSE);
		m_startButton.EnableWindow(FALSE);
		m_stopButton.EnableWindow(TRUE);
		m_stateStatic.SetWindowText(_T("拉黑中"));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 拉黑新关注的
void CDefriendDlg::OnBnClickedCheck1()
{
	if (m_defriendNewUsersCheck.GetCheck())
	{
		m_startPageEdit.EnableWindow(FALSE);
		m_startPageEdit.SetWindowText(_T("1"));
	}
	else
		m_startPageEdit.EnableWindow(TRUE);
}

// 开始
void CDefriendDlg::OnBnClickedButton1()
{
	m_startPageEdit.GetWindowText(s_startPage);
	m_endPageEdit.GetWindowText(s_endPage);
	int iStartPage = _ttoi(s_startPage), iEndPage = _ttoi(s_endPage);
	if (iStartPage > iEndPage)
	{
		m_startPageEdit.SetWindowText(s_endPage);
		m_endPageEdit.SetWindowText(s_startPage);
		CString st = s_startPage;
		s_startPage = s_endPage;
		s_endPage = st;
		int t = iStartPage;
		iStartPage = iEndPage;
		iEndPage = t;
	}
	if (iStartPage < 1)
	{
		AfxMessageBox(_T("起始页数应大于0！"), MB_ICONERROR);
		return;
	}
	s_defriendNewUsers = m_defriendNewUsersCheck.GetCheck();

	m_startPageEdit.EnableWindow(FALSE);
	m_endPageEdit.EnableWindow(FALSE);
	m_defriendNewUsersCheck.EnableWindow(FALSE);
	m_startButton.EnableWindow(FALSE);
	m_stopButton.EnableWindow(TRUE);
	
	g_defriend->StartDefriend(s_startPage, s_endPage, s_defriendNewUsers);
}

// 停止
void CDefriendDlg::OnBnClickedButton2()
{
	g_defriend->StopDefriend();
}
