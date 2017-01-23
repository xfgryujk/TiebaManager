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

// ScanPage.cpp : 实现文件
//

#include "stdafx.h"
#include "ScanPage.h"
#include "SettingDlg.h"


// CScanPage 对话框

IMPLEMENT_DYNAMIC(CScanPage, CNormalDlg)

CScanPage::CScanPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CScanPage::IDD, pParent)
{

}

#pragma region MFC
CScanPage::~CScanPage()
{
}

void CScanPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_scanIntervalEdit);
	DDX_Control(pDX, IDC_CHECK3, m_onlyScanTitleCheck);
	DDX_Control(pDX, IDC_EDIT5, m_scanPageCountEdit);
	DDX_Control(pDX, IDC_CHECK5, m_briefLogCheck);
	DDX_Control(pDX, IDC_EDIT2, m_threadCountEdit);
	DDX_Control(pDX, IDC_CHECK6, m_autoSaveLogCheck);
	DDX_Control(pDX, IDC_EDIT7, m_illegalLevelEdit);
	DDX_Control(pDX, IDC_CHECK7, m_clawerClientInterfaceCheck);
}


BEGIN_MESSAGE_MAP(CScanPage, CNormalDlg)
	ON_EN_KILLFOCUS(IDC_EDIT1, &CScanPage::OnEnKillfocusEdit1)
	ON_EN_KILLFOCUS(IDC_EDIT5, &CScanPage::OnEnKillfocusEdit5)
	ON_EN_CHANGE(IDC_EDIT5, &CScanPage::OnEnChangeEdit5)
	ON_EN_KILLFOCUS(IDC_EDIT2, &CScanPage::OnEnKillfocusEdit2)
	ON_EN_KILLFOCUS(IDC_EDIT7, &CScanPage::OnEnKillfocusEdit7)
END_MESSAGE_MAP()
#pragma endregion

// CScanPage 消息处理程序

// 扫描间隔
void CScanPage::OnEnKillfocusEdit1()
{
	CString tmp;
	m_scanIntervalEdit.GetWindowText(tmp);
	int scanInterval = _ttoi(tmp);
	if (scanInterval < 0 || scanInterval > 600)
		m_scanIntervalEdit.SetWindowText(_T("5"));
}

// 扫描最后页数 失去焦点
void CScanPage::OnEnKillfocusEdit5()
{
	CString tmp;
	m_scanPageCountEdit.GetWindowText(tmp);
	int scanPageCount = _ttoi(tmp);
	if (scanPageCount < 1)
		m_scanPageCountEdit.SetWindowText(_T("1"));
}

// 扫描最后页数 内容改变
void CScanPage::OnEnChangeEdit5()
{
	((CSettingDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

// 线程数
void CScanPage::OnEnKillfocusEdit2()
{
	CString tmp;
	m_threadCountEdit.GetWindowText(tmp);
	int threadCount = _ttoi(tmp);
	if (threadCount < 1 || threadCount > 16)
		m_threadCountEdit.SetWindowText(_T("2"));
}

// 违规等级
void CScanPage::OnEnKillfocusEdit7()
{
	CString tmp;
	m_illegalLevelEdit.GetWindowText(tmp);
	int illegalLevel = _ttoi(tmp);
	if (illegalLevel < 0 || illegalLevel > 6)
		m_illegalLevelEdit.SetWindowText(_T("0"));
}
