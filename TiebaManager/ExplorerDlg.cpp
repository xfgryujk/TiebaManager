// ExplorerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "ExplorerDlg.h"
#include "afxdialogex.h"
#include "TiebaManagerDlg.h"


// CExplorerDlg 对话框

IMPLEMENT_DYNAMIC(CExplorerDlg, CDialog)

// 构造函数
CExplorerDlg::CExplorerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExplorerDlg::IDD, pParent)
{
	// 初始化m_pages
	int i = 0;
	m_pages[i++] = &m_exploreThreadPage;
	//m_pages[i++] = &m_keywordsPage;
	//m_pages[i++] = &m_blackListPage;
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
	//m_tab.InsertItem(i++, _T("帖子"));
	//m_tab.InsertItem(i++, _T("楼中楼"));

	// 初始化各页
	m_exploreThreadPage.Create(IDD_EXPLORER_PAGE, &m_tab);

	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	m_pages[0]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	for (i = 1; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_HIDEWINDOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
