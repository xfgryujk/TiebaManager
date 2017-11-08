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

// PluginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PluginDlg.h"

#include "PluginManager.h"


// CPluginDlg 对话框

IMPLEMENT_DYNAMIC(CPluginDlg, CModelessDlg)

CPluginDlg::CPluginDlg(CPluginDlg*& pThis, CWnd* pParent /*=NULL*/) : 
	CModelessDlg(CPluginDlg::IDD, (CModelessDlg**)&pThis, pParent)
{
}

#pragma region MFC
CPluginDlg::~CPluginDlg()
{
}

void CPluginDlg::DoDataExchange(CDataExchange* pDX)
{
	CModelessDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_BUTTON1, m_configButton);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
}


BEGIN_MESSAGE_MAP(CPluginDlg, CModelessDlg)
	ON_LBN_SELCHANGE(IDC_LIST1, &CPluginDlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CPluginDlg::OnBnClickedButton1)
END_MESSAGE_MAP()
#pragma endregion

// CPluginDlg 消息处理程序

// 初始化
BOOL CPluginDlg::OnInitDialog()
{
	CModelessDlg::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// 设置大图标
	SetIcon(hIcon, FALSE);			// 设置小图标

	m_resize.AddControl(&m_list, RT_NULL, NULL, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_configButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_edit, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);

	// 显示插件
	for (const auto& i : CPluginManager::GetInstance().GetPlugins())
		m_list.AddString(i.m_name);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 选中
void CPluginDlg::OnLbnSelchangeList1()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	auto& plugin = CPluginManager::GetInstance().GetPlugins()[index];
	m_edit.SetWindowText(plugin.m_description);
}

// 设置
void CPluginDlg::OnBnClickedButton1()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	auto& plugin = CPluginManager::GetInstance().GetPlugins()[index];
	if (plugin.m_onConfig)
		plugin.m_onConfig();
}
