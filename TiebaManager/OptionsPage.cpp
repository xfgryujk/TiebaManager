// OptionsPage.cpp : 实现文件
//

#include "stdafx.h"
#include "OptionsPage.h"
#include "Setting.h"
#include "SettingDlg.h"


// COptionsPage 对话框

IMPLEMENT_DYNAMIC(COptionsPage, CNormalDlg)

COptionsPage::COptionsPage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(COptionsPage::IDD, pParent)
{

}

#pragma region MFC
COptionsPage::~COptionsPage()
{
}

void COptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_BUTTON1, m_newOptionsButton);
	DDX_Control(pDX, IDC_BUTTON2, m_deleteOptionsButton);
	DDX_Control(pDX, IDC_BUTTON3, m_renameOptionsButton);
	DDX_Control(pDX, IDC_BUTTON6, m_loadOptionsButton);
	DDX_Control(pDX, IDC_BUTTON4, m_saveOptionsButton);
	DDX_Control(pDX, IDC_STATIC1, m_currentOptionStatic);
}


BEGIN_MESSAGE_MAP(COptionsPage, CNormalDlg)
	ON_LBN_DBLCLK(IDC_LIST1, &COptionsPage::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON1, &COptionsPage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &COptionsPage::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &COptionsPage::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON6, &COptionsPage::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON4, &COptionsPage::OnBnClickedButton4)
END_MESSAGE_MAP()
#pragma endregion

// COptionsPage 消息处理程序

// 初始化
BOOL COptionsPage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	m_resize.AddControl(&m_currentOptionStatic, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_list, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_edit, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_newOptionsButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_deleteOptionsButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_renameOptionsButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_loadOptionsButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_saveOptionsButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 双击列表
void COptionsPage::OnLbnDblclkList1()
{
	CString tmp;
	m_list.GetText(m_list.GetCurSel(), tmp);
	m_edit.SetWindowText(tmp);
}

// 新建
void COptionsPage::OnBnClickedButton1()
{
	CString name;
	m_edit.GetWindowText(name);
	if (name == _T(""))
	{
		AfxMessageBox(_T("方案名不能为空！"), MB_ICONERROR);
		return;
	}
	if (!PathFileExists(OPTIONS_PATH))
		CreateDirectory(OPTIONS_PATH, NULL);
	if (PathFileExists(OPTIONS_PATH + name + _T(".tb")))
	{
		AfxMessageBox(_T("方案已存在！"), MB_ICONERROR);
		return;
	}
	m_list.SetCurSel(m_list.AddString(name));
	OnBnClickedButton4();
	OnBnClickedButton6();
}

// 删除
void COptionsPage::OnBnClickedButton2()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	CString name;
	m_list.GetText(index, name);
	if (!DeleteFile(OPTIONS_PATH + name + _T(".tb")))
	{
		AfxMessageBox(_T("删除方案失败！"), MB_ICONERROR);
		return;
	}
	m_list.DeleteString(index);
	m_list.SetCurSel(index == 0 ? 0 : index - 1);
	if (m_list.GetCurSel() == LB_ERR)
	{
		ReadOptions(_T(""));
		((CSettingDlg*)GetParent()->GetParent())->ShowCurrentOptions();
	}
	else
		OnBnClickedButton6();
}

// 重命名
void COptionsPage::OnBnClickedButton3()
{
	CString newName;
	m_edit.GetWindowText(newName);
	if (newName == _T(""))
	{
		AfxMessageBox(_T("方案名不能为空！"), MB_ICONERROR);
		return;
	}
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	CString name;
	m_list.GetText(index, name);
	if (!MoveFile(OPTIONS_PATH + name + _T(".tb"), OPTIONS_PATH + newName + _T(".tb")))
	{
		AfxMessageBox(_T("重命名方案失败！"), MB_ICONERROR);
		return;
	}
	if (g_currentOption == name)
	{
		g_currentOption = newName;
		m_currentOptionStatic.SetWindowText(_T("当前方案：") + newName);
	}
	m_list.DeleteString(index);
	m_list.InsertString(index, newName);
	m_list.SetCurSel(index);
}

// 载入
void COptionsPage::OnBnClickedButton6()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
	{
		AfxMessageBox(_T("请选择一个方案！"), MB_ICONERROR);
		return;
	}
	CString name;
	m_list.GetText(index, name);
	((CSettingDlg*)GetParent()->GetParent())->ShowOptionsInFile(OPTIONS_PATH + name + _T(".tb"));
	m_currentOptionStatic.SetWindowText(_T("当前方案：") + name);
}

// 保存
void COptionsPage::OnBnClickedButton4()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
	{
		AfxMessageBox(_T("请选择一个方案！"), MB_ICONERROR);
		return;
	}
	if (!PathFileExists(OPTIONS_PATH))
		CreateDirectory(OPTIONS_PATH, NULL);
	CString name;
	m_list.GetText(index, name);
	((CSettingDlg*)GetParent()->GetParent())->SaveOptionsInDlg(OPTIONS_PATH + name + _T(".tb"));
}
