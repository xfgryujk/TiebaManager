#pragma once
#include "afxwin.h"
#include <ConfigFile.h>
#include "NormalListPage.h"


class CLoopBanConfig : public CConfigBase
{
public:
	COption<BOOL> m_enable;					// 开启
	COption<BOOL> m_log;					// 输出日志
	COption<float> m_banInterval;			// 封禁间隔
	COption<vector<CString> > m_userList;	// 用户列表
	COption<vector<CString> > m_pidList;	// PID列表

	CLoopBanConfig() : CConfigBase("LoopBan"),
		m_enable("Enable", TRUE),
		m_log("Log"),
		m_banInterval("BanInterval", 0.0f, [](const float& value)->BOOL{ return 0.0f <= value && value <= 60.0f; }),
		m_userList("Name"),
		m_pidList("PID")
	{
		m_options.push_back(&m_enable);
		m_options.push_back(&m_log);
		m_options.push_back(&m_banInterval);
		m_options.push_back(&m_userList);
		m_options.push_back(&m_pidList);
	}

	virtual BOOL Load(const CString& path)
	{
		BOOL res = CConfigBase::Load(path);
		if (m_pidList->size() != m_userList->size())
			m_pidList->resize(m_userList->size());
		return res;
	}
};

// CLoopBanPage 对话框

class CLoopBanPage : public CNormalListPage
{
	DECLARE_DYNAMIC(CLoopBanPage)

public:
	CLoopBanPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLoopBanPage();

// 对话框数据
	enum { IDD = IDD_LOOP_BAN_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	virtual void OnAdd(int index);
	virtual void OnDelete(int index);


public:
	CButton m_enableCheck;
	CButton m_logCheck;
	CStatic m_static2;
	CEdit m_banIntervalEdit;
	CStatic m_static3;

	std::vector<CString> m_pid;
};

UINT AFX_CDECL LoopBanThread(LPVOID _dlg);
