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
#include "afxwin.h"
#include <ModelessDlg.h>


// CImageViewDlg 对话框

class CImageViewDlg : public CModelessDlg
{
	DECLARE_DYNAMIC(CImageViewDlg)

public:
	CImageViewDlg(CImageViewDlg*& pThis, CWnd* pParent = NULL);   // 构造函数
	virtual ~CImageViewDlg();

// 对话框数据
	enum { IDD = IDD_IMAGE_VIEW_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void UpdateScrollRange();
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnBnClickedButton3();

	void SetImages(unique_ptr<vector<CString> > imageURL);
protected:
	SIZE GetImageSize(const CImage& image);


public:
	CStatic m_imageStatic;
	CScrollBar m_imageScrollBar;
	CButton m_saveButton;

protected:
	unique_ptr<vector<CString> > m_imageURL;
	vector<CImage> m_image;
};
