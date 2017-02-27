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

// ComboDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <ComboDlg.h>
#include "resource.h"


// CComboDlg 对话框

const UINT CComboDlg::IDD = IDD_COMBO_DIALOG;


IMPLEMENT_DYNAMIC(CComboDlg, CDialog)

CComboDlg::CComboDlg(const CString& title, const std::vector<CString>& list, int& select, UINT nIDTemplate, CWnd* pParent /*=NULL*/) :
	CDialog(nIDTemplate, pParent), 
	m_title(title), 
	m_list(list), 
	m_select(select)
{
}

CComboDlg::~CComboDlg()
{
}

void CComboDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo);
}


BEGIN_MESSAGE_MAP(CComboDlg, CDialog)
END_MESSAGE_MAP()


// CComboDlg 消息处理程序

// 初始化
BOOL CComboDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_title);

	for (const auto& i : m_list)
		m_combo.AddString(i);

	m_combo.SetCurSel(m_select);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 确定
void CComboDlg::OnOK()
{
	m_select = m_combo.GetCurSel();

	CDialog::OnOK();
}
