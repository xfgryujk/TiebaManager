#pragma once
#include "afxwin.h"
#include <vector>
using std::vector;


// CImageViewDlg 对话框

class CImageViewDlg : public CDialog
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
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnClose();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();

	void SetImages(vector<CString>* imageURL);
	void SetCurImage(int index);


public:
	CStatic m_imageStatic;
	CButton m_prevButton;
	CButton m_nextButton;
	CButton m_saveButton;

protected:
	CImageViewDlg** m_pThis;

	vector<CString>* m_imageURL;
	int m_curImageIndex;
	CImage m_curImage;
};
