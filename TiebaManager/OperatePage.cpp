// OperatePage.cpp : 实现文件
//

#include "stdafx.h"
#include "OperatePage.h"
#include "SettingDlg.h"


// COperatePage 对话框

IMPLEMENT_DYNAMIC(COperatePage, CNormalDlg)

COperatePage::COperatePage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(COperatePage::IDD, pParent)
{

}

#pragma region MFC
COperatePage::~COperatePage()
{
}

void COperatePage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK2, m_deleteCheck);
	DDX_Control(pDX, IDC_CHECK1, m_banIDCheck);
	DDX_Control(pDX, IDC_COMBO1, m_banDurationCombo);
	DDX_Control(pDX, IDC_EDIT3, m_banTrigCountEdit);
	DDX_Control(pDX, IDC_EDIT4, m_deleteIntervalEdit);
	DDX_Control(pDX, IDC_CHECK4, m_confirmCheck);
	DDX_Control(pDX, IDC_EDIT7, m_banReasonEdit);
	DDX_Control(pDX, IDC_CHECK3, m_defriendCheck);
	DDX_Control(pDX, IDC_EDIT8, m_defriendTrigCountEdit);
	DDX_Control(pDX, IDC_CHECK5, m_wapBanInterfaceCheck);
}


BEGIN_MESSAGE_MAP(COperatePage, CNormalDlg)
	ON_BN_CLICKED(IDC_CHECK1, &COperatePage::OnBnClickedCheck1)
	ON_EN_KILLFOCUS(IDC_EDIT3, &COperatePage::OnEnKillfocusEdit3)
	ON_EN_KILLFOCUS(IDC_EDIT4, &COperatePage::OnEnKillfocusEdit4)
	ON_EN_KILLFOCUS(IDC_EDIT8, &COperatePage::OnEnKillfocusEdit8)
	ON_BN_CLICKED(IDC_CHECK3, &COperatePage::OnBnClickedCheck3)
END_MESSAGE_MAP()
#pragma endregion

// COperatePage 消息处理程序

BOOL COperatePage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	// 初始化封禁时长
	m_banDurationCombo.AddString(_T("1"));
	m_banDurationCombo.AddString(_T("3"));
	m_banDurationCombo.AddString(_T("10"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 封ID
void COperatePage::OnBnClickedCheck1()
{
	BOOL enable = m_banIDCheck.GetCheck();
	m_banDurationCombo.EnableWindow(enable);
	m_banReasonEdit.EnableWindow(enable);
	m_banTrigCountEdit.EnableWindow(enable);
}

// 拉黑
void COperatePage::OnBnClickedCheck3()
{
	BOOL enable = m_defriendCheck.GetCheck();
	m_defriendTrigCountEdit.EnableWindow(enable);
}

// 封禁违规次数
void COperatePage::OnEnKillfocusEdit3()
{
	CString tmp;
	m_banTrigCountEdit.GetWindowText(tmp);
	int trigCount = _ttoi(tmp);
	if (trigCount < 1)
		m_banTrigCountEdit.SetWindowText(_T("1"));
}

// 拉黑违规次数
void COperatePage::OnEnKillfocusEdit8()
{
	CString tmp;
	m_defriendTrigCountEdit.GetWindowText(tmp);
	int trigCount = _ttoi(tmp);
	if (trigCount < 1)
		m_defriendTrigCountEdit.SetWindowText(_T("1"));
}

// 删帖间隔
void COperatePage::OnEnKillfocusEdit4()
{
	CString tmp;
	m_deleteIntervalEdit.GetWindowText(tmp);
	float deleteInterval = (float)_ttof(tmp);
	if (deleteInterval < 0 || deleteInterval > 60)
		m_deleteIntervalEdit.SetWindowText(_T("1.5"));
}
