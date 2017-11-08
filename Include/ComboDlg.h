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
#include "HelperCommon.h"
#include "afxwin.h"
#include <vector>


// CComboDlg 对话框

class HELPER_API CComboDlg : public CDialog
{
	DECLARE_DYNAMIC(CComboDlg)

public:
	CComboDlg(const CString& title, const std::vector<CString>& list, int& select, UINT nIDTemplate = IDD, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CComboDlg();

// 对话框数据
	static const UINT IDD;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();


public:
	CComboBox m_combo;

	int& m_select;

protected:
	const CString m_title;
	const std::vector<CString>& m_list;
};
