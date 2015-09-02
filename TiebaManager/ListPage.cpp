// ListPage.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "ListPage.h"
#include "afxdialogex.h"


// CListPage 对话框

IMPLEMENT_DYNAMIC(CListPage, CDialog)

CListPage::CListPage(CWnd* pParent /*=NULL*/)
	: CDialog(CListPage::IDD, pParent)
{
	m_staticColor = RGB(255, 0, 0);
}

#pragma region MFC
CListPage::~CListPage()
{
}

void CListPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_CHECK1, m_regexCheck);
	DDX_Control(pDX, IDC_BUTTON1, m_addButton);
	DDX_Control(pDX, IDC_BUTTON2, m_deleteButton);
	DDX_Control(pDX, IDC_BUTTON3, m_changeButton);
	DDX_Control(pDX, IDC_EDIT6, m_testEdit);
	DDX_Control(pDX, IDC_BUTTON6, m_testButton);
	DDX_Control(pDX, IDC_STATIC1, m_static);
}


BEGIN_MESSAGE_MAP(CListPage, CDialog)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_LBN_DBLCLK(IDC_LIST1, &CListPage::OnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON2, &CListPage::OnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CListPage::OnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CListPage::OnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON6, &CListPage::OnClickedButton6)
END_MESSAGE_MAP()
#pragma endregion

// CListPage 消息处理程序

// 初始化
BOOL CListPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (!HasRegex())
		m_regexCheck.ShowWindow(SW_HIDE);

	m_testEdit.SetWindowText(_T("欲测试文本"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

#pragma region UI
// 屏蔽Esc关闭窗口
void CListPage::OnCancel()
{
}

// 屏蔽回车关闭窗口
void CListPage::OnOK()
{
}

// 销毁窗口
void CListPage::OnClose()
{
	DestroyWindow();

	CDialog::OnClose();
}

// 改变尺寸
void CListPage::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_list.m_hWnd == NULL)
		return;

	CRect rect;
	GetClientRect(&rect); // 默认446 * 287
	m_list.SetWindowPos(NULL, 0, 0, rect.Width() - 21, rect.Height() - 191, SWP_NOMOVE | SWP_NOREDRAW);
	m_edit.SetWindowPos(NULL, 13, rect.Height() - 172, rect.Width() - 309, 26, SWP_NOREDRAW);
	int y = rect.Height() - 174;
	m_regexCheck.SetWindowPos(NULL, rect.Width() - 288, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_addButton.SetWindowPos(NULL, rect.Width() - 194, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_deleteButton.SetWindowPos(NULL, rect.Width() - 131, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_changeButton.SetWindowPos(NULL, rect.Width() - 68, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_testEdit.SetWindowPos(NULL, 13, rect.Height() - 139, rect.Width() - 21, 96, SWP_NOREDRAW);
	m_static.SetWindowPos(NULL, 11, rect.Height() - 43, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_testButton.SetWindowPos(NULL, rect.Width() - 68, rect.Height() - 39, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);

	Invalidate();
}

// 控件颜色
HBRUSH CListPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->m_hWnd == m_static.m_hWnd)
		pDC->SetTextColor(m_staticColor);

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
#pragma endregion

// 双击列表
void CListPage::OnDblclkList1()
{
	CString tmp;
	m_list.GetText(m_list.GetCurSel(), tmp);
	m_regexCheck.SetCheck(tmp.Left(REGEX_PREFIX_LENGTH) == IS_REGEX_PREFIX);
	if (HasRegex())
		m_edit.SetWindowText(tmp.Right(tmp.GetLength() - REGEX_PREFIX_LENGTH));
	else
		m_edit.SetWindowText(tmp);
}

// 添加
void CListPage::OnClickedButton1()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("内容不能为空！"), MB_ICONERROR);
		return;
	}
	BOOL isRegex = m_regexCheck.GetCheck();
	if (CheckMatchTooMuch(text, isRegex))
	{
		AfxMessageBox(_T("匹配太多啦！"), MB_ICONERROR);
		return;
	}
	int index = m_list.GetCurSel();
	if (HasRegex())
		index = m_list.InsertString(index + 1, (isRegex ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + text);
	else
		index = m_list.InsertString(index + 1, text);
	m_list.SetCurSel(index);
	PostChangeList();
}

// 删除
void CListPage::OnClickedButton2()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_list.DeleteString(index);
	m_list.SetCurSel(index == 0 ? 0 : index - 1);
}

// 修改
void CListPage::OnClickedButton3()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("内容不能为空！"), MB_ICONERROR);
		return;
	}
	BOOL isRegex = m_regexCheck.GetCheck();
	if (CheckMatchTooMuch(text, isRegex))
	{
		AfxMessageBox(_T("匹配太多啦！"), MB_ICONERROR);
		return;
	}
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_list.DeleteString(index);
	if (HasRegex())
		index = m_list.InsertString(index, (isRegex ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + text);
	else
		index = m_list.InsertString(index, text);
	m_list.SetCurSel(index);
	PostChangeList();
}

// 测试
void CListPage::OnClickedButton6()
{
	CString text;
	m_edit.GetWindowText(text);
	BOOL isRegex = m_regexCheck.GetCheck();
	CString test;
	m_testEdit.GetWindowText(test);
	if (TestMatch(test, text, isRegex))
	{
		m_staticColor = RGB(0, 255, 0);
		m_static.SetWindowText(_T("匹配成功！"));
	}
	else
	{
		m_staticColor = RGB(255, 0, 0);
		m_static.SetWindowText(_T("匹配失败，请检查匹配文本！"));
	}
}
