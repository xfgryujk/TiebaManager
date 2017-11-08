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

// InputTimeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "InputTimeDlg.h"


// CInputTimeDlg 对话框

IMPLEMENT_DYNAMIC(CInputTimeDlg, CDialog)

CInputTimeDlg::CInputTimeDlg(CTimeParam* param, CWnd* pParent /*=NULL*/) :
	CDialog(CInputTimeDlg::IDD, pParent), 
	m_param(param)
{
}

CInputTimeDlg::~CInputTimeDlg()
{
}

void CInputTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO2, m_operatorCombo);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_timeCtrl);
}


BEGIN_MESSAGE_MAP(CInputTimeDlg, CDialog)
END_MESSAGE_MAP()


// CInputTimeDlg 消息处理程序


BOOL CInputTimeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_operatorCombo.SetCurSel(m_param->m_operator);
	m_timeCtrl.SetFormat(_T("yyyy-MM-dd HH:mm:ss"));
	CTime time(m_param->m_time);
	m_timeCtrl.SetTime(&time);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CInputTimeDlg::OnOK()
{
	m_param->m_operator = CTimeParam::Operator(m_operatorCombo.GetCurSel());
	CTime time;
	m_timeCtrl.GetTime(time);
	m_param->m_time = time.GetTime();

	CDialog::OnOK();
}
