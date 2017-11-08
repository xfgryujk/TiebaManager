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

// ListTestPage.cpp : 实现文件
//

#include "stdafx.h"
#include <ListTestPage.h>
#include "resource.h"


// CListTestPage 对话框

IMPLEMENT_DYNAMIC(CListTestPage, CListPage)

CListTestPage::CListTestPage(CWnd* pParent /*=NULL*/) : CListPage(IDD_LIST_TEST_PAGE, pParent)
{
}

#pragma region MFC
CListTestPage::~CListTestPage()
{
}

void CListTestPage::DoDataExchange(CDataExchange* pDX)
{
	CListPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_testEdit);
	DDX_Control(pDX, IDC_BUTTON7, m_testButton);
}


BEGIN_MESSAGE_MAP(CListTestPage, CListPage)
	ON_BN_CLICKED(IDC_BUTTON7, &CListTestPage::OnClickedButton7)
END_MESSAGE_MAP()
#pragma endregion

// CListTestPage 消息处理程序

// 初始化
BOOL CListTestPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_resize.AddControl(&m_testEdit, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_testButton, RT_KEEP_DIST_TO_RIGHT, &m_static, RT_KEEP_DIST_TO_BOTTOM, &m_testEdit);

	m_testEdit.SetWindowText(_T("欲测试文本"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 测试
void CListTestPage::OnClickedButton7()
{
	int index = m_list.GetSelectionMark();
	if (index == LB_ERR)
		return;
	if (TestMatch(index))
	{
		m_staticColor = RGB(0, 255, 0);
		m_static.SetWindowText(_T("匹配成功！"));
	}
	else
	{
		m_staticColor = RGB(255, 0, 0);
		m_static.SetWindowText(_T("匹配失败，请检查匹配文本！"));
	}
}
