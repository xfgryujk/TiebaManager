// LoopBan.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "LoopBan.h"
#include "LoopBanDlg.h"

#include <TBMAPI.h>
#include <TBMEvent.h>
#include <TBMOperateEvent.h>

#include <StringHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>

#include <TiebaOperate.h>
#include <TBMCoreConfig.h>
#include <TBMOperate.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CLoopBan g_loopBan;


extern "C" __declspec(dllexport) bool __cdecl Init()
{
	return g_loopBan.Init();
}

extern "C" __declspec(dllexport) bool __cdecl Uninit()
{
	return g_loopBan.Uninit();
}

extern "C" __declspec(dllexport) CString __cdecl GetDescription()
{
	return _T("循环封插件\r\n")
		   _T("\r\n")
		   _T("作者：盗我原号的没J8");
}

extern "C" __declspec(dllexport) void __cdecl OnConfig()
{
	g_loopBan.OnConfig();
}


bool CLoopBan::Init()
{
	// 注册监听
	m_onPostSetTiebaID = CTBMAPI::GetInstance().GetEventBus()->AddListener(PostSetTiebaEvent, 
		std::bind(&CLoopBan::OnPostSetTieba, this, std::placeholders::_1));
	m_onPostBanID = CTBMAPI::GetInstance().GetOperate()->m_eventBus.AddListener(PostBanEvent,
		std::bind(&CLoopBan::OnPostBan, this, std::placeholders::_1));
	
	// 每24小时开始循环封
	thread([this]{
		while (TRUE)
		{
			Sleep(24 * 60 * 60 * 1000);
			if (CTBMAPI::GetInstance().GetTiebaOperate()->HasSetTieba())
				thread(&CLoopBan::LoopBanThread, this).detach();
		}
	}).detach();

	return true;
}

bool CLoopBan::Uninit()
{
	// 删除监听
	CTBMAPI::GetInstance().GetEventBus()->DeleteListener(PostSetTiebaEvent, m_onPostSetTiebaID);
	CTBMAPI::GetInstance().GetOperate()->m_eventBus.DeleteListener(PostBanEvent, m_onPostBanID);
	
	// 关闭窗口
	if (m_loopBanDlg != NULL)
		m_loopBanDlg->DestroyWindow();

	// 保存配置
	if (CTBMAPI::GetInstance().GetTiebaOperate()->HasSetTieba())
	{
		CString currentUserDir = CTBMAPI::GetCurrentUserDir();
		m_config.Save(currentUserDir + _T("options2.xml"));
	}
	return true;
}

void CLoopBan::OnConfig()
{
	if (!CTBMAPI::GetInstance().GetTiebaOperate()->HasSetTieba())
	{
		AfxMessageBox(_T("请先确认贴吧！"), MB_ICONERROR);
		return;
	}

	if (m_loopBanDlg == NULL)
	{
		m_loopBanDlg = new CLoopBanDlg(m_loopBanDlg, m_config);
		m_loopBanDlg->Create(m_loopBanDlg->IDD, CWnd::GetDesktopWindow());
	}
}


void CLoopBan::OnPostSetTieba(CEventBase* event__)
{
	// 载入配置
	CString currentUserDir = CTBMAPI::GetCurrentUserDir();
	m_config.Load(currentUserDir + _T("options2.xml"));

	// 开始循环封
	thread(&CLoopBan::LoopBanThread, this).detach();
}

void CLoopBan::OnPostBan(CEventBase* event__)
{
	COperateEvent* event_ = (COperateEvent*)event__;
	// 自动循环封
	if (m_config.m_autoLoopBan)
	{
		auto it = std::find(m_config.m_userList->cbegin(), m_config.m_userList->cend(), event_->m_op.author);
		if (it == m_config.m_userList->cend())
		{
			m_config.m_userList->push_back(event_->m_op.author);
			m_config.m_pidList->push_back(_T(""));
			CString currentUserDir = CTBMAPI::GetCurrentUserDir();
			DeleteFile(currentUserDir + _T("LoopBanDate.xml"));
		}
	}
}


// 取用户发的帖子ID
static CString GetPIDFromUser(const CString& userName)
{
	CString src = HTTPGet(_T("http://tieba.baidu.com/f/search/ures?ie=utf-8&kw=") + CTBMAPI::GetInstance().GetTiebaOperate()->GetEncodedForumName() 
		+ _T("&qw=&rn=10&un=") + userName + _T("&only_thread=&sm=1&sd=&ed=&pn=1"));
	if (src == NET_TIMEOUT_TEXT)
		return NET_TIMEOUT_TEXT;
	CString pid = GetStringBetween(src, _T("<div class=\"s_post\">"), _T("target=\"_blank\" >"));
	pid = GetStringBetween(pid, _T("?pid="), _T("&"));
	return pid;
}

void CLoopBan::LoopBanThread()
{
	ILog& log = *CTBMAPI::GetInstance().GetLog();
	CString currentUserDir = CTBMAPI::GetCurrentUserDir();
	CTiebaOperate& tiebaOperate = *CTBMAPI::GetInstance().GetTiebaOperate();
	CTBMOperate& operate = *CTBMAPI::GetInstance().GetOperate();


	class CLoopBanDate : public CConfigBase
	{
	public:
		COption<int> m_year;
		COption<int> m_month;
		COption<int> m_day;

		CLoopBanDate() : CConfigBase("LoopBanDate"),
			m_year("Year"),
			m_month("Month"),
			m_day("Day")
		{
			m_options.push_back(&m_year);
			m_options.push_back(&m_month);
			m_options.push_back(&m_day);
		}
	};

	// 一天内循环封过不再封
	SYSTEMTIME time = {};
	GetLocalTime(&time);
	CLoopBanDate lastTime;
	lastTime.Load(currentUserDir + _T("LoopBanDate.xml"));
	if (time.wDay == lastTime.m_day && time.wMonth == lastTime.m_month && time.wYear == lastTime.m_year)
		return;

	if (!m_config.m_enable)
		return;
	CLoopBanConfig config = m_config; // 复制一份防止在主线程被修改

	BOOL updatePID = FALSE;
	// 循环封
	if (!CoInitializeHelper())
		return;
	for (UINT i = 0; i < config.m_userList->size(); i++)
	{
		CString code;
		if (operate.m_config->m_wapBanInterface)
			code = tiebaOperate.BanIDClient((*config.m_userList)[i]); // 用WAP接口封禁
		else
		{
			if ((*config.m_pidList)[i] != _T("")) // 尝试用PID封禁
				code = tiebaOperate.BanID((*config.m_userList)[i], (*config.m_pidList)[i]);
			if ((*config.m_pidList)[i] == _T("") || code != _T("0")) // 尝试不用PID封禁（用户必须为本吧会员）
			{
				code = tiebaOperate.BanID((*config.m_userList)[i]);
				if (code != _T("0")) // 尝试获取新的PID并用PID封禁
				{
					(*config.m_pidList)[i] = GetPIDFromUser((*config.m_userList)[i]);
					updatePID = TRUE;
					code = tiebaOperate.BanID((*config.m_userList)[i], (*config.m_pidList)[i]);
				}
			}
		}

		if (config.m_log)
		{
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<font color=red>封禁 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
					_T("\"bd:%s,\">重试</a>"), (LPCTSTR)(*config.m_userList)[i], (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), 
					(LPCTSTR)(*config.m_userList)[i]);
				log.Log(content);
			}
			else
				log.Log(_T("<font color=red>封禁 </font>") + (*config.m_userList)[i]);
		}

		if (code == _T("0") && i < config.m_userList->size() - 1)
			Sleep((DWORD)(config.m_banInterval * 1000));
	}
	CoUninitialize();

	// 更新PID
	if (updatePID)
		config.Save(currentUserDir + _T("options2.xml")); // 可能会覆盖刚刚在LoopBanDlg修改的设置

	// 更新时间
	*lastTime.m_year = time.wYear;
	*lastTime.m_month = time.wMonth;
	*lastTime.m_day = time.wDay;
	lastTime.Save(currentUserDir + _T("LoopBanDate.xml"));
}
