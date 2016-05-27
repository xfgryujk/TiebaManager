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

// NormalDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <NormalDlg.h>


// CNormalDlg 对话框

IMPLEMENT_DYNAMIC(CNormalDlg, CDialog)

CNormalDlg::CNormalDlg(UINT nIDTemplate, CWnd* pParentWnd) : CDialog(nIDTemplate, pParentWnd),
	m_resize(this)
{

}

#pragma region MFC
CNormalDlg::~CNormalDlg()
{
}

void CNormalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNormalDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_SIZE()
END_MESSAGE_MAP()
#pragma endregion

// CNormalDlg 消息处理程序

// 屏蔽Esc关闭窗口
void CNormalDlg::OnCancel()
{
}

// 屏蔽回车关闭窗口
void CNormalDlg::OnOK()
{
}

// 销毁窗口
void CNormalDlg::OnClose()
{
	DestroyWindow();
}

// 改变尺寸
void CNormalDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	m_resize.Resize();
}
