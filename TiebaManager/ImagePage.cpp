// ImagePage.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "ImagePage.h"
#include "afxdialogex.h"
#include "ScanImage.h"


// CImagePage 对话框

IMPLEMENT_DYNAMIC(CImagePage, CDialog)

CImagePage::CImagePage(CWnd* pParent /*=NULL*/)
	: CDialog(CImagePage::IDD, pParent)
{
	m_updateImage = FALSE;
}

#pragma region MFC
CImagePage::~CImagePage()
{
}

void CImagePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_dirEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_browseButton);
	DDX_Control(pDX, IDC_BUTTON2, m_updateButton);
	DDX_Control(pDX, IDC_EDIT5, m_thresholdEdit);
}


BEGIN_MESSAGE_MAP(CImagePage, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CImagePage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CImagePage::OnBnClickedButton2)
	ON_EN_KILLFOCUS(IDC_EDIT5, &CImagePage::OnEnKillfocusEdit5)
END_MESSAGE_MAP()
#pragma endregion

// CImagePage 消息处理程序

#pragma region UI
// 屏蔽Esc关闭窗口
void CImagePage::OnCancel()
{
}

// 屏蔽回车关闭窗口
void CImagePage::OnOK()
{
}

// 销毁窗口
void CImagePage::OnClose()
{
	DestroyWindow();

	CDialog::OnClose();
}
#pragma endregion

// 浏览
void CImagePage::OnBnClickedButton1()
{
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.lpszTitle = _T("违规图片目录：");
	bi.ulFlags = BIF_STATUSTEXT;
	LPITEMIDLIST pidlSel = SHBrowseForFolder(&bi);

	if (pidlSel != NULL)
	{
		CString folder;
		SHGetPathFromIDList(pidlSel, folder.GetBuffer(MAX_PATH));
		folder.ReleaseBuffer();
		m_dirEdit.SetWindowText(folder);
	}
}

// 更新图片
void CImagePage::OnBnClickedButton2()
{
	m_updateImage = TRUE;
	((CSettingDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

// 阈值
void CImagePage::OnEnKillfocusEdit5()
{
	CString tmp;
	m_thresholdEdit.GetWindowText(tmp);
	double threshold = _ttof(tmp);
	if (threshold < 2.0 || threshold > 3.0)
		m_thresholdEdit.SetWindowText(_T("2.43"));
}
