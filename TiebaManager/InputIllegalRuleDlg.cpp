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

// InputIllegalRuleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "InputIllegalRuleDlg.h"


// CInputIllegalRuleDlg 对话框

CInputIllegalRuleDlg::CInputIllegalRuleDlg(CIllegalRule& rule, UINT nIDTemplate, CWnd* pParent /*=NULL*/) :
	CInputRuleDlg(rule, nIDTemplate, pParent)
{
}

#pragma region MFC
CInputIllegalRuleDlg::~CInputIllegalRuleDlg()
{
}

void CInputIllegalRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CInputRuleDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_forceToComfirmCheck);
}


BEGIN_MESSAGE_MAP(CInputIllegalRuleDlg, CInputRuleDlg<CIllegalRule>)
END_MESSAGE_MAP()
#pragma endregion

// CInputIllegalRuleDlg 消息处理程序

// 初始化
BOOL CInputIllegalRuleDlg::OnInitDialog()
{
	CInputRuleDlg::OnInitDialog();

	m_resize.AddControl(&m_forceToComfirmCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

BOOL CInputIllegalRuleDlg::SetItem(int index)
{
	if (CInputRuleDlg::SetItem(index))
	{
		m_ruleCopy.m_trigCount = 0;
		return TRUE;
	}
	return FALSE;
}

void CInputIllegalRuleDlg::ShowList(const CIllegalRule& list)
{
	CInputRuleDlg::ShowList(list);

	m_forceToComfirmCheck.SetCheck(m_ruleCopy.m_forceToConfirm);
}

void CInputIllegalRuleDlg::ShowList(CIllegalRule&& list)
{
	CInputRuleDlg::ShowList(std::move(list));

	m_forceToComfirmCheck.SetCheck(m_ruleCopy.m_forceToConfirm);
}


// 确认
void CInputIllegalRuleDlg::OnOK()
{
	m_ruleCopy.m_forceToConfirm = m_forceToComfirmCheck.GetCheck();

	CInputRuleDlg::OnOK();
}
