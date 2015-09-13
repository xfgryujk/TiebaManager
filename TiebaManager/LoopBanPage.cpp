// LoopBanPage.cpp : 实现文件
//

#include "stdafx.h"
#include "LoopBanPage.h"
#include "Global.h"
#include "Tieba.h"


// CLoopBanPage 对话框

IMPLEMENT_DYNAMIC(CLoopBanPage, CListPage)

CLoopBanPage::CLoopBanPage(CWnd* pParent /*=NULL*/)
	: CListPage(CLoopBanPage::IDD, pParent)
{

}

#pragma region MFC
CLoopBanPage::~CLoopBanPage()
{
}

void CLoopBanPage::DoDataExchange(CDataExchange* pDX)
{
	CListPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoopBanPage, CListPage)
END_MESSAGE_MAP()
#pragma endregion

// CLoopBanPage 消息处理程序

// 初始化
BOOL CLoopBanPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_contentStatic.SetWindowText(_T("用户名："));
	m_static.SetWindowText(_T("添加的用户会在每天确认贴吧后封禁"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 添加
void CLoopBanPage::OnClickedButton1()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("内容不能为空！"), MB_ICONERROR);
		return;
	}

	CString pid = GetPID(text);
	if (pid == NET_TIMEOUT_TEXT)
	{
		AfxMessageBox(_T("连接超时"), MB_ICONERROR);
		return;
	}
	if (pid == _T(""))
	{
		AfxMessageBox(_T("此用户没有在本吧发过贴"), MB_ICONERROR);
		return;
	}

	int index = m_list.GetCurSel();
	index = m_list.InsertString(index + 1, text);
	m_pid.insert(m_pid.begin() + index, pid);
	m_list.SetCurSel(index);
}

// 删除
void CLoopBanPage::OnClickedButton2()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_list.DeleteString(index);
	m_pid.erase(m_pid.begin() + index);
	m_list.SetCurSel(index == 0 ? 0 : index - 1);
}

// 修改
void CLoopBanPage::OnClickedButton3()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("内容不能为空！"), MB_ICONERROR);
		return;
	}
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;

	CString pid = GetPID(text);
	if (pid == NET_TIMEOUT_TEXT)
	{
		AfxMessageBox(_T("连接超时"), MB_ICONERROR);
		return;
	}
	if (pid == _T(""))
	{
		AfxMessageBox(_T("此用户没有在本吧发过贴"), MB_ICONERROR);
		return;
	}

	m_list.DeleteString(index);
	index = m_list.InsertString(index, text);
	m_pid.insert(m_pid.begin() + index, pid);
	m_list.SetCurSel(index);
}

CString CLoopBanPage::GetPID(const CString& userName)
{
	CString src = HTTPGet(_T("http://tieba.baidu.com/f/search/ures?ie=utf-8&kw=") + g_encodedForumName + _T("&qw=&rn=10&un=") + userName + _T("&only_thread=&sm=1&sd=&ed=&pn=1"), FALSE);
	if (src == NET_TIMEOUT_TEXT)
		return NET_TIMEOUT_TEXT;
	CString pid =  GetStringBetween(src, _T("<div class=\"s_post\">"), _T("target=\"_blank\" >"));
	pid = GetStringBetween(pid, _T("?pid="), _T("&"));
	return pid;
}
