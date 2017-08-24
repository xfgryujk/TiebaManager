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

// RegListPage.cpp : 实现文件
//

#include "stdafx.h"
#include <RegListPage.h>
#include "resource.h"
#include <ConfigFile.h>
#include <InputDlg.h>


// CRegListPage 对话框

IMPLEMENT_DYNAMIC(CRegListPage, CListTestPage)

CRegListPage::CRegListPage(const CString& inputTitle, CWnd* pParent /*=NULL*/) : CListTestPage(pParent), 
	m_inputTitle(inputTitle)
{
}

CRegListPage::~CRegListPage()
{
}

void CRegListPage::DoDataExchange(CDataExchange* pDX)
{
	CListTestPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRegListPage, CListTestPage)
END_MESSAGE_MAP()


// CRegListPage 消息处理程序


BOOL CRegListPage::OnInitDialog()
{
	CListTestPage::OnInitDialog();

	int i = 0;
	m_list.InsertColumn(i++, _T("内容"), LVCFMT_LEFT, 450);
	m_list.InsertColumn(i++, _T("正则"), LVCFMT_LEFT, 50);
	m_list.InsertColumn(i++, _T("忽略大小写"), LVCFMT_LEFT, 50);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

class CRegListFile : public CConfigBase
{
public:
	COption<std::vector<RegexText> > m_list;

	CRegListFile()
		: CConfigBase("RegList"),
		m_list("RegList")
	{
		m_options.push_back(&m_list);
	}
};

// 导出xml
BOOL CRegListPage::Export(const CString& path)
{
	if (path.Right(4).CompareNoCase(_T(".xml")) != 0)
		return CListPage::Export(path);

	CRegListFile tmp;
	ApplyList(tmp.m_list);
	return tmp.Save(path);
}

// 导入xml
BOOL CRegListPage::Import(const CString& path)
{
	if (path.Right(4).CompareNoCase(_T(".xml")) != 0)
		return CListPage::Import(path);

	CRegListFile tmp;
	if (!tmp.Load(path))
		return FALSE;
	ShowList(tmp.m_list);
	return TRUE;
}

BOOL CRegListPage::SetItem(int index)
{
	CString content = m_list.GetItemText(index, 0);
	BOOL isRegex = m_list.GetItemText(index, 1) == IS_REGEX_TEXT;
	CInputDlg dlg(m_inputTitle, content, &isRegex, TRUE, CInputDlg::IDD, this);
	if (dlg.DoModal() == IDOK && content != _T(""))
	{
		if (CheckMatchTooMuch(content, isRegex))
		{
			AfxMessageBox(_T("匹配太多了！"), MB_ICONERROR);
			return FALSE;
		}
		m_list.SetItemText(index, 0, content);
		m_list.SetItemText(index, 1, isRegex ? IS_REGEX_TEXT : _T(""));
		return TRUE;
	}
	return FALSE;
}

BOOL CRegListPage::TestMatch(int index)
{
	CString test;
	m_testEdit.GetWindowText(test);
	CString text = m_list.GetItemText(index, 0);
	BOOL isRegex = m_list.GetItemText(index, 1) == IS_REGEX_TEXT;
	BOOL ignoreCase = m_list.GetItemText(index, 2) == IS_REGEX_TEXT;
	return TestMatch(test, text, isRegex, ignoreCase);
}

void CRegListPage::ShowList(const std::vector<RegexText>& list)
{
	m_list.DeleteAllItems();
	for (UINT i = 0; i < list.size(); i++)
	{
		m_list.InsertItem(i, list[i].text);
		m_list.SetItemText(i, 1, list[i].isRegex ? IS_REGEX_TEXT : _T(""));
	}
}

void CRegListPage::ApplyList(std::vector<RegexText>& list)
{
	int size = m_list.GetItemCount();
	list.resize(size);
	for (int i = 0; i < size; i++)
		list[i].Set(m_list.GetItemText(i, 0), m_list.GetItemText(i, 1) == IS_REGEX_TEXT, m_list.GetItemText(i, 2) == IS_REGEX_TEXT);
}
