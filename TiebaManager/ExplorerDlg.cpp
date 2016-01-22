// ExplorerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageViewDlg.h"
#include "ExplorerDlg.h"
#include "TiebaManagerDlg.h"
#include "TiebaVariable.h"
#include "TiebaCollect.h"
#include "TiebaOperate.h"
#include <Mmsystem.h>


// CExplorerDlg 对话框

IMPLEMENT_DYNAMIC(CExplorerDlg, CNormalDlg)

// 构造函数
CExplorerDlg::CExplorerDlg(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CExplorerDlg::IDD, pParent),
	m_pagesResize(&m_tab)
{
	// 初始化m_pages
	int i = 0;
	m_pages[i++] = &m_exploreThreadPage;
	m_pages[i++] = &m_explorePostPage;
	m_pages[i++] = &m_exploreLzlPage;

	m_imageViewDlg = NULL;
}

#pragma region MFC
CExplorerDlg::~CExplorerDlg()
{
}

void CExplorerDlg::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_BUTTON1, m_deleteButton);
	DDX_Control(pDX, IDC_BUTTON2, m_banButton);
	DDX_Control(pDX, IDC_BUTTON3, m_explorerButton);
}


BEGIN_MESSAGE_MAP(CExplorerDlg, CNormalDlg)
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

// 销毁窗口
void CExplorerDlg::OnClose()
{
	DestroyWindow();
}

// 释放this
void CExplorerDlg::PostNcDestroy()
{
	CNormalDlg::PostNcDestroy();

	((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_explorerDlg = NULL;
	delete this;
}

// 限制最小尺寸
void CExplorerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 829;
	lpMMI->ptMinTrackSize.y = 588;

	CNormalDlg::OnGetMinMaxInfo(lpMMI);
}

// 改变尺寸
void CExplorerDlg::OnSize(UINT nType, int cx, int cy)
{
	CNormalDlg::OnSize(nType, cx, cy);
	m_pagesResize.Resize();
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
	CNormalDlg::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// 设置大图标
	SetIcon(hIcon, FALSE);			// 设置小图标

	// 初始化m_tab
	int i = 0;
	m_tab.InsertItem(i++, _T("主题"));
	m_tab.InsertItem(i++, _T("帖子"));
	m_tab.InsertItem(i++, _T("楼中楼"));

	// 初始化各页
#define CREATE_PAGE(page) page.Create(page.IDD, &m_tab)
	CREATE_PAGE(m_exploreThreadPage);
	CREATE_PAGE(m_explorePostPage);
	CREATE_PAGE(m_exploreLzlPage);

	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	m_pages[0]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	for (i = 1; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_HIDEWINDOW);

	m_resize.AddControl(&m_tab, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_edit, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_deleteButton, RT_KEEP_DIST_TO_RIGHT, &m_edit);
	m_resize.AddControl(&m_banButton, RT_KEEP_DIST_TO_RIGHT, &m_edit);
	m_resize.AddControl(&m_explorerButton, RT_KEEP_DIST_TO_RIGHT, &m_edit);
	for (i = 0; i < _countof(m_pages); i++)
		m_pagesResize.AddControl(m_pages[i], RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, &m_tab, RT_KEEP_DIST_TO_BOTTOM, &m_tab);

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
			g_userCache.m_deletedTID.insert(_ttoi64(thread.tid));
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


	CString author, pid;
	if (tabIndex == 0) // 主题
	{
		author = m_exploreThreadPage.m_threads[index].author;
		vector<PostInfo> posts, lzls;
		GetPosts(m_exploreThreadPage.m_threads[index].tid, _T(""), _T("1"), posts, lzls);
		if (posts.size() > 0)
			pid = posts[0].pid;
	}
	else if (tabIndex == 1) // 帖子
	{
		author = m_explorePostPage.m_posts[index].author;
		pid = m_explorePostPage.m_posts[index].pid;
	}
	else // 楼中楼
	{
		author = m_exploreLzlPage.m_lzls[index].author;
		pid = m_exploreLzlPage.m_lzls[index].pid;
	}


	if (pid == _T(""))
	{
		AfxMessageBox(_T("封禁失败(获取帖子ID失败)"), MB_ICONERROR);
		return;
	}
	CString code = BanID(author, pid);
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

// 打开浏览图片对话框
void CExplorerDlg::ViewImages(vector<CString>* img)
{
	if (m_imageViewDlg == NULL)
	{
		m_imageViewDlg = new CImageViewDlg(&m_imageViewDlg, this);
		m_imageViewDlg->Create(m_imageViewDlg->IDD, this);
	}
	m_imageViewDlg->SetImages(img);
}
