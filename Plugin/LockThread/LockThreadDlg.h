#pragma once
#include <NormalDlg.h>
#include "afxwin.h"


// CLockThreadDlg 对话框

class CLockThreadDlg : public CNormalDlg
{
	DECLARE_DYNAMIC(CLockThreadDlg)

public:
	CLockThreadDlg(CLockThreadDlg*& pThis, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLockThreadDlg();

// 对话框数据
	enum { IDD = IDD_LOCK_THREAD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();


public:
	CEdit m_tidEdit;
	CEdit m_pageEdit;
	CEdit m_floorEdit;
	CButton m_startButton;
	CButton m_stopButton;

protected:
	CLockThreadDlg*& m_pThis;
};
