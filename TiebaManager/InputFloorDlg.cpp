/*
Copyright (C) 2011-2017  xfgryujk
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

// InputFloorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "InputFloorDlg.h"


// CInputFloorDlg 对话框

IMPLEMENT_DYNAMIC(CInputFloorDlg, CDialog)

CInputFloorDlg::CInputFloorDlg(CFloorParam* param, CWnd* pParent /*=NULL*/) : 
	CDialog(CInputFloorDlg::IDD, pParent), 
	m_param(param)
{
}

CInputFloorDlg::~CInputFloorDlg()
{
}

void CInputFloorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO2, m_operatorCombo);
	DDX_Control(pDX, IDC_EDIT1, m_floorEdit);
}


BEGIN_MESSAGE_MAP(CInputFloorDlg, CDialog)
END_MESSAGE_MAP()


// CInputFloorDlg 消息处理程序

BOOL CInputFloorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_operatorCombo.SetCurSel(m_param->m_operator);
	CString tmp;
	tmp.Format(_T("%d"), m_param->m_floor);
	m_floorEdit.SetWindowText(tmp);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CInputFloorDlg::OnOK()
{
	m_param->m_operator = CFloorParam::Operator(m_operatorCombo.GetCurSel());
	CString tmp;
	m_floorEdit.GetWindowText(tmp);
	m_param->m_floor = _ttoi(tmp);
	if (m_param->m_floor < 1)
	{
		AfxMessageBox(_T("楼层应大于或等于1"), MB_ICONERROR);
		return;
	}

	CDialog::OnOK();
}
