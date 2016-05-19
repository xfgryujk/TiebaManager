// LoopBanPage.cpp : 实现文件
//

#include "stdafx.h"
#include "LoopBanPage.h"

#include <StringHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>
#include "TiebaManagerDlg.h"

#include "TBMConfig.h"
#include "TBMConfigPath.h"
#include "TiebaManager.h"
#include <TiebaOperate.h>

#include "SuperFunctionDlg.h"


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
	CString src = HTTPGet(_T("http://tieba.baidu.com/f/search/ures?ie=utf-8&kw=") + theApp.m_tiebaOperate->GetEncodedForumName() + _T("&qw=&rn=10&un=")
		+ userName + _T("&only_thread=&sm=1&sd=&ed=&pn=1"));
	if (src == NET_TIMEOUT_TEXT)
		return NET_TIMEOUT_TEXT;
	CString pid = GetStringBetween(src, _T("<div class=\"s_post\">"), _T("target=\"_blank\" >"));
	pid = GetStringBetween(pid, _T("?pid="), _T("&"));
	return pid;
}
