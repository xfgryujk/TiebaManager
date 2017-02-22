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

#include "stdafx.h"
#include "IllegalRulesPage.h"
#include "SettingDlg.h"
#include "InputIllegalRuleDlg.h"


CIllegalRulesPage::CIllegalRulesPage(const CString& inputTitle, CWnd* pParent /*=NULL*/) :
	CRulesPage(inputTitle, pParent)
{
}

CIllegalRulesPage::CIllegalRulesPage(const CString& inputTitle, UINT nIDTemplate, CWnd* pParentWnd /*=NULL*/) : 
	CRulesPage(inputTitle, nIDTemplate, pParentWnd)
{
}


BOOL CIllegalRulesPage::OnInitDialog()
{
	CRulesPage::OnInitDialog();

	m_list.DeleteColumn(0);
	m_list.ModifyStyle(LVS_NOCOLUMNHEADER, 0);
	int i = 0;
	m_list.InsertColumn(i++, _T("规则名"), LVCFMT_LEFT, 350);
	m_list.InsertColumn(i++, _T("强制确认"), LVCFMT_LEFT, 80);
	m_list.InsertColumn(i++, _T("触发次数"), LVCFMT_LEFT, 80);

	m_static.SetWindowText(_T("匹配的帖子为违规帖"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

BOOL CIllegalRulesPage::SetItem(int index)
{
	CInputIllegalRuleDlg dlg(m_rules[index], CInputIllegalRuleDlg::IDD, this);
	if (dlg.DoModal() == IDOK)
	{
		m_list.SetItemText(index, 0, m_rules[index].m_name);
		OnUpdateRule(index);

		((CSettingDlg*)GetParent())->m_clearScanCache = TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL CIllegalRulesPage::Import(const CString& path)
{
	if (CRulesPage::Import(path))
		((CSettingDlg*)GetParent())->m_clearScanCache = TRUE;
}

void CIllegalRulesPage::OnUpdateRule(int index)
{
	m_list.SetItemText(index, 1, m_rules[index].m_forceToConfirm ? _T("是") : _T("否"));
	CString tmp;
	tmp.Format(_T("%d"), m_rules[index].m_trigCount);
	m_list.SetItemText(index, 2, tmp);
}
