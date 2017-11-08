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
#include "resource.h"
#include <TBMCoreRule.h>


// CInputRuleDlg 对话框

template<class RuleType>
class CInputRuleDlg : public CNormalListPage
{
public:
	CInputRuleDlg(RuleType& rule, UINT nIDTemplate, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInputRuleDlg();

	// 对话框数据
	enum { IDD = IDD_INPUT_RULE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnClose();

	virtual BOOL SetItem(int index) override;
	virtual void OnAdd(int index) override;
	virtual void OnDelete(int index) override;
	virtual BOOL Export(const CString& path) override;
	virtual BOOL Import(const CString& path) override;
	virtual void ShowList(const RuleType& list);
	virtual void ShowList(RuleType&& list);


public:
	CEdit m_ruleNameEdit;
	CButton m_okButton;
	CButton m_cancelButton;

protected:
	class CRuleFile : public CConfigBase
	{
	public:
		COption<RuleType> m_list;

		CRuleFile() : CConfigBase("Rule"),
			m_list("Rule")
		{
			m_options.push_back(&m_list);
		}
	};

	RuleType& m_rule;
	RuleType m_ruleCopy;
};
