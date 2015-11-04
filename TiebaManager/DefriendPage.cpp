// DefriendPage.cpp : 实现文件
//

#include "stdafx.h"
#include "DefriendPage.h"
#include "TiebaManagerDlg.h"
#include "Global.h"
#include "Tieba.h"
#include <vector>


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
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_startPageEdit);
	DDX_Control(pDX, IDC_EDIT2, m_endPageEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_startButton);
}


BEGIN_MESSAGE_MAP(CDefriendPage, CNormalDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CDefriendPage::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDefriendPage 消息处理程序

static CDefriendPage* g_this = NULL;

BOOL CDefriendPage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	g_this = this;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CDefriendPage::OnDestroy()
{
	CNormalDlg::OnDestroy();

	g_this = NULL;
}

// 开始
void CDefriendPage::OnBnClickedButton1()
{
	m_startPageEdit.EnableWindow(FALSE);
	m_endPageEdit.EnableWindow(FALSE);
	m_startButton.EnableWindow(FALSE);
	AfxBeginThread(DefriendThread, this);
}

UINT AFX_CDECL CDefriendPage::DefriendThread(LPVOID _thiz)
{
	//CDefriendPage* thiz = (CDefriendPage*)_thiz;
	// 初始化日志文档
	CTiebaManagerDlg* mainDlg = (CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CComPtr<IHTMLDocument2> document;
	mainDlg->GetLogDocument(document);
	CComPtr<IHTMLDocument2>* pDocument = (CComPtr<IHTMLDocument2>*)&(int&)document;

	CString sStartPage, sEndPage;
	if (g_this != NULL)
	{
		g_this->m_startPageEdit.GetWindowText(sStartPage);
		g_this->m_endPageEdit.GetWindowText(sEndPage);
	}
	int startPage = _ttoi(sStartPage), endPage = _ttoi(sEndPage);
	if (startPage > endPage)
	{
		int t = startPage;
		startPage = endPage;
		endPage = t;
	}

	// 获取拉黑列表
	int index = 0;
	std::vector<CString> userName, userID;
	for (int page = startPage; page <= endPage; page++)
	{
		CString url;
		url.Format(_T("http://tieba.baidu.com/bawu2/platform/listMember?ie=utf-8&word=%s&pn=%d"), g_encodedForumName, page);
		CString src = HTTPGet(url);

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
		if (g_defriendedUser.find(userName[i]) != g_defriendedUser.end())
			continue;

		CString code = Defriend(userID[i]);
		if (code != _T("0"))
		{
			CString content;
			content.Format(_T("<font color=red>拉黑 </font>%s<font color=red> 失败！\
错误代码：%s(%s)</font><a href=\"df:%s\">重试</a>"), userName[i], code, GetTiebaErrorText(code), userID[i]);
			mainDlg->Log(content, pDocument);
		}
		else
		{
			g_defriendedUser.insert(userName[i]);
			mainDlg->Log(_T("<font color=red>拉黑 </font>") + userName[i], pDocument);
		}
	}

	CoUninitialize();
	if (g_this != NULL)
	{
		g_this->m_startPageEdit.EnableWindow(TRUE);
		g_this->m_endPageEdit.EnableWindow(TRUE);
		g_this->m_startButton.EnableWindow(TRUE);
	}
	return 0;
}
