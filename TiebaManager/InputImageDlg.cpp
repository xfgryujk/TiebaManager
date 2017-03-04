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

// InputImageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "InputImageDlg.h"
#include <ImageHelper.h>


// CInputImageDlg 对话框

IMPLEMENT_DYNAMIC(CInputImageDlg, CDialog)

CInputImageDlg::CInputImageDlg(CImageParam* param, CWnd* pParent /*=NULL*/) :
	CDialog(CInputImageDlg::IDD, pParent), 
	m_param(param)
{
}

CInputImageDlg::~CInputImageDlg()
{
}

void CInputImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_imageEdit);
	DDX_Control(pDX, IDC_COMBO1, m_algorithmCombo);
	DDX_Control(pDX, IDC_CHECK3, m_ignoreSizeCheck);
	DDX_Control(pDX, IDC_EDIT5, m_thresholdEdit);
	DDX_Control(pDX, IDC_EDIT2, m_testImageEdit);
}


BEGIN_MESSAGE_MAP(CInputImageDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, &CInputImageDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CInputImageDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON7, &CInputImageDlg::OnBnClickedButton7)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CInputImageDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CInputImageDlg 消息处理程序

BOOL CInputImageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_imageEdit.SetWindowText(m_param->m_imagePath);
	m_algorithmCombo.SetCurSel(m_param->m_algorithm);
	OnCbnSelchangeCombo1();
	m_ignoreSizeCheck.SetCheck(m_param->m_ignoreSize);
	CString tmp;
	tmp.Format(_T("%.5lf"), m_param->m_threshold);
	m_thresholdEdit.SetWindowText(tmp);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CInputImageDlg::OnOK()
{
	ApplyImageParam(*m_param);

	CDialog::OnOK();
}

void CInputImageDlg::OnCbnSelchangeCombo1()
{
	int sel = m_algorithmCombo.GetCurSel();
	if (sel == CImageParam::EQUAL)
	{
		m_ignoreSizeCheck.EnableWindow(FALSE);
		m_thresholdEdit.EnableWindow(FALSE);
	}
	else
	{
		m_ignoreSizeCheck.EnableWindow(sel == CImageParam::PSNR || sel == CImageParam::SSIM);
		m_thresholdEdit.EnableWindow(TRUE);
	}
}

// 浏览图片
void CInputImageDlg::OnBnClickedButton2()
{
	BrowseImage(m_imageEdit);
}

// 浏览测试图片
void CInputImageDlg::OnBnClickedButton4()
{
	BrowseImage(m_testImageEdit);
}

void CInputImageDlg::BrowseImage(CEdit& edit)
{
	CFileDialog dlg(TRUE, _T("jpg"), NULL, 0,
		_T("图片文件 (*.jpg;*.png;*.bmp;*.gif)|*.jpg;*.png;*.bmp;*.gif|所有文件 (*.*)|*.*||"), this);
	if (dlg.DoModal() == IDOK)
		edit.SetWindowText(dlg.GetPathName());
}

// 测试匹配
void CInputImageDlg::OnBnClickedButton7()
{
	CString testImagePath;
	m_testImageEdit.GetWindowText(testImagePath);
	cv::Mat testImage;
	if (!ReadImage(testImagePath, testImage))
	{
		AfxMessageBox(_T("加载测试图片失败！"), MB_ICONERROR);
		return;
	}

	CImageParam param;
	ApplyImageParam(param);
	if (param.m_image.empty())
		return;

	double res = CImageCondition::GetInstance().CompareImage(param, testImage);
	if (res < 0.0)
	{
		if (-2.00001 < res && res < -1.99999)
			AfxMessageBox(_T("图片尺寸太小！"));
		return;
	}

	CString msg;
	if (param.m_algorithm == CImageParam::EQUAL)
		msg = res > 0.99 ? _T("图片相同，匹配成功") : _T("图片不相同，匹配失败");
	else
	{
		BOOL succeeded;
		if (param.m_algorithm == CImageParam::PSNR || param.m_algorithm == CImageParam::SSIM)
			succeeded = res > param.m_threshold;
		else
			succeeded = res < param.m_threshold;
		msg.Format(_T("比较结果：%.5lf，匹配%s"), res, succeeded ? _T("成功") : _T("失败"));
	}
	AfxMessageBox(msg);
}

void CInputImageDlg::ApplyImageParam(CImageParam& param)
{
	m_imageEdit.GetWindowText(param.m_imagePath);
	param.m_algorithm = CImageParam::Algorithm(m_algorithmCombo.GetCurSel());
	param.m_ignoreSize = m_ignoreSizeCheck.GetCheck();
	CString tmp;
	m_thresholdEdit.GetWindowText(tmp);
	param.m_threshold = _ttof(tmp);

	CImageCondition::GetInstance().UpdateImage(param);
}
