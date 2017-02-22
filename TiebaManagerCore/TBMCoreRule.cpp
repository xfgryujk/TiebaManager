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
#include <TBMCoreRule.h>
using namespace tinyxml2;


// 条件参数 ///////////////////////////////////////////////////////////////////////////

CCondition& CConditionParam::GetCondition()
{
	return CCondition::GetCondition(m_conditionName);
}

CString CConditionParam::GetDescription()
{
	return GetCondition().GetDescription(*this);
}


CConditionParam* CConditionParam::Clone()
{
	return GetCondition().CloneParam(*this);
}


DECLEAR_READ(std::shared_ptr<CConditionParam>)
{
	const XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return;
	}

	COption<CString> name("Name");
	name.Read(*optionNode);

	CCondition& condition = CCondition::GetCondition(name);
	if (&condition == &CCondition::s_defaultCondition)
		AfxMessageBox(_T("未知的条件：") + name, MB_ICONWARNING);

	m_value.reset(condition.ReadParam(optionNode));

	if (!IsValid(m_value))
		UseDefault();
}

DECLEAR_WRITE(std::shared_ptr<CConditionParam>)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<CString> name("Name");
	*name = m_value->m_conditionName;
	name.Write(*optionNode);

	CCondition& condition = CCondition::GetCondition(name);
	condition.WriteParam(*m_value, optionNode);
}


BOOL CConditionParam::MatchThread(const ThreadInfo& thread, int& pos, int& length)
{
	return GetCondition().MatchThread(*this, thread, pos, length);
}

BOOL CConditionParam::MatchPost(const PostInfo& post, int& pos, int& length)
{
	return GetCondition().MatchPost(*this, post, pos, length);
}

BOOL CConditionParam::MatchLzl(const LzlInfo& lzl, int& pos, int& length)
{
	return GetCondition().MatchLzl(*this, lzl, pos, length);
}


// 条件 ///////////////////////////////////////////////////////////////////////////

std::map<CString, CCondition*> CCondition::s_conditions;
CCondition CCondition::s_defaultCondition(_T("默认"));


CCondition& CCondition::GetCondition(const CString& name)
{
	auto it = s_conditions.find(name);
	if (it == s_conditions.end())
		return s_defaultCondition;
	return *it->second;
}

BOOL CCondition::AddCondition(CCondition& condition)
{
	if (s_conditions.find(condition.m_name) != s_conditions.end())
	{
		AfxMessageBox(_T("条件ID冲突：") + condition.m_name, MB_ICONWARNING);
		return FALSE;
	}
	s_conditions[condition.m_name] = &condition;
	return TRUE;
}


CString CCondition::GetDescription(const CConditionParam& param)
{
	return _T("未知条件：") + param.m_conditionName;
}


CCondition::CCondition(const CString& name) :
	m_name(name)
{
}


class CDefaultConditionParam : public CConditionParam
{
public:
	tinyxml2::XMLDocument m_doc;   // 保存未知条件用
};

static XMLNode* DeepCloneXMLNode(const XMLNode* node, XMLNode* newParent)
{
	tinyxml2::XMLDocument* doc = newParent->GetDocument();

	// 拷贝当前结点
	XMLNode* newNode = node->ShallowClone(doc);
	newParent->LinkEndChild(newNode);

	// 拷贝子结点
	for (const XMLNode* child = node->FirstChild(); child != NULL; child = child->NextSiblingElement())
		DeepCloneXMLNode(child, newNode);

	return newNode;
}

CConditionParam* CCondition::ReadParam(const XMLElement* optionNode)
{
	CDefaultConditionParam* param = new CDefaultConditionParam();
	DeepCloneXMLNode(optionNode, &param->m_doc);
	return param;
}

void CCondition::WriteParam(const CConditionParam& _param, XMLElement* optionNode)
{
	const auto& param = (CDefaultConditionParam&)_param;
	DeepCloneXMLNode(&param.m_doc, optionNode->Parent());
	optionNode->Parent()->DeleteChild(optionNode);
}

CConditionParam* CCondition::CloneParam(const CConditionParam& _param)
{
	const auto& param = (CDefaultConditionParam&)_param;
	CDefaultConditionParam* newParam = new CDefaultConditionParam();
	
	newParam->m_conditionName = param.m_conditionName;
	DeepCloneXMLNode(param.m_doc.FirstChild(), &newParam->m_doc);

	return newParam;
}


BOOL CCondition::MatchThread(const CConditionParam& param, const ThreadInfo& thread, int& pos, int& length)
{
	return FALSE;
}

BOOL CCondition::MatchPost(const CConditionParam& param, const PostInfo& post, int& pos, int& length)
{
	return FALSE;
}

BOOL CCondition::MatchLzl(const CConditionParam& param, const LzlInfo& lzl, int& pos, int& length)
{
	return FALSE;
}


// 规则 ///////////////////////////////////////////////////////////////////////////

DECLEAR_READ(CRule)
{
	const XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return;
	}

	COption<CString> name("Name");
	COption<std::vector<std::shared_ptr<CConditionParam> > > conditions("Conditions");
	name.Read(*optionNode);
	conditions.Read(*optionNode);

	m_value.m_name = name;
	m_value.m_conditionParams = std::move(*conditions);

	if (!IsValid(m_value))
		UseDefault();
}

DECLEAR_WRITE(CRule)
{
	tinyxml2::XMLDocument* doc = root.GetDocument();
	XMLElement* optionNode = doc->NewElement(m_name);
	root.LinkEndChild(optionNode);

	COption<CString> name("Name");
	*name = m_value.m_name;
	name.Write(*optionNode);

	XMLElement* conditionsNode = doc->NewElement("Conditions");
	optionNode->LinkEndChild(conditionsNode);
	COption<std::shared_ptr<CConditionParam> > value("value");
	for (const auto& i : m_value.m_conditionParams)
	{
		XMLElement* item = doc->NewElement("item");
		optionNode->LinkEndChild(item);
		*value = i;
		value.Write(*item);
	}
}


CRule::CRule(const CRule& other)
{
	*this = other;
}

CRule::CRule(CRule&& other)
{
	*this = std::move(other);
}

CRule& CRule::operator = (const CRule& other)
{
	m_name = other.m_name;

	m_conditionParams.resize(other.m_conditionParams.size());
	for (size_t i = 0; i < other.m_conditionParams.size(); ++i)
		m_conditionParams[i].reset(other.m_conditionParams[i]->Clone());

	return *this;
}

CRule& CRule::operator = (CRule&& other)
{
	m_name = std::move(other.m_name);
	m_conditionParams = std::move(other.m_conditionParams);

	return *this;
}


BOOL CRule::Match(const ThreadInfo& thread, int& pos, int& length)
{
	if (m_conditionParams.empty())
		return FALSE;
	for (const auto& i : m_conditionParams)
	{
		if (!i->MatchThread(thread, pos, length))
			return FALSE;
	}
	return TRUE;
}

BOOL CRule::Match(const PostInfo& post, int& pos, int& length)
{
	if (m_conditionParams.empty())
		return FALSE;
	for (const auto& i : m_conditionParams)
	{
		if (!i->MatchPost(post, pos, length))
			return FALSE;
	}
	return TRUE;
}

BOOL CRule::Match(const LzlInfo& lzl, int& pos, int& length)
{
	if (m_conditionParams.empty())
		return FALSE;
	for (const auto& i : m_conditionParams)
	{
		if (!i->MatchLzl(lzl, pos, length))
			return FALSE;
	}
	return TRUE;
}

