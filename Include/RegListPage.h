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

#pragma once
#include "HelperCommon.h"
#include "ListTestPage.h"
#include <StringHelper.h>


// CRegListPage 对话框

class HELPER_API CRegListPage : public CListTestPage
{
	DECLARE_DYNAMIC(CRegListPage)

public:
	CRegListPage(const CString& inputTitle, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRegListPage();

	// 对话框数据

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	virtual BOOL SetItem(int index);
	virtual BOOL Export(const CString& path);
	virtual BOOL Import(const CString& path);
	virtual void ShowList(const std::vector<RegexText>& list);
	virtual void ApplyList(std::vector<RegexText>& list);
protected:
	virtual BOOL CheckMatchTooMuch(const CString& text, BOOL isRegex){ return FALSE; };
	virtual BOOL TestMatch(int index);
	virtual BOOL TestMatch(const CString& test, const CString& text, BOOL isRegex) = 0;


protected:
	const CString m_inputTitle;
};

const TCHAR IS_REGEX_TEXT[] = _T("√");
