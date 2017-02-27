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

// InputKeywordDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "InputKeywordDlg.h"
#include <StringHelper.h>


// CInputKeywordDlg 对话框

IMPLEMENT_DYNAMIC(CInputKeywordDlg, CDialog)

CInputKeywordDlg::CInputKeywordDlg(CKeywordParam* param, CWnd* pParent /*=NULL*/) : 
	CDialog(CInputKeywordDlg::IDD, pParent), 
	m_param(param)
{
}

CInputKeywordDlg::~CInputKeywordDlg()
{
}

void CInputKeywordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_rangeCombo);
	DDX_Control(pDX, IDC_CHECK1, m_notCheck);
	DDX_Control(pDX, IDC_COMBO2, m_includeCombo);
	DDX_Control(pDX, IDC_CHECK3, m_regexCheck);
	DDX_Control(pDX, IDC_EDIT1, m_contentEdit);
	DDX_Control(pDX, IDC_EDIT3, m_testEdit);
}


BEGIN_MESSAGE_MAP(CInputKeywordDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON7, &CInputKeywordDlg::OnBnClickedButton7)
END_MESSAGE_MAP()


// CInputKeywordDlg 消息处理程序

BOOL CInputKeywordDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_rangeCombo.SetCurSel(m_param->m_range);
	m_notCheck.SetCheck(m_param->m_not);
	m_includeCombo.SetCurSel(m_param->m_include ? 0 : 1);
	m_regexCheck.SetCheck(m_param->m_keyword.isRegex);
	m_contentEdit.SetWindowText(m_param->m_keyword.text);

	m_testEdit.SetWindowText(_T("欲测试文本"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CInputKeywordDlg::OnOK()
{
	m_param->m_range = KeywordRange(m_rangeCombo.GetCurSel());
	m_param->m_not = m_notCheck.GetCheck();
	m_param->m_include = m_includeCombo.GetCurSel() == 0;
	CString content;
	m_contentEdit.GetWindowText(content);
	m_param->m_keyword.Set(m_regexCheck.GetCheck(), content);

	CDialog::OnOK();
}

// 测试
void CInputKeywordDlg::OnBnClickedButton7()
{
	CString content, test;
	m_contentEdit.GetWindowText(content);
	m_testEdit.GetWindowText(test);

	BOOL res;
	if (m_includeCombo.GetCurSel() == 0)
		res = StringIncludes(test, content, m_regexCheck.GetCheck());
	else
		res = StringMatchs(test, content, m_regexCheck.GetCheck());

	AfxMessageBox(res ? _T("匹配成功") : _T("匹配失败"));
}

CConditionParam* CInputKeywordDlg::SetKeywordCondition(CConditionParam* _param)
{
	auto param = (CKeywordParam*)_param;
	BOOL release = FALSE;
	if (param == nullptr)
	{
		param = new CKeywordParam();
		release = TRUE;
	}

	CInputKeywordDlg dlg(param);
	if (dlg.DoModal() == IDOK)
		return param;

	if (release)
		delete param;
	return nullptr;
}
