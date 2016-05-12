// UsersPage.cpp : 实现文件
//

#include "stdafx.h"
#include "UsersPage.h"
#include "LoginDlg.h"
#include <TBMConfig.h>
#include "ConfigHelper.h"
#include "TiebaVariable.h"
#include "SettingDlg.h"
#include "TiebaManagerDlg.h"
#include <MiscHelper.h>


// CUsersPage 对话框

IMPLEMENT_DYNAMIC(CUsersPage, CNormalDlg)

CUsersPage::CUsersPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CUsersPage::IDD, pParent)
{

}

#pragma region MFC
CUsersPage::~CUsersPage()
{
}

void CUsersPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_currentUserStatic);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_BUTTON1, m_loginButton);
	DDX_Control(pDX, IDC_BUTTON2, m_deleteButton);
	DDX_Control(pDX, IDC_BUTTON3, m_switchButton);
}


BEGIN_MESSAGE_MAP(CUsersPage, CNormalDlg)
	ON_BN_CLICKED(IDC_BUTTON1, &CUsersPage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CUsersPage::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CUsersPage::OnBnClickedButton3)
END_MESSAGE_MAP()
#pragma endregion

// CUsersPage 消息处理程序

// 初始化
BOOL CUsersPage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	m_resize.AddControl(&m_currentUserStatic, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_list, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_loginButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_deleteButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_switchButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);

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
	CreateDir(USERS_PATH + loginDlg.m_userName);

	// 保存Cookie
	CUserTiebaInfo ck;
	*ck.m_cookie = loginDlg.m_cookie;
	TRACE(_T("%s\n"), (LPCTSTR)*ck.m_cookie);
	if (!ck.Save(USERS_PATH + loginDlg.m_userName + _T("\\ck.xml")))
	{
		AfxMessageBox(_T("保存账号失败！"), MB_ICONERROR);
		return;
	}
	if (g_globalConfig.m_currentUser == loginDlg.m_userName)
		*g_userTiebaInfo.m_cookie = loginDlg.m_cookie;

	int index = m_list.FindStringExact(-1, loginDlg.m_userName);
	if (index == LB_ERR)
		index = m_list.AddString(loginDlg.m_userName);
	m_list.SetCurSel(index);
	if (g_globalConfig.m_currentUser == _T("[NULL]"))
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
	if (name == g_globalConfig.m_currentUser)
	{
		AfxMessageBox(_T("不能删除当前账号！"), MB_ICONERROR);
		return;
	}
	CString path = USERS_PATH + name + _T("\\");
	if (!DeleteFile(path + _T("ck.xml")))
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
	SetCurrentUser(name, TRUE);
	((CSettingDlg*)GetParent()->GetParent())->ShowPlan(g_plan);
	m_currentUserStatic.SetWindowText(_T("当前账号：") + g_globalConfig.m_currentUser);
}
