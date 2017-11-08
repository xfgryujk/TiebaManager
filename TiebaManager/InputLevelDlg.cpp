/*
Copyright (C) 2011-2017  xfgryujk
https://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

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

// InputLevelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "InputLevelDlg.h"


// CInputLevelDlg 对话框

IMPLEMENT_DYNAMIC(CInputLevelDlg, CDialog)

CInputLevelDlg::CInputLevelDlg(CLevelParam* param, CWnd* pParent /*=NULL*/) : 
	CDialog(CInputLevelDlg::IDD, pParent), 
	m_param(param)
{
}

CInputLevelDlg::~CInputLevelDlg()
{
}

void CInputLevelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO2, m_operatorCombo);
	DDX_Control(pDX, IDC_EDIT1, m_levelEdit);
}


BEGIN_MESSAGE_MAP(CInputLevelDlg, CDialog)
END_MESSAGE_MAP()


// CInputLevelDlg 消息处理程序

BOOL CInputLevelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_operatorCombo.SetCurSel(m_param->m_operator);
	CString tmp;
	tmp.Format(_T("%d"), m_param->m_level);
	m_levelEdit.SetWindowText(tmp);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CInputLevelDlg::OnOK()
{
	m_param->m_operator = CLevelParam::Operator(m_operatorCombo.GetCurSel());
	CString tmp;
	m_levelEdit.GetWindowText(tmp);
	m_param->m_level = _ttoi(tmp);
	if (m_param->m_level < 1 || m_param->m_level > 18)
	{
		AfxMessageBox(_T("等级范围1到18"), MB_ICONERROR);
		return;
	}

	CDialog::OnOK();
}
