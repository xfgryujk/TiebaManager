#pragma once
#include <NormalDlg.h>
class CPluginManager;
class CPlugin;


// CPluginDlg 对话框

class CPluginDlg : public CNormalDlg
{
	DECLARE_DYNAMIC(CPluginDlg)

public:
	CPluginDlg(CPluginDlg*& pThis, CPluginManager& pluginManager, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPluginDlg();

// 对话框数据
	enum { IDD = IDD_PLUGIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	afx_msg void OnClose();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton1();


public:
	CListBox m_list;
	CButton m_configButton;
	CEdit m_edit;

protected:
	CPluginDlg*& m_pThis;

	CPluginManager& m_pluginManager;
	const vector<unique_ptr<CPlugin> >& m_plugins;
};
