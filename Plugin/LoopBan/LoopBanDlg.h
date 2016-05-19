#pragma once
#include "afxwin.h"
#include <NormalListPage.h>
class CLoopBanConfig;


// CLoopBanDlg 对话框

class CLoopBanDlg : public CNormalListPage
{
	DECLARE_DYNAMIC(CLoopBanDlg)

public:
	CLoopBanDlg(CLoopBanDlg*& pThis, CLoopBanConfig& config, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLoopBanDlg();

// 对话框数据

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	virtual void OnOK();
	afx_msg void OnClose();

	virtual void OnAdd(int index);
	virtual void OnDelete(int index);


public:
	CButton m_enableCheck;
	CButton m_logCheck;
	CStatic m_static2;
	CEdit m_banIntervalEdit;
	CStatic m_static3;
	CButton m_autoLoopBanCheck;
	CButton m_okButton;
	CButton m_cancelButton;

	CLoopBanConfig& m_config;
	std::vector<CString> m_pid;

	BOOL m_clearCache = FALSE;
protected:
	CLoopBanDlg*& m_pThis;
};
