// ExploreThreadPage.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "ExplorePostPage.h"
#include "afxdialogex.h"
#include "Tieba.h"
#include "ExplorerDlg.h"
#include "ExploreLzlPage.h"


// CExplorePostPage 对话框

IMPLEMENT_DYNAMIC(CExplorePostPage, CExplorerPage)

CExplorePostPage::CExplorePostPage(CWnd* pParent /*=NULL*/)
	: CExplorerPage(pParent)
{

}

#pragma region MFC
CExplorePostPage::~CExplorePostPage()
{
}

void CExplorePostPage::DoDataExchange(CDataExchange* pDX)
{
	CExplorerPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExplorePostPage, CExplorerPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CExplorePostPage::OnBnClickedButton1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CExplorePostPage::OnItemchangedList1)
END_MESSAGE_MAP()
#pragma endregion

// CExplorePostPage 消息处理程序

// 初始化
BOOL CExplorePostPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_edit.SetWindowText(_T("1"));

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	int i = 0;
	m_list.InsertColumn(i++, _T(""), LVCFMT_LEFT, 0);
	m_list.InsertColumn(i++, _T("楼层"), LVCFMT_RIGHT, 50);
	m_list.InsertColumn(i++, _T("内容"), LVCFMT_LEFT, 540);
	m_list.InsertColumn(i++, _T("作者"), LVCFMT_CENTER, 130);
	m_list.DeleteColumn(0); // 解决第一列文字不能右对齐的问题

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 转到
void CExplorePostPage::OnBnClickedButton1()
{
	if (m_tid == _T(""))
		return;
	m_gotoButton.EnableWindow(FALSE);

	CString sPage;
	m_edit.GetWindowText(sPage);
	if (_ttoi(sPage) < 1)
	{
		m_edit.SetWindowText(_T("1"));
		sPage = _T("1");
	}


	CExplorerDlg* parentDlg = (CExplorerDlg*)GetParent()->GetParent();
	CExploreLzlPage& exploreLzlPage = parentDlg->m_exploreLzlPage;
	GetPosts(m_tid, _T(""), sPage, m_posts, exploreLzlPage.m_lzls);
	m_list.DeleteAllItems();
	parentDlg->m_edit.SetWindowText(_T(""));
	for (const PostInfo& i : m_posts)
	{
		int index = m_list.GetItemCount();
		m_list.InsertItem(index, i.floor);
		m_list.SetItemText(index, 1, i.content);
		m_list.SetItemText(index, 2, i.author);
	}
	exploreLzlPage.m_list.DeleteAllItems();
	for (const PostInfo& i : exploreLzlPage.m_lzls)
	{
		int index = exploreLzlPage.m_list.GetItemCount();
		exploreLzlPage.m_list.InsertItem(index, i.floor);
		exploreLzlPage.m_list.SetItemText(index, 1, i.content);
		exploreLzlPage.m_list.SetItemText(index, 2, i.author);
	}

	m_gotoButton.EnableWindow(TRUE);
}

// 选中项改变
void CExplorePostPage::OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if (pNMLV->iItem != LB_ERR)
		((CExplorerDlg*)GetParent()->GetParent())->m_edit.SetWindowText(m_posts[pNMLV->iItem].content + 
			_T("\r\n\r\n") + m_posts[pNMLV->iItem].author);

	*pResult = 0;
}
