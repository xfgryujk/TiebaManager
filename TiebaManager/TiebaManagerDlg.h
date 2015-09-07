
// TiebaManagerDlg.h : 头文件
//

#pragma once
#include "explorer1.h"
#include <mshtml.h>
#include "afxwin.h"
#include "TextButton.h"
#include "NormalDlg.h"
class CSettingDlg;
class CExplorerDlg;
class CSuperFunctionDlg;


// CTiebaManagerDlg 对话框
class CTiebaManagerDlg : public CNormalDlg
{
// 构造
public:
	CTiebaManagerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TIEBAMANAGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	LRESULT OnTray(WPARAM wParam, LPARAM lParam);
	LRESULT OnTaskBarCreated(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnStnClickedStatic6();
	afx_msg void OnStnClickedStatic7();
	afx_msg void OnBnClickedButton7();
	DECLARE_EVENTSINK_MAP()
	void BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel);


public:
	void Log(LPCTSTR content, CComPtr<IHTMLDocument2>* document = NULL);
	void GetLogDocument(CComPtr<IHTMLDocument2>& document);
protected:
	inline static void WriteDocument(const CString& content, CComPtr<IHTMLDocument2>& document);
	static BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);
	static LRESULT CALLBACK ExplorerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static UINT AFX_CDECL SaveLogThread(LPVOID _thiz);

	static UINT AFX_CDECL AutoUpdateThread(LPVOID _thiz);


public:
	CEdit m_forumNameEdit;
	CButton m_confirmButton;
	CButton m_startButton;
	CButton m_stopButton;
	CEdit m_pageEdit;
	CButton m_explorerButton;
	CButton m_superFunctionButton;
	CButton m_settingButton;
	CStatic m_logStatic;
	CExplorer1 m_logExplorer;
	CStatic m_stateStatic;
	CTextButton m_clearLogStatic;
	CTextButton m_saveLogStatic;
	CListBox m_stateList;

	CSettingDlg* m_settingDlg;
	CExplorerDlg* m_explorerDlg;
	CSuperFunctionDlg* m_superFunctionDlg;

protected:
	static WNDPROC s_oldExplorerWndProc;
	CComPtr<IHTMLDocument2> m_logDocument;
	SYSTEMTIME m_logStartTime;

	NOTIFYICONDATA m_nfData;
};
