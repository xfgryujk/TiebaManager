#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "PrefPage.h"
#include "KeywordsPage.h"
#include "BlackListPage.h"
#include "WhiteListPage.h"
#include "WhiteContentPage.h"
#include "OptionsPage.h"
#include "UsersPage.h"
#include "AboutPage.h"


// CSettingDlg 对话框

class CSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSettingDlg();

// 对话框数据
	enum { IDD = IDD_SETTING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	virtual void OnOK();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();

	void ShowCurrentOptions();
	void ApplyOptionsInDlg();
	void ShowOptionsInFile(LPCTSTR path);
	void SaveOptionsInDlg(LPCTSTR path);


public:
	CTabCtrl m_tab;
	CPrefPage m_prefPage;
	CKeywordsPage m_keywordsPage;
	CBlackListPage m_blackListPage;
	CWhiteListPage m_whiteListPage;
	CWhiteContentPage m_whiteContentPage;
	COptionsPage m_optionsPage;
	CUsersPage m_usersPage;
	CAboutPage m_aboutPage;
	CButton m_okButton;
	CButton m_cancelButton;

	BOOL m_clearScanCache;
protected:
	CDialog* m_pages[8];
};
