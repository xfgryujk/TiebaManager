// LoopBan.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "LoopBan.h"

#include <TBMAPI.h>
#include <TBMEvent.h>

#include <StringHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>

#include <TiebaOperate.h>
#include <TBMCoreConfig.h>

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


bool CLoopBan::Init()
{
	m_onPostSetTiebaID = CTBMAPI::GetInstance().GetEventBus()->AddListener(PostSetTiebaEvent, 
		std::bind(&CLoopBan::OnPostSetTieba, this, std::placeholders::_1));
	
	// 每24小时开始循环封
	thread([this]{
		while (TRUE)
		{
			Sleep(24 * 60 * 60 * 1000);
			thread(&CLoopBan::LoopBanThread, this).detach();
		}
	}).detach();

	return true;
}

bool CLoopBan::Uninit()
{
	CTBMAPI::GetInstance().GetEventBus()->DeleteListener(PostSetTiebaEvent, m_onPostSetTiebaID);
	return true;
}


void CLoopBan::OnPostSetTieba(CEventBase* event__)
{
	thread(&CLoopBan::LoopBanThread, this).detach();
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
	CTiebaOperate tiebaOperate = *CTBMAPI::GetInstance().GetTiebaOperate();


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

	CLoopBanConfig config;
	config.Load(currentUserDir + _T("options2.xml"));
	if (!config.m_enable)
		return;

	BOOL updatePID = FALSE;
	// 循环封
	if (!CoInitializeHelper())
		return;
	for (UINT i = 0; i < config.m_userList->size(); i++)
	{
		CString code;
		//if (theApp.m_plan->m_wapBanInterface)
			code = tiebaOperate.BanIDClient((*config.m_userList)[i]); // 用WAP接口封禁
		//else
		//{
		//	if ((*config.m_pidList)[i] != _T("")) // 尝试用PID封禁
		//		code = theApp.m_tiebaOperate->BanID((*config.m_userList)[i], (*config.m_pidList)[i]);
		//	if ((*config.m_pidList)[i] == _T("") || code != _T("0")) // 尝试不用PID封禁（用户必须为本吧会员）
		//	{
		//		code = theApp.m_tiebaOperate->BanID((*config.m_userList)[i]);
		//		if (code != _T("0")) // 尝试获取新的PID并用PID封禁
		//		{
		//			(*config.m_pidList)[i] = GetPIDFromUser((*config.m_userList)[i]);
		//			updatePID = TRUE;
		//			code = theApp.m_tiebaOperate->BanID((*config.m_userList)[i], (*config.m_pidList)[i]);
		//		}
		//	}
		//}

		if (config.m_log)
		{
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<font color=red>封禁 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
					_T("\"bd:%s,\">重试</a>"), (LPCTSTR)(*config.m_userList)[i], (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)(*config.m_userList)[i]);
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
		config.Save(currentUserDir + _T("options2.xml"));

	// 更新时间
	*lastTime.m_year = time.wYear;
	*lastTime.m_month = time.wMonth;
	*lastTime.m_day = time.wDay;
	lastTime.Save(currentUserDir + _T("LoopBanDate.xml"));
}
