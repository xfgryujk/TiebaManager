// PrefPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PrefPage.h"
#include "SettingDlg.h"


// CPrefPage 对话框

IMPLEMENT_DYNAMIC(CPrefPage, CDialog)

CPrefPage::CPrefPage(CWnd* pParent /*=NULL*/)
	: CDialog(CPrefPage::IDD, pParent)
{

}

#pragma region MFC
CPrefPage::~CPrefPage()
{
}

void CPrefPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_scanIntervalEdit);
	DDX_Control(pDX, IDC_CHECK2, m_deleteCheck);
	DDX_Control(pDX, IDC_CHECK1, m_banIDCheck);
	DDX_Control(pDX, IDC_COMBO1, m_banDurationCombo);
	DDX_Control(pDX, IDC_EDIT3, m_trigCountEdit);
	DDX_Control(pDX, IDC_CHECK3, m_onlyScanTitleCheck);
	DDX_Control(pDX, IDC_EDIT4, m_deleteIntervalEdit);
	DDX_Control(pDX, IDC_CHECK4, m_confirmCheck);
	DDX_Control(pDX, IDC_EDIT5, m_scanPageCountEdit);
	DDX_Control(pDX, IDC_CHECK5, m_briefLogCheck);
	DDX_Control(pDX, IDC_EDIT2, m_threadCountEdit);
	DDX_Control(pDX, IDC_EDIT7, m_banReasonEdit);
}


BEGIN_MESSAGE_MAP(CPrefPage, CDialog)
	ON_WM_CLOSE()
	ON_EN_KILLFOCUS(IDC_EDIT1, &CPrefPage::OnEnKillfocusEdit1)
	ON_BN_CLICKED(IDC_CHECK1, &CPrefPage::OnBnClickedCheck1)
	ON_EN_KILLFOCUS(IDC_EDIT3, &CPrefPage::OnEnKillfocusEdit3)
	ON_EN_KILLFOCUS(IDC_EDIT4, &CPrefPage::OnEnKillfocusEdit4)
	ON_EN_KILLFOCUS(IDC_EDIT5, &CPrefPage::OnEnKillfocusEdit5)
	ON_EN_CHANGE(IDC_EDIT5, &CPrefPage::OnEnChangeEdit5)
	ON_EN_KILLFOCUS(IDC_EDIT2, &CPrefPage::OnEnKillfocusEdit2)
END_MESSAGE_MAP()
#pragma endregion

// CPrefPage 消息处理程序

#pragma region UI
// 屏蔽Esc关闭窗口
void CPrefPage::OnCancel()
{
}

// 屏蔽回车关闭窗口
void CPrefPage::OnOK()
{
}

// 销毁窗口
void CPrefPage::OnClose()
{
	DestroyWindow();

	CDialog::OnClose();
}
#pragma endregion

BOOL CPrefPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 初始化封禁时长
	m_banDurationCombo.AddString(_T("1"));
	m_banDurationCombo.AddString(_T("3"));
	m_banDurationCombo.AddString(_T("10"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 扫描间隔
void CPrefPage::OnEnKillfocusEdit1()
{
	CString tmp;
	m_scanIntervalEdit.GetWindowText(tmp);
	int scanInterval = _ttoi(tmp);
	if (scanInterval < 0 || scanInterval > 600)
		m_scanIntervalEdit.SetWindowText(_T("5"));
}

// 封ID
void CPrefPage::OnBnClickedCheck1()
{
	BOOL enable = m_banIDCheck.GetCheck();
	m_banDurationCombo.EnableWindow(enable);
	m_banReasonEdit.EnableWindow(enable);
	m_trigCountEdit.EnableWindow(enable);
}

// 封禁违规次数
void CPrefPage::OnEnKillfocusEdit3()
{
	CString tmp;
	m_trigCountEdit.GetWindowText(tmp);
	int trigCount = _ttoi(tmp);
	if (trigCount < 1)
		m_trigCountEdit.SetWindowText(_T("1"));
}

// 删帖间隔
void CPrefPage::OnEnKillfocusEdit4()
{
	CString tmp;
	m_deleteIntervalEdit.GetWindowText(tmp);
	float deleteInterval = (float)_ttof(tmp);
	if (deleteInterval < 0 || deleteInterval > 60)
		m_deleteIntervalEdit.SetWindowText(_T("1.5"));
}

// 扫描最后页数 失去焦点
void CPrefPage::OnEnKillfocusEdit5()
{
	CString tmp;
	m_scanPageCountEdit.GetWindowText(tmp);
	int scanPageCount = _ttoi(tmp);
	if (scanPageCount < 1)
		m_scanPageCountEdit.SetWindowText(_T("1"));
}

// 扫描最后页数 内容改变
void CPrefPage::OnEnChangeEdit5()
{
	((CSettingDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

// 线程数
void CPrefPage::OnEnKillfocusEdit2()
{
	CString tmp;
	m_threadCountEdit.GetWindowText(tmp);
	int threadCount = _ttoi(tmp);
	if (threadCount < 1 || threadCount > 16)
		m_threadCountEdit.SetWindowText(_T("2"));
}
