#pragma once
#include <NormalDlg.h>
class CLoopBanPage;
class CDefriendPage;
class CLockThreadPage;


// CSuperFunctionDlg 对话框

class CSuperFunctionDlg : public CNormalDlg
{
	DECLARE_DYNAMIC(CSuperFunctionDlg)

public:
	CSuperFunctionDlg(CSuperFunctionDlg*& pThis, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSuperFunctionDlg();

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


public:
	CTabCtrl m_tab;
	unique_ptr<CLoopBanPage> m_loopBanPage;
	unique_ptr<CDefriendPage> m_defriendPage;
	unique_ptr<CLockThreadPage> m_lockThreadPage;
	CButton m_okButton;
	CButton m_cancelButton;

	BOOL m_clearCache;

protected:
	CSuperFunctionDlg*& m_pThis;

	CWnd* m_pages[3];
	CResizeControl m_pagesResize;
};
