#pragma once
#include <ConfigFile.h>
class CEventBase;


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

	BOOL Load(const CString& path)
	{
		BOOL res = CConfigBase::Load(path);
		if (m_pidList->size() != m_userList->size())
			m_pidList->resize(m_userList->size());
		return res;
	}
};


class CLoopBan
{
protected:
	int m_onPostSetTiebaID = -1;

public:
	bool Init();
	bool Uninit();

	void OnPostSetTieba(CEventBase* event__);

	void LoopBanThread();
};
extern CLoopBan g_loopBan;
