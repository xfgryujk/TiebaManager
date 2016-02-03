// RegListPage.cpp : 实现文件
//

#include "stdafx.h"
#include "RegListPage.h"
#include "Setting.h"
#include "InputDlg.h"
#include "StringHelper.h"


// CRegListPage 对话框

IMPLEMENT_DYNAMIC(CRegListPage, CListTestPage)

CRegListPage::CRegListPage(const CString& inputTitle, CWnd* pParent /*=NULL*/)
	: CListTestPage(pParent), 
	m_inputTitle(inputTitle)
{
}

CRegListPage::~CRegListPage()
{
}

void CRegListPage::DoDataExchange(CDataExchange* pDX)
{
	CListTestPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRegListPage, CListTestPage)
END_MESSAGE_MAP()


// CRegListPage 消息处理程序


BOOL CRegListPage::OnInitDialog()
{
	CListTestPage::OnInitDialog();

	int i = 0;
	m_list.InsertColumn(i++, _T("内容"), LVCFMT_LEFT, 500);
	m_list.InsertColumn(i++, _T("正则"), LVCFMT_LEFT, 50);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

class CRegListFile : public CConfigBase
{
public:
	COption<vector<RegexText> > m_list;

	CRegListFile()
		: CConfigBase("RegList"),
		m_list("RegList")
	{
		m_options.push_back(&m_list);
	}
};

// 导出
void CRegListPage::OnClickedButton4()
{
	CFileDialog dlg(FALSE, _T("xml"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("XML文件 (*.xml)|*.xml|所有文件 (*.*)|*.*||"), this);
	if (dlg.DoModal() == IDOK)
	{
		CRegListFile tmp;
		ApplyList(tmp.m_list);
		if (!tmp.Save(dlg.GetPathName()))
			AfxMessageBox(_T("保存失败！"), MB_ICONERROR);
	}
}

// 导入
void CRegListPage::OnClickedButton5()
{
	CFileDialog dlg(TRUE, _T("xml"), NULL, 0,
		_T("XML文件 (*.xml)|*.xml|所有文件 (*.*)|*.*||"), this);
	if (dlg.DoModal() == IDOK)
	{
		CRegListFile tmp;
		if (!tmp.Load(dlg.GetPathName()))
			AfxMessageBox(_T("读取失败！"), MB_ICONERROR);
		else
			ShowList(tmp.m_list);
	}
}

BOOL CRegListPage::SetItem(int index)
{
	CString content = m_list.GetItemText(index, 0);
	BOOL isRegex = m_list.GetItemText(index, 1) == IS_REGEX_TEXT;
	CInputDlg dlg(m_inputTitle, content, &isRegex, TRUE, this);
	if (dlg.DoModal() == IDOK && content != _T(""))
	{
		if (CheckMatchTooMuch(content, isRegex))
		{
			AfxMessageBox(_T("匹配太多了！"), MB_ICONERROR);
			return FALSE;
		}
		m_list.SetItemText(index, 0, content);
		m_list.SetItemText(index, 1, isRegex ? IS_REGEX_TEXT : _T(""));
		PostChangeList();
		return TRUE;
	}
	return FALSE;
}

BOOL CRegListPage::TestMatch(int index)
{
	CString test;
	m_testEdit.GetWindowText(test);
	BOOL isRegex = m_list.GetItemText(index, 1) == IS_REGEX_TEXT;
	CString text = m_list.GetItemText(index, 0);
	return TestMatch(test, text, isRegex);
}

void CRegListPage::ShowList(const vector<RegexText>& list)
{
	m_list.DeleteAllItems();
	for (UINT i = 0; i < list.size(); i++)
	{
		m_list.InsertItem(i, list[i].text);
		m_list.SetItemText(i, 1, list[i].isRegex ? IS_REGEX_TEXT : _T(""));
	}
}

void CRegListPage::ApplyList(vector<RegexText>& list)
{
	int size = m_list.GetItemCount();
	list.resize(size);
	for (int i = 0; i < size; i++)
	{
		list[i].text = m_list.GetItemText(i, 0);
		list[i].isRegex = m_list.GetItemText(i, 1) == IS_REGEX_TEXT;
	}
}
