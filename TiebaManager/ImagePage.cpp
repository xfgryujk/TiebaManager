// ImagePage.cpp : 实现文件
//

#include "stdafx.h"
#include "ImagePage.h"
#include "SettingDlg.h"


// CImagePage 对话框

IMPLEMENT_DYNAMIC(CImagePage, CNormalDlg)

CImagePage::CImagePage(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CImagePage::IDD, pParent)
{
	g_plan.m_updateImage = FALSE;
}

#pragma region MFC
CImagePage::~CImagePage()
{
}

void CImagePage::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_dirEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_browseButton);
	DDX_Control(pDX, IDC_BUTTON2, m_updateButton);
	DDX_Control(pDX, IDC_EDIT5, m_thresholdEdit);
	DDX_Control(pDX, IDC_STATIC1, m_static);
}


BEGIN_MESSAGE_MAP(CImagePage, CNormalDlg)
	ON_BN_CLICKED(IDC_BUTTON1, &CImagePage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CImagePage::OnBnClickedButton2)
	ON_EN_KILLFOCUS(IDC_EDIT5, &CImagePage::OnEnKillfocusEdit5)
END_MESSAGE_MAP()
#pragma endregion

// CImagePage 消息处理程序

// 初始化
BOOL CImagePage::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	m_resize.AddControl(&m_dirEdit, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_browseButton, RT_KEEP_DIST_TO_RIGHT, &m_dirEdit, RT_NULL, NULL);
	m_resize.AddControl(&m_static, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

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
	g_plan.m_updateImage = TRUE;
	((CSettingDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

// 阈值
void CImagePage::OnEnKillfocusEdit5()
{
	CString tmp;
	m_thresholdEdit.GetWindowText(tmp);
	double threshold = _ttof(tmp);
	if (threshold < 1.0 || threshold > 3.0)
		m_thresholdEdit.SetWindowText(_T("2.43"));

	if (abs(threshold - g_plan.m_SSIMThreshold) > 0.00001)
	{
		g_plan.m_updateImage = TRUE;
		if (threshold < g_plan.m_SSIMThreshold)
			((CSettingDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
	}
}
