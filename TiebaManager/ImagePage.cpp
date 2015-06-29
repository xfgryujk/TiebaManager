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
	DDX_Control(pDX, IDC_BUTTON2, m_calcFeatureButton);
}


BEGIN_MESSAGE_MAP(CImagePage, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CImagePage::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CImagePage::OnBnClickedButton2)
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

// 计算特征值
void CImagePage::OnBnClickedButton2()
{
	vector<CString> imagePath;

	CString dir;
	m_dirEdit.GetWindowText(dir);
	if (dir == _T(""))
		return;
	CFileFind fileFind;
	static const TCHAR* IMG_EXT[] = { _T("\\*.jpg"), _T("\\*.png"), _T("\\*.jpeg"), _T("\\*.bmp") };
	for (int i = 0; i < _countof(IMG_EXT); i++)
	{
		BOOL flag = fileFind.FindFile(dir + IMG_EXT[i]);
		while (flag)
		{
			flag = fileFind.FindNextFile();
			imagePath.push_back(fileFind.GetFilePath());
		}
	}

	vector<ImageFeature> feature(imagePath.size());
	for (UINT i = 0; i < imagePath.size(); i++)
	{
		LPTSTR pos = StrRChr(imagePath[i], NULL, _T('\\'));
		feature[i].name = (pos == NULL ? imagePath[i] : pos + 1);
		CalculateFeature(imagePath[i], feature[i].feature);
	}
	WriteFeatures(dir + FEATURE_PATH, feature);

	CString msg;
	msg.Format(_T("已保存%d张图片"), feature.size());
	AfxMessageBox(msg);
	((CSettingDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}
