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

#include "stdafx.h"
#include "RulesPage.h"
#include <TBMCoreRules.h>
#include "InputRuleDlg.h"


template class CRulesPage<CRule>;
template class CRulesPage<CIllegalRule>;


template<class RuleType>
CRulesPage<RuleType>::CRulesPage(CWnd* pParent /*=NULL*/) :
	CNormalListPage(_T(""), pParent)
{
}

template<class RuleType>
CRulesPage<RuleType>::CRulesPage(UINT nIDTemplate, CWnd* pParentWnd /*=NULL*/) :
	CNormalListPage(_T(""), nIDTemplate, pParentWnd)
{
}


template<class RuleType>
BOOL CRulesPage<RuleType>::SetItem(int index)
{
	CInputRuleDlg<RuleType> dlg(m_rules[index], CInputRuleDlg<RuleType>::IDD, this);
	if (dlg.DoModal() == IDOK)
	{
		m_list.SetItemText(index, 0, m_rules[index].m_name);
		OnUpdateRule(index);
		return TRUE;
	}
	return FALSE;
}

template<class RuleType>
void CRulesPage<RuleType>::OnAdd(int index)
{
	if (index >= 0)
	{
		if (m_rules.size() != m_list.GetItemCount()) // 添加
			m_rules.insert(m_rules.begin() + index, RuleType());
	}
}

template<class RuleType>
void CRulesPage<RuleType>::OnDelete(int index)
{
	if (index >= 0)
		m_rules.erase(m_rules.begin() + index);
	else
		m_rules.clear();
}

template<class RuleType>
BOOL CRulesPage<RuleType>::Export(const CString& path)
{
	// 导出txt
	if (path.Right(4).CompareNoCase(_T(".xml")) != 0)
		return CListPage::Export(path);

	// 导出xml
	CRuleListFile tmp;
	ApplyList(tmp.m_list);
	return tmp.Save(path);
}

template<class RuleType>
BOOL CRulesPage<RuleType>::Import(const CString& path)
{
	// 导入txt
	if (path.Right(4).CompareNoCase(_T(".xml")) != 0)
	{
		if (CListPage::Import(path))
		{
			m_rules.clear();
			int size = m_list.GetItemCount();
			m_rules.resize(size);
			for (int i = 0; i < size; ++i)
			{
				m_rules[i].m_name = m_list.GetItemText(i, 0);
				OnUpdateRule(i);
			}
			return TRUE;
		}
		return FALSE;
	}

	// 导入xml
	CRuleListFile tmp;
	if (!tmp.Load(path))
		return FALSE;
	ShowList(std::move(tmp.m_list));
	return TRUE;
}

template<class RuleType>
void CRulesPage<RuleType>::ShowList(const std::vector<RuleType>& list)
{
	m_rules = list;

	m_list.DeleteAllItems();
	for (UINT i = 0; i < m_rules.size(); i++)
	{
		m_list.InsertItem(i, m_rules[i].m_name);
		OnUpdateRule(i);
	}
}

template<class RuleType>
void CRulesPage<RuleType>::ShowList(std::vector<RuleType>&& list)
{
	m_rules = std::move(list);

	m_list.DeleteAllItems();
	for (UINT i = 0; i < m_rules.size(); i++)
	{
		m_list.InsertItem(i, m_rules[i].m_name);
		OnUpdateRule(i);
	}
}

template<class RuleType>
void CRulesPage<RuleType>::ApplyList(std::vector<RuleType>& list)
{
	list = m_rules;
}
