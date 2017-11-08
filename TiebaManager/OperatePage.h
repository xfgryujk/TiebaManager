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
#include "afxwin.h"
#include <NormalDlg.h>
#include "resource.h"


// COperatePage 对话框

class COperatePage : public CNormalDlg
{
	DECLARE_DYNAMIC(COperatePage)

public:
	COperatePage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COperatePage();

// 对话框数据
	enum { IDD = IDD_OPERATE_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnEnKillfocusEdit3();
	afx_msg void OnEnKillfocusEdit4();
	afx_msg void OnEnKillfocusEdit8();
	afx_msg void OnBnClickedCheck3();


public:
	CButton m_deleteCheck;
	CButton m_banIDCheck;
	CButton m_defriendCheck;
	CComboBox m_banDurationCombo;
	CEdit m_banReasonEdit;
	CEdit m_banTrigCountEdit;
	CEdit m_defriendTrigCountEdit;
	CEdit m_deleteIntervalEdit;
	CButton m_confirmCheck;
	CButton m_banClientInterfaceCheck;
};
