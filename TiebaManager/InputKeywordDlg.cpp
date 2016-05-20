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
