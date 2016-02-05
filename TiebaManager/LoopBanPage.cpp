// LoopBanPage.cpp : 实现文件
//

#include "stdafx.h"
#include "LoopBanPage.h"

#include "StringHelper.h"
#include "NetworkHelper.h"
#include "Setting.h"

#include "TiebaCollect.h"
#include "TiebaOperate.h"

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

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 添加
void CLoopBanPage::OnClickedButton1()
{
	int index = m_list.GetSelectionMark() + 1;
	index = m_list.InsertItem(index, NULL);
	if (SetItem(index))
	{
		m_pid.insert(m_pid.begin() + index, _T("")); // 优先尝试不用PID封禁
		m_list.SetSelectionMark(index);
		m_list.SetItemState(index, LVNI_FOCUSED | LVNI_SELECTED, LVNI_FOCUSED | LVNI_SELECTED);
		((CSuperFunctionDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
	}
	else
		m_list.DeleteItem(index);
}

// 删除
void CLoopBanPage::OnClickedButton2()
{
	int index = m_list.GetSelectionMark();
	if (index == LB_ERR)
		return;
	m_list.DeleteItem(index);
	m_pid.erase(m_pid.begin() + index);
	index = index > 0 ? index - 1 : index;
	m_list.SetSelectionMark(index);
	m_list.SetItemState(index, LVNI_FOCUSED | LVNI_SELECTED, LVNI_FOCUSED | LVNI_SELECTED);
}

// 修改
void CLoopBanPage::OnClickedButton3()
{
	int index = m_list.GetSelectionMark();
	if (index == LB_ERR)
		return;
	if (SetItem(index))
	{
		m_pid[index] = _T(""); // 优先尝试不用PID封禁
		((CSuperFunctionDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
	}
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

		BOOL LoadOld(const CString& path)
		{
			CFile file;
			if (!file.Open(path, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite))
				return FALSE;
			SYSTEMTIME lastTime = {};
			file.Read(&lastTime, sizeof(lastTime));
			*m_year = lastTime.wYear;
			*m_month = lastTime.wMonth;
			*m_day = lastTime.wDay;
			return TRUE;
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

	// 更新时间
	*lastTime.m_year = time.wYear;
	*lastTime.m_month = time.wMonth;
	*lastTime.m_day = time.wDay;
	lastTime.Save(CURRENT_USER_PATH + _T("\\LoopBanDate.xml"));

	BOOL updatePID = FALSE;
	// 循环封
	dlg->m_stateStatic.SetWindowText(_T("循环封禁中"));
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	for (UINT i = 0; i < config.m_userList->size(); i++)
	{
		CString code;
		if ((*config.m_pidList)[i] != _T("")) // 尝试用PID封禁
			code = BanID((*config.m_userList)[i], (*config.m_pidList)[i]);
		if ((*config.m_pidList)[i] == _T("") || code != _T("0")) // 尝试不用PID封禁（用户必须为本吧会员）
		{
			code = BanID((*config.m_userList)[i]);
			if (code != _T("0")) // 尝试获取新的PID并用PID封禁
			{
				(*config.m_pidList)[i] = GetPIDFromUser((*config.m_userList)[i]);
				updatePID = TRUE;
				code = BanID((*config.m_userList)[i], (*config.m_pidList)[i]);
			}
		}

		if (config.m_log)
		{
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<font color=red>封禁 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
					_T("\"bd:%s,%s\">重试</a>"), (*config.m_userList)[i], code, GetTiebaErrorText(code), (*config.m_pidList)[i], 
					(*config.m_userList)[i]);
				dlg->m_log.Log(content);
			}
			else
				dlg->m_log.Log(_T("<font color=red>封禁 </font>") + (*config.m_userList)[i]);
		}

		if (code == _T("0") && i < config.m_userList->size() - 1)
			Sleep(3000);
	}
	CoUninitialize();

	// 更新PID
	if (updatePID)
		config.Save(CURRENT_USER_PATH + _T("\\options2.xml"));

	dlg->m_stateStatic.SetWindowText(_T("待机中"));
	return 0;
}
