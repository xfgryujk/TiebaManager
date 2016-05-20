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

// WhiteContentPage.cpp : 实现文件
//

#include "stdafx.h"
#include "WhiteContentPage.h"

#include <StringHelper.h>

#include "TBMConfig.h"


// CWhiteContentPage 对话框

IMPLEMENT_DYNAMIC(CWhiteContentPage, CRegListPage)

CWhiteContentPage::CWhiteContentPage(CWnd* pParent /*=NULL*/)
	: CRegListPage(_T("信任内容："), pParent)
{

}

CWhiteContentPage::~CWhiteContentPage()
{
}

void CWhiteContentPage::DoDataExchange(CDataExchange* pDX)
{
	CRegListPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWhiteContentPage, CRegListPage)
END_MESSAGE_MAP()


// CWhiteContentPage 消息处理程序


BOOL CWhiteContentPage::OnInitDialog()
{
	CRegListPage::OnInitDialog();

	m_static.SetWindowText(_T("支持图片地址\\视频地址，当被测文本含有文本时匹配"));
	m_static.ModifyStyle(0, SS_CENTERIMAGE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

BOOL CWhiteContentPage::CheckMatchTooMuch(const CString& text, BOOL isRegex)
{
	return StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST1, text, isRegex) 
		&& StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST2, text, isRegex);
}

BOOL CWhiteContentPage::TestMatch(const CString& test, const CString& text, BOOL isRegex)
{
	return StringIncludes(test, text, isRegex);
}
