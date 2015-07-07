#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "ExploreThreadPage.h"
#include "ExplorePostPage.h"
#include "ExploreLzlPage.h"
#include "ImageViewDlg.h"


// CExplorerDlg 对话框

class CExplorerDlg : public CDialog
{
	DECLARE_DYNAMIC(CExplorerDlg)

public:
	CExplorerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExplorerDlg();

// 对话框数据
	enum { IDD = IDD_EXPLORER_DIALOG };

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
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();

	void ViewImages(vector<CString>* img);


public:
	CTabCtrl m_tab;
	CExploreThreadPage m_exploreThreadPage;
	CExplorePostPage m_explorePostPage;
	CExploreLzlPage m_exploreLzlPage;
	CEdit m_edit;
	CButton m_deleteButton;
	CButton m_banButton;
	CButton m_explorerButton;

	CImageViewDlg* m_imageViewDlg;

protected:
	CExplorerPage* m_pages[3];
};
