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

// ImagePage.cpp : 实现文件
//

#include "stdafx.h"
#include "ImagePage.h"
#include "SettingDlg.h"

#include "TBMConfig.h"
#include "TiebaManager.h"


// CImagePage 对话框

IMPLEMENT_DYNAMIC(CImagePage, CNormalDlg)

CImagePage::CImagePage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CImagePage::IDD, pParent)
{
	theApp.m_plan->m_updateImage = FALSE;
}

#pragma region MFC
CImagePage::~CImagePage()
{
}

void CImagePage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_dirEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_browseButton);
	DDX_Control(pDX, IDC_BUTTON2, m_updateButton);
	DDX_Control(pDX, IDC_EDIT5, m_thresholdEdit);
	DDX_Control(pDX, IDC_STATIC1, m_static);
}


BEGIN_MESSAGE_MAP(CImagePage, CNormalDlg)
	ON_BN_CLICKED(IDC_BUTTON1, &CImagePage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CImagePage::OnBnClickedButton2)
	ON_EN_KILLFOCUS(IDC_EDIT5, &CImagePage::OnEnKillfocusEdit5)
END_MESSAGE_MAP()
#pragma endregion

// CImagePage 消息处理程序

// 初始化
BOOL CImagePage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	m_resize.AddControl(&m_dirEdit, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_browseButton, RT_KEEP_DIST_TO_RIGHT, &m_dirEdit, RT_NULL, NULL);
	m_resize.AddControl(&m_static, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 浏览
void CImagePage::OnBnClickedButton1()
{
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.lpszTitle = _T("违规图片目录：");
	bi.ulFlags = BIF_STATUSTEXT;
	LPITEMIDLIST pidlSel = SHBrowseForFolder(&bi);

	if (pidlSel != NULL)
	{
		CString folder;
		SHGetPathFromIDList(pidlSel, folder.GetBuffer(MAX_PATH));
		folder.ReleaseBuffer();
		m_dirEdit.SetWindowText(folder);
	}
}

// 更新图片
void CImagePage::OnBnClickedButton2()
{
	theApp.m_plan->m_updateImage = TRUE;
	((CSettingDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

// 阈值
void CImagePage::OnEnKillfocusEdit5()
{
	CString tmp;
	m_thresholdEdit.GetWindowText(tmp);
	double threshold = _ttof(tmp);
	if (threshold < 1.0 || threshold > 3.0)
		m_thresholdEdit.SetWindowText(_T("2.43"));

	if (abs(threshold - theApp.m_plan->m_SSIMThreshold) > 0.00001)
	{
		theApp.m_plan->m_updateImage = TRUE;
		if (threshold < theApp.m_plan->m_SSIMThreshold)
			((CSettingDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
	}
}
