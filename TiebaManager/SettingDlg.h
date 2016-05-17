#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "NormalDlg.h"
class ILog;
class CPlan;

class CScanPage;
class COperatePage;
class CKeywordsPage;
class CImagePage;
class CImagePage;
class CBlackListPage;
class CNormalListPage;
class CWhiteContentPage;
class COptionsPage;
class CUsersPage;
class CAboutPage;


// CSettingDlg 对话框

const int SETTING_DLG_PAGE_COUNT = 11;

class CSettingDlg : public CNormalDlg
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CSettingDlg*& pThis, ILog& log, CWnd* pParent = NULL);   // 标准构造函数
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

	void ShowPlan(const CPlan& plan);
	void ApplyPlanInDlg(CPlan& plan);
	void ShowPlanInFile(const CString& path);
	void SavePlanInDlg(const CString& path);


public:
	CTabCtrl m_tab;
	unique_ptr<CScanPage> m_scanPage;
	unique_ptr<COperatePage> m_operatePage;
	unique_ptr<CKeywordsPage> m_keywordsPage;
	unique_ptr<CImagePage> m_imagePage;
	unique_ptr<CBlackListPage> m_blackListPage;
	unique_ptr<CNormalListPage> m_whiteListPage;
	unique_ptr<CWhiteContentPage> m_whiteContentPage;
	unique_ptr<CNormalListPage> m_trustedThreadPage;
	unique_ptr<COptionsPage> m_optionsPage;
	unique_ptr<CUsersPage> m_usersPage;
	unique_ptr<CAboutPage> m_aboutPage;
	CButton m_okButton;
	CButton m_cancelButton;

	BOOL m_clearScanCache;
protected:
	CSettingDlg*& m_pThis;
	ILog& m_log;

	CWnd* m_pages[SETTING_DLG_PAGE_COUNT];
	CResizeControl m_pagesResize;
};
