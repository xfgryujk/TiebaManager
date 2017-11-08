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

// InputRuleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "InputRuleDlg.h"
#include <TBMCoreRules.h>
#include "ConditionGUI.h"
#include <ComboDlg.h>


template class CInputRuleDlg<CRule>;
template class CInputRuleDlg<CIllegalRule>;


// CInputRuleDlg 对话框

template<class RuleType>
CInputRuleDlg<RuleType>::CInputRuleDlg(RuleType& rule, UINT nIDTemplate, CWnd* pParent /*=NULL*/) :
	CNormalListPage(_T(""), nIDTemplate, pParent),
	m_rule(rule)
{
}

#pragma region MFC
template<class RuleType>
CInputRuleDlg<RuleType>::~CInputRuleDlg()
{
}

template<class RuleType>
void CInputRuleDlg<RuleType>::DoDataExchange(CDataExchange* pDX)
{
	CNormalListPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_ruleNameEdit);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
}

PTM_WARNING_DISABLE
template<class RuleType>
const AFX_MSGMAP* CInputRuleDlg<RuleType>::GetMessageMap() const { return GetThisMessageMap(); }
template<class RuleType>
const AFX_MSGMAP* PASCAL CInputRuleDlg<RuleType>::GetThisMessageMap()
{
	typedef CInputRuleDlg<RuleType> ThisClass;
	typedef CNormalListPage TheBaseClass;
	static const AFX_MSGMAP_ENTRY _messageEntries[] =
	{
		ON_WM_CLOSE()
		{ 0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 }
	};
	static const AFX_MSGMAP messageMap = { &TheBaseClass::GetThisMessageMap, &_messageEntries[0] };
	return &messageMap;
}
PTM_WARNING_RESTORE
#pragma endregion

// CInputRuleDlg 消息处理程序

// 初始化
template<class RuleType>
BOOL CInputRuleDlg<RuleType>::OnInitDialog()
{
	CNormalListPage::OnInitDialog();

	m_resize.AddControl(&m_ruleNameEdit, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_okButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_cancelButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);

	ShowList(m_rule);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

template<class RuleType>
BOOL CInputRuleDlg<RuleType>::SetItem(int index)
{
	auto& conditionGUIManager = CConditionGUIManager::GetInstance();

	int iConditionGUI;
	if (m_ruleCopy.m_conditionParams[index] == nullptr) // 刚添加，需要选择条件类型
	{
		// 选择条件类型
		std::vector<CString> conditions;
		conditionGUIManager.GetConditionGUINames(conditions);
		iConditionGUI = 0;
		CComboDlg dlg(_T("条件类型："), conditions, iConditionGUI);
		if (dlg.DoModal() != IDOK)
			return FALSE;
	}
	else
	{
		// 寻找回调
		iConditionGUI = conditionGUIManager.GetConditionGUIIndex(m_ruleCopy.m_conditionParams[index]->m_conditionName);
		if (iConditionGUI == -1)
			return FALSE;
	}

	// 设置条件
	CConditionParam* param = conditionGUIManager.GetOnSetCondition(iConditionGUI)(m_ruleCopy.m_conditionParams[index].get());
	if (param == nullptr)
		return FALSE;
	if (param != m_ruleCopy.m_conditionParams[index].get())
		m_ruleCopy.m_conditionParams[index].reset(param);
	m_list.SetItemText(index, 0, param->GetDescription());
	return TRUE;
}

// 添加
template<class RuleType>
void CInputRuleDlg<RuleType>::OnAdd(int index)
{
	if (index >= 0)
	{
		if (m_ruleCopy.m_conditionParams.size() != m_list.GetItemCount())
			m_ruleCopy.m_conditionParams.insert(m_ruleCopy.m_conditionParams.begin() + index, nullptr); // 选择条件后再分配内存
	}
}

// 删除
template<class RuleType>
void CInputRuleDlg<RuleType>::OnDelete(int index)
{
	if (index >= 0)
		m_ruleCopy.m_conditionParams.erase(m_ruleCopy.m_conditionParams.begin() + index);
	else
		m_ruleCopy.m_conditionParams.clear();
}

template<class RuleType>
BOOL CInputRuleDlg<RuleType>::Export(const CString& path)
{
	// 导出txt
	if (path.Right(4).CompareNoCase(_T(".xml")) != 0)
		return FALSE;

	// 导出xml
	CRuleFile tmp;
	m_ruleNameEdit.GetWindowText(m_ruleCopy.m_name);
	*tmp.m_list = m_ruleCopy;
	return tmp.Save(path);
}

template<class RuleType>
BOOL CInputRuleDlg<RuleType>::Import(const CString& path)
{
	// 导入txt
	if (path.Right(4).CompareNoCase(_T(".xml")) != 0)
		return FALSE;

	// 导入xml
	CRuleFile tmp;
	if (!tmp.Load(path))
		return FALSE;
	ShowList(std::move(tmp.m_list));
	return TRUE;
}

template<class RuleType>
void CInputRuleDlg<RuleType>::ShowList(const RuleType& list)
{
	m_ruleCopy = list;

	m_ruleNameEdit.SetWindowText(m_ruleCopy.m_name);

	m_list.DeleteAllItems();
	for (UINT i = 0; i < m_ruleCopy.m_conditionParams.size(); i++)
		m_list.InsertItem(i, m_ruleCopy.m_conditionParams[i]->GetDescription());
}

template<class RuleType>
void CInputRuleDlg<RuleType>::ShowList(RuleType&& list)
{
	m_ruleCopy = std::move(list);

	m_ruleNameEdit.SetWindowText(m_ruleCopy.m_name);

	m_list.DeleteAllItems();
	for (UINT i = 0; i < m_ruleCopy.m_conditionParams.size(); i++)
		m_list.InsertItem(i, m_ruleCopy.m_conditionParams[i]->GetDescription());
}


template<class RuleType>
void CInputRuleDlg<RuleType>::OnCancel()
{
	CDialog::OnCancel();
}

// 确认
template<class RuleType>
void CInputRuleDlg<RuleType>::OnOK()
{
	if (m_ruleCopy.m_conditionParams.empty())
		AfxMessageBox(_T("没有添加条件，此规则将无效"), MB_ICONWARNING);

	m_ruleNameEdit.GetWindowText(m_ruleCopy.m_name);
	m_rule = std::move(m_ruleCopy);

	CDialog::OnOK();
}

template<class RuleType>
void CInputRuleDlg<RuleType>::OnClose()
{
	CDialog::OnClose();
}
