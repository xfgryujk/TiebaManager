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

#pragma once
#include <NormalListPage.h>
#include <TBMCoreRule.h>


template<class RuleType>
class CRulesPage : public CNormalListPage
{
public:
	CRulesPage(CWnd* pParent = NULL);   // 标准构造函数
	CRulesPage(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CRulesPage() = default;

	virtual BOOL SetItem(int index) override;
	virtual void OnAdd(int index) override;
	virtual void OnDelete(int index) override;
	virtual BOOL Export(const CString& path) override;
	virtual BOOL Import(const CString& path) override;
	virtual void ShowList(const std::vector<RuleType>& list);
	virtual void ShowList(std::vector<RuleType>&& list);
	virtual void ApplyList(std::vector<RuleType>& list);
	// 在这里更新列表中显示的内容
	virtual void OnUpdateRule(int index) { }

protected:
	class CRuleListFile : public CConfigBase
	{
	public:
		COption<std::vector<RuleType> > m_list;

		CRuleListFile() : CConfigBase("RuleList"),
			m_list("RuleList")
		{
			m_options.push_back(&m_list);
		}
	};

	std::vector<RuleType> m_rules;
};
