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

// LoopBanPage.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "LoopBanDlg.h"
#include "LoopBan.h"

#include <TBMAPI.h>


// CLoopBanDlg 对话框

IMPLEMENT_DYNAMIC(CLoopBanDlg, CNormalListPage)

CLoopBanDlg::CLoopBanDlg(CLoopBanDlg*& pThis, CLoopBanConfig& config, CWnd* pParent /*=NULL*/) :
	CNormalListPage(_T("用户名："), IDD_LOOP_BAN_DLG, pParent),
	m_pThis(pThis),
	m_config(config)
{
}

#pragma region MFC
CLoopBanDlg::~CLoopBanDlg()
{
}

void CLoopBanDlg::DoDataExchange(CDataExchange* pDX)
{
	CNormalListPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK6, m_logCheck);
	DDX_Control(pDX, IDC_CHECK7, m_enableCheck);
	DDX_Control(pDX, IDC_EDIT9, m_banIntervalEdit);
	DDX_Control(pDX, IDC_STATIC2, m_static2);
	DDX_Control(pDX, IDC_STATIC3, m_static3);
	DDX_Control(pDX, IDC_CHECK8, m_autoLoopBanCheck);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
}


BEGIN_MESSAGE_MAP(CLoopBanDlg, CNormalListPage)
	ON_WM_CLOSE()
END_MESSAGE_MAP()
#pragma endregion

// CLoopBanDlg 消息处理程序

// 初始化
BOOL CLoopBanDlg::OnInitDialog()
{
	CNormalListPage::OnInitDialog();

	m_resize.AddControl(&m_enableCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_logCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_static2, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_banIntervalEdit, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_static3, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_autoLoopBanCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_okButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_cancelButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);

	CString currentUserDir = GetCurrentUserDir();

	// 显示配置
	CString tmp;
	ShowList(m_config.m_userList);							// 用户名
	m_pid = std::move(*m_config.m_pidList);					// PID
	m_logCheck.SetCheck(m_config.m_log);					// 输出日志
	m_enableCheck.SetCheck(m_config.m_enable);				// 开启
	tmp.Format(_T("%g"), *m_config.m_banInterval);
	m_banIntervalEdit.SetWindowText(tmp);					// 封禁间隔
	m_autoLoopBanCheck.SetCheck(m_config.m_autoLoopBan);	// 自动循环封

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

#pragma region UI
// 取消
void CLoopBanDlg::OnCancel()
{
	DestroyWindow();
}

// 提示是否保存
void CLoopBanDlg::OnClose()
{
	int result = AfxMessageBox(_T("保存设置？"), MB_ICONQUESTION | MB_YESNOCANCEL);
	if (result == IDYES)
	{
		OnOK();
		return;
	}
	else if (result == IDCANCEL)
		return;

	CNormalListPage::OnClose();
}

// 释放this
void CLoopBanDlg::PostNcDestroy()
{
	CNormalListPage::PostNcDestroy();

	m_pThis = NULL;
	delete this;
}
#pragma endregion

// 添加
void CLoopBanDlg::OnAdd(int index)
{
	if (index >= 0)
	{
		if (m_pid.size() != m_list.GetItemCount())
			m_pid.insert(m_pid.begin() + index, _T("")); // 优先不使用PID封禁
		else
			m_pid[index] = _T("");
	}
	else
	{
		m_pid.clear();
		m_pid.resize(m_list.GetItemCount());
	}
	m_clearCache = TRUE;
}

// 删除
void CLoopBanDlg::OnDelete(int index)
{
	if (index >= 0)
		m_pid.erase(m_pid.begin() + index);
	else
		m_pid.clear();
}

// 确认
void CLoopBanDlg::OnOK()
{
	CString currentUserDir = GetCurrentUserDir();

	CString strBuf;
	// 循环封
	ApplyList(m_config.m_userList);								// 用户名
	*m_config.m_pidList = std::move(m_pid);						// PID
	*m_config.m_log = m_logCheck.GetCheck();					// 输出日志
	*m_config.m_enable = m_enableCheck.GetCheck();				// 开启
	m_banIntervalEdit.GetWindowText(strBuf);
	*m_config.m_banInterval = (float)_ttof(strBuf);				// 封禁间隔
	*m_config.m_autoLoopBan = m_autoLoopBanCheck.GetCheck();	// 自动循环封

	m_config.Save(currentUserDir + _T("options2.xml"));

	if (m_clearCache)
		DeleteFile(currentUserDir + _T("LoopBanDate.xml"));

	DestroyWindow();
}
