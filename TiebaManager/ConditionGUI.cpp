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
#include "ConditionGUI.h"
#include "InputKeywordDlg.h"
#include "InputLevelDlg.h"
#include "InputTimeDlg.h"
#include "InputImageDlg.h"
#include "InputFloorDlg.h"


CConditionGUIManager::CConditionGUIManager()
{
	Init();
}

CConditionGUIManager::~CConditionGUIManager()
{
	Uninit();
}

template<class ParamType, class EditDlg>
static CConditionParam* SetCondition(CConditionParam* _param)
{
	auto param = (ParamType*)_param;
	BOOL release = FALSE;
	if (param == nullptr)
	{
		param = new ParamType();
		release = TRUE;
	}

	EditDlg dlg(param);
	if (dlg.DoModal() == IDOK)
		return param;

	if (release)
		delete param;
	return nullptr;
}

void CConditionGUIManager::Init()
{
	AddConditionGUI(_T("关键词条件"), SetCondition<CKeywordParam, CInputKeywordDlg>);
	AddConditionGUI(_T("等级条件"), SetCondition<CLevelParam, CInputLevelDlg>);
	AddConditionGUI(_T("时间条件"), SetCondition<CTimeParam, CInputTimeDlg>);
	AddConditionGUI(_T("图片条件"), SetCondition<CImageParam, CInputImageDlg>);
	AddConditionGUI(_T("楼层条件"), SetCondition<CFloorParam, CInputFloorDlg>);
}

void CConditionGUIManager::Uninit()
{
	m_conditionGUI.clear();
}


BOOL CConditionGUIManager::AddConditionGUI(const CString& conditionName, std::function<CConditionParam*(CConditionParam*)> onSetCondition)
{
	if (&CCondition::GetCondition(conditionName) == &CCondition::s_defaultCondition)
		return FALSE;
	if (GetConditionGUIIndex(conditionName) != -1)
		return FALSE;

	m_conditionGUI.push_back(CConditionGUI{
		conditionName,
		std::move(onSetCondition)
	});
	return TRUE;
}

void CConditionGUIManager::GetConditionGUINames(std::vector<CString>& list)
{
	size_t size = m_conditionGUI.size();
	list.resize(size);
	for (size_t i = 0; i < size; ++i)
		list[i] = m_conditionGUI[i].m_name;
}

int CConditionGUIManager::GetConditionGUIIndex(const CString& name)
{
	for (size_t i = 0; i < m_conditionGUI.size(); ++i)
	{
		if (m_conditionGUI[i].m_name == name)
			return i;
	}
	return -1;
}

const std::function<CConditionParam*(CConditionParam*)>& CConditionGUIManager::GetOnSetCondition(int index)
{
	return m_conditionGUI[index].m_onSetCondition;
}
