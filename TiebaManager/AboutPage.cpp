// AboutPage.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "AboutPage.h"
#include "afxdialogex.h"
#include "Global.h"


// CAboutPage 对话框

IMPLEMENT_DYNAMIC(CAboutPage, CDialog)

CAboutPage::CAboutPage(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutPage::IDD, pParent)
{

}

#pragma region MFC
CAboutPage::~CAboutPage()
{
}

void CAboutPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_STATIC1, m_checkUpdateStatic);
	DDX_Control(pDX, IDC_CHECK1, m_autoCheckUpdateCheck);
	DDX_Control(pDX, IDC_STATIC2, m_authorStatic);
}


BEGIN_MESSAGE_MAP(CAboutPage, CDialog)
	ON_WM_CLOSE()
	ON_STN_CLICKED(IDC_STATIC1, &CAboutPage::OnStnClickedStatic1)
	ON_STN_CLICKED(IDC_STATIC2, &CAboutPage::OnStnClickedStatic2)
	ON_WM_SIZE()
END_MESSAGE_MAP()
#pragma endregion

// CAboutPage 消息处理程序

// 初始化
BOOL CAboutPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_edit.SetWindowText(_T("目测是现在百度贴吧唯一的吧务管理工具\r\n\
\r\n\
不懂正则表达式的自己去百度一下~\r\n\
\r\n\
代码只转换了\"'&<>还有空格，其他以网页源码为准，如换行符为<br> (不注意的话会不匹配或匹配太多)"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

#pragma region UI
// 屏蔽Esc关闭窗口
void CAboutPage::OnCancel()
{
}

// 屏蔽回车关闭窗口
void CAboutPage::OnOK()
{
}

// 销毁窗口
void CAboutPage::OnClose()
{
	DestroyWindow();

	CDialog::OnClose();
}

// 改变尺寸
void CAboutPage::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_edit.m_hWnd == NULL)
		return;

	CRect rect;
	GetClientRect(&rect); // 默认446 * 287
	m_edit.SetWindowPos(NULL, 0, 0, rect.Width() - 19, rect.Height() - 45, SWP_NOMOVE | SWP_NOREDRAW);
	int y = rect.Height() - 28;
	m_checkUpdateStatic.SetWindowPos(NULL, 11, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_autoCheckUpdateCheck.SetWindowPos(NULL, 116, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_authorStatic.SetWindowPos(NULL, rect.Width() - 124, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);

	Invalidate();
}
#pragma endregion


// 检查更新
void CAboutPage::OnStnClickedStatic1()
{
	switch (CheckUpdate())
	{
	case UPDATE_NO_UPDATE:
		AfxMessageBox(_T("已经是最新版本"));
		break;
	case UPDATE_FAILED_TO_GET_FILE_ID:
		if (AfxMessageBox(_T("获取文件ID失败，手动更新？"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			ShellExecute(NULL, _T("open"), _T("http://pan.baidu.com/s/1hq86os8#dir/path=%2F%E6%88%91%E7%9A\
%84%E5%88%86%E4%BA%AB%2F%E7%99%BE%E5%BA%A6%E8%B4%B4%E5%90%A7%E7%9B%B8%E5%85%B3"), NULL, NULL, SW_NORMAL);
		break;
	case UPDATE_FAILED_TO_GET_LINK:
		if (AfxMessageBox(_T("获取下载地址失败，手动更新？"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			ShellExecute(NULL, _T("open"), _T("http://pan.baidu.com/s/1hq86os8#dir/path=%2F%E6%88%91%E7%9A\
%84%E5%88%86%E4%BA%AB%2F%E7%99%BE%E5%BA%A6%E8%B4%B4%E5%90%A7%E7%9B%B8%E5%85%B3"), NULL, NULL, SW_NORMAL);
		break;
	}
}

// 关于作者
void CAboutPage::OnStnClickedStatic2()
{
	ShellExecute(NULL, _T("open"), _T("http://tieba.baidu.com/i/37897560"), NULL, NULL, SW_NORMAL);
}
