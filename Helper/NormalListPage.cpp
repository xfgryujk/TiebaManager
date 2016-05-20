/*
Copyright (C) 2015  xfgryujk
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

// NormalListPage.cpp : 实现文件
//

#include "stdafx.h"
#include <NormalListPage.h>
#include "resource.h"
#include <ConfigFile.h>
#include <InputDlg.h>


// CNormalListPage 对话框

IMPLEMENT_DYNAMIC(CNormalListPage, CListPage)

CNormalListPage::CNormalListPage(const CString& inputTitle, CWnd* pParent /*=NULL*/) : CListPage(IDD_LIST_PAGE, pParent),
	m_inputTitle(inputTitle)
{
}

CNormalListPage::CNormalListPage(const CString& inputTitle, UINT nIDTemplate, CWnd* pParentWnd /*=NULL*/) : CListPage(nIDTemplate, pParentWnd),
	m_inputTitle(inputTitle)
{
}

CNormalListPage::~CNormalListPage()
{
}

void CNormalListPage::DoDataExchange(CDataExchange* pDX)
{
	CListPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNormalListPage, CListPage)
END_MESSAGE_MAP()


// CNormalListPage 消息处理程序


BOOL CNormalListPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_list.ModifyStyle(0, LVS_NOCOLUMNHEADER);
	m_list.InsertColumn(0, _T(""), LVCFMT_LEFT, 500);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

class CNormalListFile : public CConfigBase
{
public:
	COption<vector<CString> > m_list;

	CNormalListFile()
		: CConfigBase("NormalList"),
		m_list("NormalList")
	{
		m_options.push_back(&m_list);
	}
};

// 导出xml
BOOL CNormalListPage::Export(const CString& path)
{
	if (path.Right(4).CompareNoCase(_T(".xml")) != 0)
		return CListPage::Export(path);

	CNormalListFile tmp;
	ApplyList(tmp.m_list);
	return tmp.Save(path);
}

// 导入xml
BOOL CNormalListPage::Import(const CString& path)
{
	if (path.Right(4).CompareNoCase(_T(".xml")) != 0)
		return CListPage::Import(path);

	CNormalListFile tmp;
	if (!tmp.Load(path))
		return FALSE;
	ShowList(tmp.m_list);
	return TRUE;
}

BOOL CNormalListPage::SetItem(int index)
{
	CString content = m_list.GetItemText(index, 0);
	CInputDlg dlg(m_inputTitle, content, NULL, FALSE, CInputDlg::IDD, this);
	if (dlg.DoModal() == IDOK && content != _T(""))
	{
		m_list.SetItemText(index, 0, content);
		return TRUE;
	}
	return FALSE;
}

void CNormalListPage::ShowList(const vector<CString>& list)
{
	m_list.DeleteAllItems();
	for (UINT i = 0; i < list.size(); i++)
		m_list.InsertItem(i, list[i]);
}

void CNormalListPage::ShowList(const set<CString>& list)
{
	m_list.DeleteAllItems();
	int i = 0;
	for (const CString& item : list)
		m_list.InsertItem(i++, item);
}

void CNormalListPage::ApplyList(vector<CString>& list)
{
	int size = m_list.GetItemCount();
	list.resize(size);
	for (int i = 0; i < size; i++)
		list[i] = m_list.GetItemText(i, 0);
}

void CNormalListPage::ApplyList(set<CString>& list)
{
	list.clear();
	CString tmp;
	int size = m_list.GetItemCount();
	for (int i = 0; i < size; i++)
	{
		tmp = m_list.GetItemText(i, 0);
		list.insert(tmp);
	}
}
