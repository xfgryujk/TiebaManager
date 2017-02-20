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
#include "TBMCoreRule.h"
#include "Singleton.h"
#include "StringHelper.h"


// 规则

class CIllegalRule final : public CRule
{
public:
	BOOL m_forceToConfirm = FALSE;	          // 强制确认
	int m_trigCount = 0;    			      // 触发次数
};

class CTrustedRule final : public CRule
{
public:
	int m_trigCount = 0;    			      // 触发次数
};


// 条件

enum KeywordRange
{
	TITLE,            // 主题标题
	PREVIEW,          // 主题预览
	POST_CONTENT,     // 帖子内容
	LZL_CONTENT,      // 楼中楼内容
	AUTHOR,           // 作者名
	ALL_CONTENT       // 所有内容（包括主题标题、主题预览、帖子内容、楼中楼内容）
};

class CKeywordParam : public CConditionParam
{
public:
	KeywordRange m_range;      // 搜索范围
	BOOL m_not;                // 结果取FALSE
	BOOL m_include;            // TRUE为包含，FALSE为匹配
	RegexText m_keyword;       // 关键词
};

class CKeywordCondition : public CCondition, public Singleton<CKeywordCondition>
{
	DECL_SINGLETON_DEFAULT(CKeywordCondition);
public:
	virtual CString GetDescription(const CConditionParam& param) override;

	virtual CConditionParam* ReadParam(const tinyxml2::XMLElement* optionNode) override;
	virtual void WriteParam(const CConditionParam& param, tinyxml2::XMLElement* optionNode) override;

	virtual BOOL MatchThread(const CConditionParam& param, const ThreadInfo& thread, int& pos, int& length) override;
	virtual BOOL MatchPost(const CConditionParam& param, const PostInfo& post, int& pos, int& length) override;
	virtual BOOL MatchLzl(const CConditionParam& param, const LzlInfo& lzl, int& pos, int& length) override;
};
