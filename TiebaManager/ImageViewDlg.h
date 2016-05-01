#pragma once
#include "afxwin.h"
#include "NormalDlg.h"


// CImageViewDlg 对话框

class CImageViewDlg : public CNormalDlg
{
	DECLARE_DYNAMIC(CImageViewDlg)

public:
	CImageViewDlg(CImageViewDlg** pThis, CWnd* pParent = NULL);   // 构造函数
	virtual ~CImageViewDlg();

// 对话框数据
	enum { IDD = IDD_IMAGE_VIEW_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual void PostNcDestroy();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
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
	CImageViewDlg** m_pThis;

	unique_ptr<vector<CString> > m_imageURL;
	vector<CImage> m_image;
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
