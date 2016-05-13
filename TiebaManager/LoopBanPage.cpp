// LoopBanPage.cpp : 实现文件
//

#include "stdafx.h"
#include "LoopBanPage.h"

#include <StringHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>
#include <TBMConfig.h>

#include "TBMOperate.h"

#include "SuperFunctionDlg.h"
#include "TiebaManagerDlg.h"


// CLoopBanPage 对话框

IMPLEMENT_DYNAMIC(CLoopBanPage, CNormalListPage)

CLoopBanPage::CLoopBanPage(CWnd* pParent /*=NULL*/)
	: CNormalListPage(_T("用户名："), CLoopBanPage::IDD, pParent)
{

}

#pragma region MFC
CLoopBanPage::~CLoopBanPage()
{
}

void CLoopBanPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalListPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK6, m_logCheck);
	DDX_Control(pDX, IDC_CHECK7, m_enableCheck);
	DDX_Control(pDX, IDC_EDIT9, m_banIntervalEdit);
	DDX_Control(pDX, IDC_STATIC2, m_static2);
	DDX_Control(pDX, IDC_STATIC3, m_static3);
}


BEGIN_MESSAGE_MAP(CLoopBanPage, CNormalListPage)
END_MESSAGE_MAP()
#pragma endregion

// CLoopBanPage 消息处理程序

// 初始化
BOOL CLoopBanPage::OnInitDialog()
{
	CNormalListPage::OnInitDialog();

	m_resize.AddControl(&m_enableCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_logCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_static2, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_banIntervalEdit, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_static3, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 添加
void CLoopBanPage::OnAdd(int index)
{
	if (index >= 0)
	{
		if (m_pid.size() != m_list.GetItemCount())
			m_pid.insert(m_pid.begin() + index, _T("")); // 优先不使用PID封禁
		else
			m_pid[index] = _T("");
	}
	else
	{
		m_pid.clear();
		m_pid.resize(m_list.GetItemCount());
	}
	((CSuperFunctionDlg*)GetParent()->GetParent())->m_clearCache = TRUE;
}

// 删除
void CLoopBanPage::OnDelete(int index)
{
	if (index >= 0)
		m_pid.erase(m_pid.begin() + index);
	else
		m_pid.clear();
}

// 取用户发的帖子ID
static CString GetPIDFromUser(const CString& userName)
{
	CString src = HTTPGet(_T("http://tieba.baidu.com/f/search/ures?ie=utf-8&kw=") + g_tiebaOperate->GetEncodedForumName() + _T("&qw=&rn=10&un=")
		+ userName + _T("&only_thread=&sm=1&sd=&ed=&pn=1"));
	if (src == NET_TIMEOUT_TEXT)
		return NET_TIMEOUT_TEXT;
	CString pid = GetStringBetween(src, _T("<div class=\"s_post\">"), _T("target=\"_blank\" >"));
	pid = GetStringBetween(pid, _T("?pid="), _T("&"));
	return pid;
}

// 循环封线程
UINT AFX_CDECL LoopBanThread(LPVOID _dlg)
{
	class CLoopBanDate : public CConfigBase
	{
	public:
		COption<int> m_year;
		COption<int> m_month;
		COption<int> m_day;

		CLoopBanDate()
			: CConfigBase("LoopBanDate"),
			m_year("Year"),
			m_month("Month"),
			m_day("Day")
		{
			m_options.push_back(&m_year);
			m_options.push_back(&m_month);
			m_options.push_back(&m_day);
		}
	};


	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)_dlg;

	// 一天内循环封过不再封
	SYSTEMTIME time = {};
	GetLocalTime(&time);
	CLoopBanDate lastTime;
	lastTime.Load(CURRENT_USER_PATH + _T("\\LoopBanDate.xml"));
	if (time.wDay == lastTime.m_day && time.wMonth == lastTime.m_month && time.wYear == lastTime.m_year)
		return 0;

	CLoopBanConfig config;
	config.Load(CURRENT_USER_PATH + _T("\\options2.xml"));
	if (!config.m_enable)
		return 0;

	BOOL updatePID = FALSE;
	// 循环封
	dlg->m_stateStatic.SetWindowText(_T("循环封禁中"));
	if (!CoInitializeHelper())
		return 0;
	for (UINT i = 0; i < config.m_userList->size(); i++)
	{
		CString code;
		if (g_plan.m_wapBanInterface)
			code = g_tiebaOperate->BanIDClient((*config.m_userList)[i]); // 用WAP接口封禁
		else
		{
			if ((*config.m_pidList)[i] != _T("")) // 尝试用PID封禁
				code = g_tiebaOperate->BanID((*config.m_userList)[i], (*config.m_pidList)[i]);
			if ((*config.m_pidList)[i] == _T("") || code != _T("0")) // 尝试不用PID封禁（用户必须为本吧会员）
			{
				code = g_tiebaOperate->BanID((*config.m_userList)[i]);
				if (code != _T("0")) // 尝试获取新的PID并用PID封禁
				{
					(*config.m_pidList)[i] = GetPIDFromUser((*config.m_userList)[i]);
					updatePID = TRUE;
					code = g_tiebaOperate->BanID((*config.m_userList)[i], (*config.m_pidList)[i]);
				}
			}
		}
		
		if (config.m_log)
		{
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<font color=red>封禁 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
					_T("\"bd:%s,\">重试</a>"), (LPCTSTR)(*config.m_userList)[i], (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)(*config.m_userList)[i]);
				dlg->m_log.Log(content);
			}
			else
				dlg->m_log.Log(_T("<font color=red>封禁 </font>") + (*config.m_userList)[i]);
		}

		if (code == _T("0") && i < config.m_userList->size() - 1)
			Sleep((DWORD)(config.m_banInterval * 1000));
	}
	CoUninitialize();

	// 更新PID
	if (updatePID)
		config.Save(CURRENT_USER_PATH + _T("\\options2.xml"));

	// 更新时间
	*lastTime.m_year = time.wYear;
	*lastTime.m_month = time.wMonth;
	*lastTime.m_day = time.wDay;
	lastTime.Save(CURRENT_USER_PATH + _T("\\LoopBanDate.xml"));

	dlg->m_stateStatic.SetWindowText(_T("待机中"));
	return 0;
}
