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

// AboutPage.cpp : 实现文件
//

#include "stdafx.h"
#include "AboutPage.h"

#include <Update.h>


// CAboutPage 对话框

IMPLEMENT_DYNAMIC(CAboutPage, CNormalDlg)

CAboutPage::CAboutPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CAboutPage::IDD, pParent)
{

}

#pragma region MFC
CAboutPage::~CAboutPage()
{
}

void CAboutPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_STATIC1, m_checkUpdateStatic);
	DDX_Control(pDX, IDC_CHECK1, m_autoCheckUpdateCheck);
	DDX_Control(pDX, IDC_STATIC2, m_authorStatic);
}


BEGIN_MESSAGE_MAP(CAboutPage, CNormalDlg)
	ON_STN_CLICKED(IDC_STATIC1, &CAboutPage::OnStnClickedStatic1)
	ON_STN_CLICKED(IDC_STATIC2, &CAboutPage::OnStnClickedStatic2)
END_MESSAGE_MAP()
#pragma endregion

// CAboutPage 消息处理程序

// 初始化
BOOL CAboutPage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	m_resize.AddControl(&m_edit, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_checkUpdateStatic, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_edit);
	m_resize.AddControl(&m_autoCheckUpdateCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_edit);
	m_resize.AddControl(&m_authorStatic, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_edit);

	m_edit.SetWindowText(_T("软件介绍帖：http://tieba.baidu.com/p/4577833688\r\n")
		                 _T("详细教程：https://github.com/xfgryujk/TiebaManager/wiki\r\n")
						 _T("对更新日志感兴趣的话看这里：https://github.com/xfgryujk/TiebaManager/commits/master\r\n")
						 _T("此程序通过GPLv2协议开源，你可以在https://github.com/xfgryujk/TiebaManager得到源码，求star，欢迎发pull request\r\n")
						 _T("\r\n")
						 _T("html代码只转换了\"'&<>还有空格，其他以网页源码为准，如换行符为<br> (不注意的话会不匹配或匹配太多)\r\n")
						 _T("\r\n")
						 _T("我的邮箱和支付宝是xfgryujk@126.com，喜欢此软件可以捐助哦\r\n")
						 _T("\r\n")
						 _T("我只是提供方便吧务管理贴吧的工具，被用来做拆吧等恶意行为造成的损失本人不负责"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 检查更新
void CAboutPage::OnStnClickedStatic1()
{
	switch (CheckUpdate())
	{
	case UPDATE_NO_UPDATE:
		AfxMessageBox(_T("已经是最新版本"));
		break;
	case UPDATE_FAILED_TO_GET_INFO:
		if (AfxMessageBox(_T("获取文件信息失败，手动更新？"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			ManuallyUpdate();
		break;
	}
}

// 关于作者
void CAboutPage::OnStnClickedStatic2()
{
	ShellExecute(NULL, _T("open"), _T("http://tieba.baidu.com/i/37897560"), NULL, NULL, SW_SHOWNORMAL);
}
