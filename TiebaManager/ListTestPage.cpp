// ListTestPage.cpp : 实现文件
//

#include "stdafx.h"
#include "ListTestPage.h"


// CListTestPage 对话框

IMPLEMENT_DYNAMIC(CListTestPage, CListPage)

CListTestPage::CListTestPage(CWnd* pParent /*=NULL*/)
	: CListPage(CListTestPage::IDD, pParent)
{
}

#pragma region MFC
CListTestPage::~CListTestPage()
{
}

void CListTestPage::DoDataExchange(CDataExchange* pDX)
{
	CListPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_testEdit);
	DDX_Control(pDX, IDC_BUTTON7, m_testButton);
}


BEGIN_MESSAGE_MAP(CListTestPage, CListPage)
	ON_BN_CLICKED(IDC_BUTTON7, &CListTestPage::OnClickedButton7)
END_MESSAGE_MAP()
#pragma endregion

// CListTestPage 消息处理程序

// 初始化
BOOL CListTestPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_resize.AddControl(&m_testEdit, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_testButton, RT_KEEP_DIST_TO_RIGHT, &m_static, RT_KEEP_DIST_TO_BOTTOM, &m_testEdit);

	m_testEdit.SetWindowText(_T("欲测试文本"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 测试
void CListTestPage::OnClickedButton7()
{
	int index = m_list.GetSelectionMark();
	if (index == LB_ERR)
		return;
	if (TestMatch(index))
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
