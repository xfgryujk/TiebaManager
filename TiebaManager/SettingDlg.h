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

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <ModeLessDlg.h>
#include "resource.h"
#include <memory>

#include <TBMCoreConfig.h>

#include "ScanPage.h"
#include "OperatePage.h"
#include "KeywordsPage.h"
#include "ImagePage.h"
#include "BlackListPage.h"
#include "NormalListPage.h"
#include "WhiteContentPage.h"
#include "OptionsPage.h"
#include "UsersPage.h"
#include "AboutPage.h"


// CSettingDlg 对话框

const int SETTING_DLG_PAGE_COUNT = 11;

class CSettingDlg : public CModelessDlg
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
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();

	void ShowPlan(const CPlan& plan);
	void ApplyPlanInDlg(CPlan& plan);
	void ShowPlanInFile(const CString& path);
	void SavePlanInDlg(const CString& path);


public:
	CTabCtrl m_tab;
	std::unique_ptr<CScanPage> m_scanPage;
	std::unique_ptr<COperatePage> m_operatePage;
	std::unique_ptr<CKeywordsPage> m_keywordsPage;
	std::unique_ptr<CImagePage> m_imagePage;
	std::unique_ptr<CBlackListPage> m_blackListPage;
	std::unique_ptr<CNormalListPage> m_whiteListPage;
	std::unique_ptr<CWhiteContentPage> m_whiteContentPage;
	std::unique_ptr<CNormalListPage> m_trustedThreadPage;
	std::unique_ptr<COptionsPage> m_optionsPage;
	std::unique_ptr<CUsersPage> m_usersPage;
	std::unique_ptr<CAboutPage> m_aboutPage;
	CButton m_okButton;
	CButton m_cancelButton;

	BOOL m_clearScanCache;
protected:
	ILog& m_log;

	CWnd* m_pages[SETTING_DLG_PAGE_COUNT];
	CResizeControl m_pagesResize;
};
