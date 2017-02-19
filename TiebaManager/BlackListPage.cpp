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

// BlackListPage.cpp : 实现文件
//

#include "stdafx.h"
#include "BlackListPage.h"
#include "SettingDlg.h"

#include <StringHelper.h>

#include "TBMConfig.h"


// CBlackListPage 对话框

IMPLEMENT_DYNAMIC(CBlackListPage, CRegListPage)

CBlackListPage::CBlackListPage(CWnd* pParent /*=NULL*/)
	: CRegListPage(_T("用户名："), pParent)
{

}

CBlackListPage::~CBlackListPage()
{
}

void CBlackListPage::DoDataExchange(CDataExchange* pDX)
{
	CRegListPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBlackListPage, CRegListPage)
END_MESSAGE_MAP()


// CBlackListPage 消息处理程序


BOOL CBlackListPage::OnInitDialog()
{
	CRegListPage::OnInitDialog();

	m_static.SetWindowText(_T("当被测文本等于文本时匹配\r\n")
						   _T("注意正则表达式.*匹配所有文本，请不要不加限制使用！！"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

BOOL CBlackListPage::CheckMatchTooMuch(const CString& text, BOOL isRegex)
{
	return StringMatchs(MATCH_TOO_MUCH_USERNAME_TEST1, text, isRegex) 
		&& StringMatchs(MATCH_TOO_MUCH_USERNAME_TEST2, text, isRegex);
}

void CBlackListPage::OnAdd(int index)
{
	((CSettingDlg*)GetParent())->m_clearScanCache = TRUE;
}

BOOL CBlackListPage::TestMatch(const CString& test, const CString& text, BOOL isRegex)
{
	return StringMatchs(test, text, isRegex);
}
