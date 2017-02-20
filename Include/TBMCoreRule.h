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

#pragma once
#include "TiebaManagerCoreCommon.h"
#include "ConfigFile.h"
#include "TiebaClawer.h"

#include <memory>
#include <vector>
#include <map>


class CCondition;


// 条件参数
class TBM_CORE_API CConditionParam
{
public:
	CString m_conditionName;       // 条件ID


	virtual ~CConditionParam() = default;

	CCondition& GetCondition();
	CString GetDescription();

	BOOL MatchThread(const ThreadInfo& thread, int& pos, int& length);
	BOOL MatchPost(const PostInfo& post, int& pos, int& length);
	BOOL MatchLzl(const LzlInfo& lzl, int& pos, int& length);
};

// 条件，享元模式
class TBM_CORE_API CCondition
{
private:
	static std::map<CString, CCondition*> s_conditions;
public:
	static CCondition s_defaultCondition;
	static CCondition& GetCondition(const CString& name);
	static BOOL AddCondition(CCondition& condition);


	const CString m_name;             // 作为ID，不能冲突


	CCondition(const CString& name);
	virtual ~CCondition() = default;

	virtual CString GetDescription(const CConditionParam& param);

	virtual CConditionParam* ReadParam(const tinyxml2::XMLElement* optionNode);
	virtual void WriteParam(const CConditionParam& param, tinyxml2::XMLElement* optionNode);

	virtual BOOL MatchThread(const CConditionParam& param, const ThreadInfo& thread, int& pos, int& length);
	virtual BOOL MatchPost(const CConditionParam& param, const PostInfo& post, int& pos, int& length);
	virtual BOOL MatchLzl(const CConditionParam& param, const LzlInfo& lzl, int& pos, int& length);
};

// 一个规则包含多个条件
class TBM_CORE_API CRule
{
public:
	CString m_name;
	
	std::vector<std::shared_ptr<CConditionParam> > m_conditionParams;      // 条件参数


	virtual ~CRule() = default;

	BOOL Match(const ThreadInfo& thread, int& pos, int& length);
	BOOL Match(const PostInfo& post, int& pos, int& length);
	BOOL Match(const LzlInfo& lzl, int& pos, int& length);
};
