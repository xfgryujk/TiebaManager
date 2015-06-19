// ExplorerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "ExplorerDlg.h"
#include "afxdialogex.h"
#include "TiebaManagerDlg.h"
#include "Tieba.h"
#include "Global.h"
#include <Mmsystem.h>


// CExplorerDlg 对话框

IMPLEMENT_DYNAMIC(CExplorerDlg, CDialog)

// 构造函数
CExplorerDlg::CExplorerDlg(CWnd* pParent /*=NULL*/)

	: CDialog(CExplorerDlg::IDD, pParent)
{
	// 初始化m_pages
	int i = 0;
	m_pages[i++] = &m_exploreThreadPage;
	m_pages[i++] = &m_explorePostPage;
	m_pages[i++] = &m_exploreLzlPage;
}

#pragma region MFC
CExplorerDlg::~CExplorerDlg()
{
}

void CExplorerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_BUTTON1, m_deleteButton);
	DDX_Control(pDX, IDC_BUTTON2, m_banButton);
	DDX_Control(pDX, IDC_BUTTON3, m_explorerButton);
}


BEGIN_MESSAGE_MAP(CExplorerDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CExplorerDlg::OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_BUTTON1, &CExplorerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CExplorerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CExplorerDlg::OnBnClickedButton3)
END_MESSAGE_MAP()
#pragma endregion

// CExplorerDlg 消息处理程序

#pragma region UI
// 窗口 /////////////////////////////////////////////////////////////////////////////////

// 屏蔽Esc关闭窗口
void CExplorerDlg::OnCancel()
{
}

// 屏蔽回车关闭窗口
void CExplorerDlg::OnOK()
{
}

// 销毁窗口
void CExplorerDlg::OnClose()
{
	DestroyWindow();
}

// 释放this
void CExplorerDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();

	((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_explorerDlg = NULL;
	delete this;
}

// 限制最小尺寸
void CExplorerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 586;
	lpMMI->ptMinTrackSize.y = 482;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

// 改变尺寸
void CExplorerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_tab.m_hWnd == NULL)
		return;

	CRect rect;
	GetClientRect(&rect); // 默认798 * 564
	m_tab.SetWindowPos(NULL, 0, 0, rect.Width() - 21, 315, SWP_NOMOVE | SWP_NOREDRAW);
	m_edit.SetWindowPos(NULL, 0, 0, rect.Width() - 94, rect.Height() - 350, SWP_NOMOVE | SWP_NOREDRAW);
	int x = rect.Width() - 72;
	m_deleteButton.SetWindowPos(NULL, x, 338, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_banButton.SetWindowPos(NULL, x, 371, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_explorerButton.SetWindowPos(NULL, x, 405, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);

	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	for (int i = 0; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOREDRAW);

	Invalidate();
}

// 切换标签
void CExplorerDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	int index = m_tab.GetCurSel();
	for (int i = 0; i < _countof(m_pages); i++)
		m_pages[i]->ShowWindow(i == index ? SW_SHOW : SW_HIDE);
}
#pragma endregion

// 初始化
BOOL CExplorerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// 设置大图标
	SetIcon(hIcon, FALSE);			// 设置小图标

	// 初始化m_tab
	int i = 0;
	m_tab.InsertItem(i++, _T("主题"));
	m_tab.InsertItem(i++, _T("帖子"));
	m_tab.InsertItem(i++, _T("楼中楼"));

	// 初始化各页
	m_exploreThreadPage.Create(IDD_EXPLORER_PAGE, &m_tab);
	m_explorePostPage.Create(IDD_EXPLORER_PAGE, &m_tab);
	m_exploreLzlPage.Create(IDD_EXPLORER_PAGE, &m_tab);

	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	m_pages[0]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	for (i = 1; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_HIDEWINDOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 删除
void CExplorerDlg::OnBnClickedButton1()
{
	int tabIndex = m_tab.GetCurSel(); 
	POSITION pos = m_pages[tabIndex]->m_list.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int index = m_pages[tabIndex]->m_list.GetNextSelectedItem(pos);


	CString code;
	if (tabIndex == 0) // 主题
	{
		ThreadInfo& thread = m_exploreThreadPage.m_threads[index];
		code = DeleteThread(thread.tid);
		if (code == _T("0"))
			g_deletedTID.insert(_ttoi64(thread.tid));
	}
	else if (tabIndex == 1) // 帖子
		code = DeletePost(m_explorePostPage.m_tid, m_explorePostPage.m_posts[index].pid);
	else // 楼中楼
		code = DeleteLZL(m_explorePostPage.m_tid, m_exploreLzlPage.m_lzls[index].pid);


	if (code != _T("0"))
		AfxMessageBox(_T("删除失败，错误代码" + code + _T("(") + GetTiebaErrorText(code) + _T(")")), MB_ICONERROR);
	else
		sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
}

// 封禁
void CExplorerDlg::OnBnClickedButton2()
{
	int tabIndex = m_tab.GetCurSel();
	POSITION pos = m_pages[tabIndex]->m_list.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;
	int index = m_pages[tabIndex]->m_list.GetNextSelectedItem(pos);


	CString author;
	get_ip_tbs banTBS;
	BOOL result;
	if (tabIndex == 0) // 主题
	{
		author = m_exploreThreadPage.m_threads[index].author;
		result = GetBanTBS(m_exploreThreadPage.m_threads[index].tid, author, banTBS);
	}
	else if (tabIndex == 1) // 帖子
	{
		author = m_explorePostPage.m_posts[index].author;
		result = GetBanTBS(m_explorePostPage.m_tid, author, banTBS);
	}
	else // 楼中楼
	{
		author = m_exploreLzlPage.m_lzls[index].author;
		result = GetBanTBS(m_explorePostPage.m_tid, author, banTBS);
	}
	if (!result)
	{
		AfxMessageBox(_T("获取封禁口令号失败"), MB_ICONERROR);
		return;
	}


	CString code = BanID(author, banTBS.tbs_ban_user);
	if (code != _T("0"))
		AfxMessageBox(_T("封禁失败，错误代码" + code + _T("(") + GetTiebaErrorText(code) + _T(")")), MB_ICONERROR);
	else
		sndPlaySound(_T("封号.wav"), SND_ASYNC | SND_NODEFAULT);
}

// 浏览器
void CExplorerDlg::OnBnClickedButton3()
{
	CString url;
	if (m_tab.GetCurSel() == 0)
	{
		POSITION pos = m_exploreThreadPage.m_list.GetFirstSelectedItemPosition();
		if (pos == NULL)
			return;
		int index = m_exploreThreadPage.m_list.GetNextSelectedItem(pos);
		url = _T("http://tieba.baidu.com/p/") + m_exploreThreadPage.m_threads[index].tid;
	}
	else
	{
		POSITION pos = m_exploreThreadPage.m_list.GetFirstSelectedItemPosition();
		if (pos == NULL)
			return;
		int index = m_exploreThreadPage.m_list.GetNextSelectedItem(pos);
		CString page;
		m_exploreThreadPage.m_edit.GetWindowText(page);
		url = _T("http://tieba.baidu.com/p/") + m_explorePostPage.m_tid + _T("?pn=") + page;
	}
	ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_NORMAL);
}
