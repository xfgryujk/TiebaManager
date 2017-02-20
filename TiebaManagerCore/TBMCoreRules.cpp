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
#include <TBMCoreRules.h>
using namespace tinyxml2;


// 规则 ///////////////////////////////////////////////////////////////////////////

DECLEAR_READ(CIllegalRule)
{
	const XMLElement* optionNode = root.FirstChildElement(m_name);
	if (optionNode == NULL)
	{
		UseDefault();
		return;
	}

	COption<CRule> rule(m_name);
	COption<BOOL> forceToConfirm("ForceToConfirm");
	COption<int> trigCount("TrigCount");
	rule.Read(root);
	forceToConfirm.Read(*optionNode);
	trigCount.Read(*optionNode);

	m_value.m_name = std::move(rule->m_name);
	m_value.m_conditionParams = std::move(rule->m_conditionParams);
	m_value.m_forceToConfirm = forceToConfirm;
	m_value.m_trigCount = trigCount;

	if (!IsValid(m_value))
		UseDefault();
}

DECLEAR_WRITE(CIllegalRule)
{
	COption<CRule> rule(m_name);
	*rule = m_value;
	rule.Write(root);

	XMLElement* optionNode = root.FirstChildElement(m_name);

	COption<BOOL> forceToConfirm("ForceToConfirm");
	*forceToConfirm = m_value.m_forceToConfirm;
	forceToConfirm.Write(*optionNode);
	COption<int> trigCount("TrigCount");
	*trigCount = m_value.m_trigCount;
	trigCount.Write(*optionNode);
}


// 条件 ///////////////////////////////////////////////////////////////////////////

void InitRules()
{
	CCondition::AddCondition(CKeywordCondition::GetInstance());
	CCondition::AddCondition(CLevelCondition::GetInstance());
}


// 关键词条件

CString CKeywordCondition::GetDescription(const CConditionParam& _param)
{
	const auto& param = (CKeywordParam&)_param;
	static LPCTSTR rangeDesc[] = {
		_T("主题标题"),
		_T("主题预览"),
		_T("帖子内容"),
		_T("楼中楼内容"),
		_T("作者名"),
		_T("所有内容")
	};
	
	CString res = rangeDesc[param.m_range];
	if (param.m_not)
		res += _T("不");
	res += param.m_include ? _T("含有") : _T("匹配");
	res += param.m_keyword.isRegex ? _T("正则表达式\"") : _T("\"");
	res += param.m_keyword.text;
	res += _T("\"");
	return res;
}


CConditionParam* CKeywordCondition::ReadParam(const tinyxml2::XMLElement* optionNode)
{
	auto* param = new CKeywordParam();

	param->m_conditionName = m_name;
	COption<int> range("Range");
	COption<BOOL> not("Not");
	COption<BOOL> include("Include");
	COption<RegexText> keyword("Keyword");
	range.Read(*optionNode);
	not.Read(*optionNode);
	include.Read(*optionNode);
	keyword.Read(*optionNode);

	param->m_range = (0 <= range && range <= KeywordRange::ALL_CONTENT) ? KeywordRange(*range) : KeywordRange::ALL_CONTENT;
	param->m_not = not;
	param->m_include = include;
	param->m_keyword = keyword;

	return param;
}

void CKeywordCondition::WriteParam(const CConditionParam& _param, tinyxml2::XMLElement* optionNode)
{
	const auto& param = (CKeywordParam&)_param;

	COption<int> range("Range");
	*range = param.m_range;
	range.Write(*optionNode);
	COption<BOOL> not("Not");
	*not = param.m_not;
	not.Write(*optionNode);
	COption<BOOL> include("Include");
	*include = param.m_include;
	include.Write(*optionNode);
	COption<RegexText> keyword("Keyword");
	*keyword = param.m_keyword;
	keyword.Write(*optionNode);
}


BOOL CKeywordCondition::MatchContent(const CKeywordParam& param, const CString& content, int startPos, int& pos, int& length)
{
	// 判断匹配
	BOOL res;
	if (param.m_include)
	{
		res = StringIncludes(content, param.m_keyword, &pos, &length);
		pos += startPos;
	}
	else
	{
		res = StringMatchs(content, param.m_keyword);
		pos = 0;
		length = content.GetLength();
	}

	// 取FALSE
	if (param.m_not)
		res = !res;

	return res;
}

BOOL CKeywordCondition::MatchThread(const CConditionParam& _param, const ThreadInfo& thread, int& pos, int& length)
{
	const auto& param = (CKeywordParam&)_param;

	// 判断范围
	int startPos;
	CString content;
	switch (param.m_range)
	{
	default: return FALSE;
	case KeywordRange::TITLE:           startPos = 0;                                    content = thread.title;         break;
	case KeywordRange::PREVIEW:         startPos = thread.title.GetLength() + 2;         content = thread.preview;       break;
	case KeywordRange::AUTHOR:          startPos = thread.GetContent().GetLength() + 7;  content = thread.author;        break;
	case KeywordRange::ALL_CONTENT:     startPos = 0;                                    content = thread.GetContent();  break;
	}

	return MatchContent(param, content, startPos, pos, length);
}

BOOL CKeywordCondition::MatchPost(const CConditionParam& _param, const PostInfo& post, int& pos, int& length)
{
	const auto& param = (CKeywordParam&)_param;

	// 判断范围
	int startPos;
	CString content;
	switch (param.m_range)
	{
	default: return FALSE;
	case KeywordRange::POST_CONTENT:    startPos = 0;                                    content = post.content;         break;
	case KeywordRange::AUTHOR:          startPos = post.GetContent().GetLength() + 7;    content = post.author;          break;
	case KeywordRange::ALL_CONTENT:     startPos = 0;                                    content = post.GetContent();    break;
	}

	return MatchContent(param, content, startPos, pos, length);
}

BOOL CKeywordCondition::MatchLzl(const CConditionParam& _param, const LzlInfo& lzl, int& pos, int& length)
{
	const auto& param = (CKeywordParam&)_param;

	// 判断范围
	int startPos;
	CString content;
	switch (param.m_range)
	{
	default: return FALSE;
	case KeywordRange::LZL_CONTENT:     startPos = 0;                                    content = lzl.content;          break;
	case KeywordRange::AUTHOR:          startPos = lzl.GetContent().GetLength() + 7;     content = lzl.author;           break;
	case KeywordRange::ALL_CONTENT:     startPos = 0;                                    content = lzl.GetContent();     break;
	}

	return MatchContent(param, content, startPos, pos, length);
}


// 等级条件

CString CLevelCondition::GetDescription(const CConditionParam& _param)
{
	const auto& param = (CLevelParam&)_param;
	static LPCTSTR operatorDesc[] = {
		_T("等级 <= "),
		_T("等级 >= ")
	};

	CString res;
	res.Format(_T("%s%d"), operatorDesc[param.m_operator], param.m_level);
	return res;
}


CConditionParam* CLevelCondition::ReadParam(const tinyxml2::XMLElement* optionNode)
{
	auto* param = new CLevelParam();

	param->m_conditionName = m_name;
	COption<int> op("Operator");
	COption<int> level("Level");
	op.Read(*optionNode);
	level.Read(*optionNode);

	param->m_operator = (0 <= op && op <= LevelOperator::GREATER) ? LevelOperator(*op) : LevelOperator::LESS;
	param->m_level = level;

	return param;
}

void CLevelCondition::WriteParam(const CConditionParam& _param, tinyxml2::XMLElement* optionNode)
{
	const auto& param = (CLevelParam&)_param;

	COption<int> op("Operator");
	*op = param.m_operator;
	op.Write(*optionNode);
	COption<int> level("Level");
	*level = param.m_level;
	level.Write(*optionNode);
}


BOOL CLevelCondition::MatchThread(const CConditionParam& _param, const ThreadInfo& thread, int& pos, int& length)
{
	return FALSE;
}

BOOL CLevelCondition::MatchPost(const CConditionParam& _param, const PostInfo& post, int& pos, int& length)
{
	const auto& param = (CLevelParam&)_param;

	int level = _ttoi(post.authorLevel);
	switch (param.m_operator)
	{
	default: return FALSE;
	case LevelOperator::LESS:       return level <= param.m_level;
	case LevelOperator::GREATER:    return level >= param.m_level;
	}
}

BOOL CLevelCondition::MatchLzl(const CConditionParam& _param, const LzlInfo& lzl, int& pos, int& length)
{
	return FALSE;
}
