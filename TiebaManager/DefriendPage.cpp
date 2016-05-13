// DefriendPage.cpp : 实现文件
//

#include "stdafx.h"
#include "DefriendPage.h"
#include "TiebaManagerDlg.h"
#include <StringHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>
#include "TiebaVariable.h"
#include "TBMOperate.h"


// CDefriendPage 对话框

IMPLEMENT_DYNAMIC(CDefriendPage, CNormalDlg)

CDefriendPage::CDefriendPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CDefriendPage::IDD, pParent)
{

}

CDefriendPage::~CDefriendPage()
{
}

void CDefriendPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_startPageEdit);
	DDX_Control(pDX, IDC_EDIT2, m_endPageEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_startButton);
	DDX_Control(pDX, IDC_BUTTON2, m_stopButton);
	DDX_Control(pDX, IDC_STATIC2, m_stateStatic);
	DDX_Control(pDX, IDC_CHECK1, m_defriendNewUsersCheck);
}


BEGIN_MESSAGE_MAP(CDefriendPage, CNormalDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CDefriendPage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDefriendPage::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CHECK1, &CDefriendPage::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CDefriendPage 消息处理程序

CDefriendPage* CDefriendPage::s_instance = NULL;
CCriticalSection CDefriendPage::s_instanceLock;
volatile BOOL CDefriendPage::s_stopFlag = TRUE;
CString CDefriendPage::s_startPage;
CString CDefriendPage::s_endPage;
BOOL CDefriendPage::s_defriendNewUsers = FALSE;

// 初始化
BOOL CDefriendPage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	s_instanceLock.Lock();
	s_instance = this;
	s_instanceLock.Unlock();

	if (!s_stopFlag)
	{
		m_startPageEdit.EnableWindow(FALSE);
		m_endPageEdit.EnableWindow(FALSE);
		m_defriendNewUsersCheck.EnableWindow(FALSE);
		m_startButton.EnableWindow(FALSE);
		m_stopButton.EnableWindow(TRUE);
		m_stateStatic.SetWindowText(_T("拉黑中"));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 销毁
void CDefriendPage::OnDestroy()
{
	s_instanceLock.Lock();
	s_instance = NULL;
	s_instanceLock.Unlock();

	CNormalDlg::OnDestroy();
}

// 拉黑新关注的
void CDefriendPage::OnBnClickedCheck1()
{
	if (m_defriendNewUsersCheck.GetCheck())
	{
		m_startPageEdit.EnableWindow(FALSE);
		m_startPageEdit.SetWindowText(_T("1"));
	}
	else
		m_startPageEdit.EnableWindow(TRUE);
}

// 开始
void CDefriendPage::OnBnClickedButton1()
{
	m_startPageEdit.GetWindowText(s_startPage);
	m_endPageEdit.GetWindowText(s_endPage);
	int iStartPage = _ttoi(s_startPage), iEndPage = _ttoi(s_endPage);
	if (iStartPage > iEndPage)
	{
		m_startPageEdit.SetWindowText(s_endPage);
		m_endPageEdit.SetWindowText(s_startPage);
		CString st = s_startPage;
		s_startPage = s_endPage;
		s_endPage = st;
		int t = iStartPage;
		iStartPage = iEndPage;
		iEndPage = t;
	}
	if (iStartPage < 1)
	{
		AfxMessageBox(_T("起始页数应大于0！"), MB_ICONERROR);
		return;
	}
	s_defriendNewUsers = m_defriendNewUsersCheck.GetCheck();

	m_startPageEdit.EnableWindow(FALSE);
	m_endPageEdit.EnableWindow(FALSE);
	m_defriendNewUsersCheck.EnableWindow(FALSE);
	m_startButton.EnableWindow(FALSE);
	m_stopButton.EnableWindow(TRUE);
	s_stopFlag = FALSE;
	AfxBeginThread(DefriendThread, NULL);
}

// 停止
void CDefriendPage::OnBnClickedButton2()
{
	s_stopFlag = TRUE;
}

UINT AFX_CDECL CDefriendPage::DefriendThread(LPVOID)
{
	// 初始化
	CTiebaManagerDlg* mainDlg = (CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd;
	if (!CoInitializeHelper())
		return 0;

	int iStartPage = _ttoi(s_startPage), iEndPage = _ttoi(s_endPage);
	int iPrevTotalPage = 0;
	do
	{
		if (s_defriendNewUsers)
		{
			CString url;
			url.Format(_T("http://tieba.baidu.com/bawu2/platform/listMember?ie=utf-8&word=%s"), (LPCTSTR)g_tiebaOperate->GetEncodedForumName());
			CString src = HTTPGet(url, &*g_cookieConfig.m_cookie);
			CString totalPage = GetStringBetween(src, _T(R"(class="tbui_total_page">共)"), _T("页"));
			if (totalPage == _T(""))
			{
				AfxMessageBox(_T("获取总页数失败！"), MB_ICONERROR);
				break;
			}
			int iTotalPage = _ttoi(totalPage);

			if (iTotalPage <= iPrevTotalPage) // 没有新关注的用户（或者不足1页）
				break;
			if (iPrevTotalPage != 0)
			{
				//iStartPage = 1;
				iEndPage = iTotalPage - iPrevTotalPage;
			}
			iPrevTotalPage = iTotalPage;
		}

		// 拉黑
		DoDefriend(iStartPage, iEndPage);
	} while (s_defriendNewUsers); // 循环直到没有新关注的用户

	// 结束
	s_stopFlag = TRUE;
	CoUninitialize();
	s_instanceLock.Lock();
	if (s_instance != NULL)
	{
		s_instance->m_startPageEdit.EnableWindow(TRUE);
		s_instance->m_endPageEdit.EnableWindow(TRUE);
		s_instance->m_defriendNewUsersCheck.EnableWindow(TRUE);
		s_instance->m_startButton.EnableWindow(TRUE);
		s_instance->m_stopButton.EnableWindow(FALSE);
		s_instance->m_stateStatic.SetWindowText(_T(""));
	}
	s_instanceLock.Unlock();
	return 0;
}

void CDefriendPage::DoDefriend(int startPage, int endPage)
{
	CTiebaManagerDlg* mainDlg = (CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd;

	// 获取拉黑列表
	int index = 0;
	std::vector<CString> userName, userID;
	for (int page = startPage; page <= endPage; page++)
	{
		if (s_stopFlag)
			break;
		CString state;
		state.Format(_T("采集第%d页"), page);
		s_instanceLock.Lock();
		if (s_instance != NULL)
			s_instance->m_stateStatic.SetWindowText(state);
		s_instanceLock.Unlock();
		CString url;
		url.Format(_T("http://tieba.baidu.com/bawu2/platform/listMember?ie=utf-8&word=%s&pn=%d"), (LPCTSTR)g_tiebaOperate->GetEncodedForumName(), page);
		CString src = HTTPGet(url, &*g_cookieConfig.m_cookie);
		if (s_stopFlag)
			break;

		CStringArray rawUsers;
		SplitString(rawUsers, src, _T("<tr><td class=\"left_cell\">"));
		if (rawUsers.GetSize() < 2)
		{
			if (src != NET_TIMEOUT_TEXT)
				WriteString(src, _T("defriend.txt"));
			else
				page--;
			continue;
		}

		userName.resize((int)userName.size() + (int)rawUsers.GetSize() - 1);
		userID.resize((int)userID.size() + (int)rawUsers.GetSize() - 1);
		CString tmp;
		for (int i = 1; i < rawUsers.GetSize(); i++, index++)
		{
			userName[index] = tmp = GetStringBetween(rawUsers[i], _T("un=\""), _T("\""));
			userID[index] = tmp = GetStringBetween(rawUsers[i], _T("id=\""), _T("\""));
		}
	}

	// 拉黑
	for (UINT i = 0; i < userName.size(); i++)
	{
		if (s_stopFlag)
			break;
		/*if (g_defriendedUser.find(userName[i]) != g_defriendedUser.end())
		continue;*/

		CString state;
		state.Format(_T("拉黑中，剩余%u"), userName.size() - i - 1);
		s_instanceLock.Lock();
		if (s_instance != NULL)
			s_instance->m_stateStatic.SetWindowText(state);
		s_instanceLock.Unlock();

		CString code = g_tiebaOperate->Defriend(userID[i]);
		if (code != _T("0"))
		{
			CString content;
			content.Format(_T("<font color=red>拉黑 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
				_T("\"df:%s\">重试</a>"), (LPCTSTR)userName[i], (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)userID[i]);
			mainDlg->m_log.Log(content);
		}
		else
		{
			g_userCache.m_defriendedUser->insert(userName[i]);
			mainDlg->m_log.Log(_T("<font color=red>拉黑 </font>") + userName[i]);
		}

		// 每分钟最多拉黑20个
		if (i % 20 == 19 && i < userName.size() - 1)
		{
			s_instanceLock.Lock();
			if (s_instance != NULL)
				s_instance->m_stateStatic.SetWindowText(_T("延迟中"));
			s_instanceLock.Unlock();
			for (int i = 0; i < 400; i++)
			{
				if (s_stopFlag)
					break;
				Sleep(100);
			}
		}
	}
}
