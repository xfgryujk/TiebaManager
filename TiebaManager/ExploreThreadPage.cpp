// ExploreThreadPage.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "ExploreThreadPage.h"
#include "afxdialogex.h"


// CExploreThreadPage 对话框

IMPLEMENT_DYNAMIC(CExploreThreadPage, CExplorerPage)

CExploreThreadPage::CExploreThreadPage(CWnd* pParent /*=NULL*/)
	: CExplorerPage(pParent)
{

}

CExploreThreadPage::~CExploreThreadPage()
{
}

void CExploreThreadPage::DoDataExchange(CDataExchange* pDX)
{
	CExplorerPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExploreThreadPage, CExplorerPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CExploreThreadPage::OnBnClickedButton1)
END_MESSAGE_MAP()


// CExploreThreadPage 消息处理程序

BOOL CExploreThreadPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_edit.SetWindowText(_T("1"));

	int i = 0;
	m_list.InsertColumn(i++, _T("回复"), LVCFMT_CENTER, 30);
	m_list.InsertColumn(i++, _T("标题"), LVCFMT_CENTER, 300);
	m_list.InsertColumn(i++, _T("作者"), LVCFMT_CENTER, 50);

	OnBnClickedButton1();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 转到
void CExploreThreadPage::OnBnClickedButton1()
{
	CString sPage;
	m_edit.GetWindowText(sPage);
	int iPage = _ttoi(sPage);
	if (iPage < 1)
	{
		m_edit.SetWindowText(_T("1"));
		iPage = 1;
	}
	CString ignoreThread; // 忽略前几个主题
	ignoreThread.Format(_T("%d"), (iPage - 1) * 50);

	GetThreads(g_forumName, ignoreThread, m_threads);
	m_list.DeleteAllItems();
	for (const ThreadInfo& i : m_threads)
	{
		int index = m_list.GetItemCount() - 1;
		m_list.InsertItem(index, i.reply);
		m_list.SetItemText(index, 1, i.title);
		m_list.SetItemText(index, 2, i.author);
	}
}
