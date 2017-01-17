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
#include "afxcmn.h"
#include "afxwin.h"
#include <ModelessDlg.h>
#include "resource.h"
#include <memory>
#include <vector>

#include "ExploreThreadPage.h"
#include "ExplorePostPage.h"
#include "ExploreLzlPage.h"

#include "ImageViewDlg.h"


// CExplorerDlg 对话框

class CExplorerDlg : public CModelessDlg
{
	DECLARE_DYNAMIC(CExplorerDlg)

public:
	CExplorerDlg(CExplorerDlg*& pThis, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExplorerDlg();

// 对话框数据
	enum { IDD = IDD_EXPLORER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();

	void ViewImages(std::unique_ptr<std::vector<CString> > img);


public:
	CTabCtrl m_tab;
	std::unique_ptr<CExploreThreadPage> m_exploreThreadPage;
	std::unique_ptr<CExplorePostPage> m_explorePostPage;
	std::unique_ptr<CExploreLzlPage> m_exploreLzlPage;
	CEdit m_edit;
	CButton m_deleteButton;
	CButton m_banButton;
	CButton m_explorerButton;

	CImageViewDlg* m_imageViewDlg;

protected:
	CExplorerPage* m_pages[3];
	CResizeControl m_pagesResize;
};
