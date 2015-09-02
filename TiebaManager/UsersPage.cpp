// UsersPage.cpp : 实现文件
//

#include "stdafx.h"
#include "UsersPage.h"
#include "LoginDlg.h"
#include "Setting.h"
#include "SettingDlg.h"
#include "TiebaManagerDlg.h"


// CUsersPage 对话框

IMPLEMENT_DYNAMIC(CUsersPage, CDialog)

CUsersPage::CUsersPage(CWnd* pParent /*=NULL*/)
	: CDialog(CUsersPage::IDD, pParent)
{

}

#pragma region MFC
CUsersPage::~CUsersPage()
{
}

void CUsersPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_currentUserStatic);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_BUTTON1, m_loginButton);
	DDX_Control(pDX, IDC_BUTTON2, m_deleteButton);
	DDX_Control(pDX, IDC_BUTTON3, m_switchButton);
}


BEGIN_MESSAGE_MAP(CUsersPage, CDialog)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &CUsersPage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CUsersPage::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CUsersPage::OnBnClickedButton3)
END_MESSAGE_MAP()
#pragma endregion

// CUsersPage 消息处理程序

#pragma region UI
// 屏蔽Esc关闭窗口
void CUsersPage::OnCancel()
{
}

// 屏蔽回车关闭窗口
void CUsersPage::OnOK()
{
}

// 销毁窗口
void CUsersPage::OnClose()
{
	DestroyWindow();

	CDialog::OnClose();
}

// 改变尺寸
void CUsersPage::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_list.m_hWnd == NULL)
		return;

	CRect rect;
	GetClientRect(&rect); // 默认446 * 287
	m_currentUserStatic.SetWindowPos(NULL, 0, 0, rect.Width() - 21, 15, SWP_NOMOVE | SWP_NOREDRAW);
	m_list.SetWindowPos(NULL, 0, 0, rect.Width() - 21, rect.Height() - 84, SWP_NOMOVE | SWP_NOREDRAW);
	int y = rect.Height() - 39;
	m_loginButton.SetWindowPos(NULL, 11, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_deleteButton.SetWindowPos(NULL, 84, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_switchButton.SetWindowPos(NULL, 158, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);

	Invalidate();
}
#pragma endregion

// 初始化
BOOL CUsersPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 已确认贴吧
	if (!((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_confirmButton.IsWindowEnabled())
		m_switchButton.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 登录
void CUsersPage::OnBnClickedButton1()
{
	CLoginDlg loginDlg(this);
	if (loginDlg.DoModal() != IDOK)
		return;
	// 创建目录
	if (!PathFileExists(USERS_PATH))
		CreateDirectory(USERS_PATH, NULL);
	if (!PathFileExists(USERS_PATH + loginDlg.m_userName))
		CreateDirectory(USERS_PATH + loginDlg.m_userName, NULL);
	// 保存Cookie
	gzFile f = gzopen_w(USERS_PATH + loginDlg.m_userName + _T("\\ck.tb"), "wb");
	if (f != NULL)
	{
		WriteText(f, loginDlg.m_cookie);
		gzclose(f);
	}

	int index = m_list.FindStringExact(-1, loginDlg.m_userName);
	if (index == LB_ERR)
		index = m_list.AddString(loginDlg.m_userName);
	m_list.SetCurSel(index);
	if (g_currentUser == _T("[NULL]"))
		OnBnClickedButton3();
}

// 删除
void CUsersPage::OnBnClickedButton2()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	CString name;
	m_list.GetText(index, name);
	if (name == g_currentUser)
	{
		AfxMessageBox(_T("不能删除当前账号！"), MB_ICONERROR);
		return;
	}
	CString path = USERS_PATH + name + _T("\\");
	if (!DeleteFile(path + _T("ck.tb")) || !DeleteFile(path + _T("cache.tb")))
	{
		AfxMessageBox(_T("删除账号失败！"), MB_ICONERROR);
		return;
	}
	m_list.DeleteString(index);
	m_list.SetCurSel(index == 0 ? 0 : index - 1);
}

// 切换
void CUsersPage::OnBnClickedButton3()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	// 已确认贴吧
	if (!((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_confirmButton.IsWindowEnabled())
		return;
	CString name;
	m_list.GetText(index, name);
	SetCurrentUser(name);
	((CSettingDlg*)GetParent()->GetParent())->ShowCurrentOptions();
	m_currentUserStatic.SetWindowText(_T("当前账号：") + g_currentUser);
}
