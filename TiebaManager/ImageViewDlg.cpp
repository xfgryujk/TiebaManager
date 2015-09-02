// ImageViewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageViewDlg.h"
#include "ScanImage.h"
#include "Global.h"


// CImageViewDlg 对话框

IMPLEMENT_DYNAMIC(CImageViewDlg, CDialog)

// 构造函数
CImageViewDlg::CImageViewDlg(CImageViewDlg** pThis, CWnd* pParent /*=NULL*/)
	: CDialog(CImageViewDlg::IDD, pParent)
{
	m_pThis = pThis;
	m_imageURL = NULL;
	m_curImageIndex = 0;
}

#pragma region MFC
CImageViewDlg::~CImageViewDlg()
{
	if (m_imageURL != NULL)
		delete m_imageURL;
}

void CImageViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_imageStatic);
	DDX_Control(pDX, IDC_BUTTON1, m_prevButton);
	DDX_Control(pDX, IDC_BUTTON2, m_nextButton);
	DDX_Control(pDX, IDC_BUTTON3, m_saveButton);
}


BEGIN_MESSAGE_MAP(CImageViewDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BUTTON1, &CImageViewDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CImageViewDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CImageViewDlg::OnBnClickedButton3)
END_MESSAGE_MAP()
#pragma endregion

// CImageViewDlg 消息处理程序

#pragma region UI
// 屏蔽Esc关闭窗口
void CImageViewDlg::OnCancel()
{
}

// 屏蔽回车关闭窗口
void CImageViewDlg::OnOK()
{
}

// 销毁窗口
void CImageViewDlg::OnClose()
{
	DestroyWindow();
}

// 释放this
void CImageViewDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();

	*m_pThis = NULL;
	delete this;
}

// 限制最小尺寸
void CImageViewDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 240;
	lpMMI->ptMinTrackSize.y = 240;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

// 改变尺寸
void CImageViewDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_imageStatic.m_hWnd == NULL)
		return;

	CRect rect;
	GetClientRect(&rect); // 默认452 * 384
	m_imageStatic.SetWindowPos(NULL, 0, 0, rect.Width() - 21, rect.Height() - 58, SWP_NOMOVE | SWP_NOREDRAW);
	int y = rect.Height() - 35;
	m_prevButton.SetWindowPos(NULL, 11, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_nextButton.SetWindowPos(NULL, 74, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_saveButton.SetWindowPos(NULL, 137, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);

	Invalidate();
}
#pragma endregion

// 初始化
BOOL CImageViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetClassLong(m_imageStatic.m_hWnd, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_ARROW));

	RECT rect;
	GetParent()->GetWindowRect(&rect);
	SetWindowPos(NULL, rect.right, rect.top, 0, 0, SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 设置图片
void CImageViewDlg::SetImages(vector<CString>* imageURL)
{
	if (m_imageURL != NULL)
		delete m_imageURL;
	m_imageURL = imageURL;
	SetCurImage(0);
}

// 设置当前图片
void CImageViewDlg::SetCurImage(int index)
{
	m_curImageIndex = index;
	if (m_curImageIndex < 0 || (UINT)m_curImageIndex >= m_imageURL->size())
	{
		if (!m_curImage.IsNull())
			m_curImage.Destroy();
		SetWindowText(_T(""));
		m_imageStatic.Invalidate();
		return;
	}

	CString imgName = GetImageName((*m_imageURL)[m_curImageIndex]);
	if (PathFileExists(IMG_CACHE_PATH + imgName))
	{
		// 读取图片缓存
		if (!m_curImage.IsNull())
			m_curImage.Destroy();
		m_curImage.Load(IMG_CACHE_PATH + imgName);
	}
	else
	{
		// 下载图片
		BYTE* buffer;
		ULONG size;
		if (HTTPGetRaw((*m_imageURL)[m_curImageIndex], &buffer, &size) == NET_SUCCESS)
		{
			ReadImage(buffer, size, m_curImage);

			if (!PathFileExists(IMG_CACHE_PATH))
				CreateDirectory(IMG_CACHE_PATH, NULL);
			CFile file;
			if (file.Open(IMG_CACHE_PATH + imgName, CFile::modeCreate | CFile::modeWrite))
				file.Write(buffer, size);
			delete buffer;
		}
	}

	SetWindowText(imgName);
	m_imageStatic.Invalidate();
}

// 画图片
void CImageViewDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == m_imageStatic.GetDlgCtrlID())
	{
		FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH));
		if (!m_curImage.IsNull())
		{
			SetStretchBltMode(lpDrawItemStruct->hDC, HALFTONE);
			float ratio1 = (float)(lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left) / m_curImage.GetWidth();
			float ratio2 = (float)(lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top) / m_curImage.GetHeight();
			float ratio = ratio1 < ratio2 ? ratio1 : ratio2;
			m_curImage.Draw(lpDrawItemStruct->hDC, 0, 0, (int)(m_curImage.GetWidth() * ratio), (int)(m_curImage.GetHeight() * ratio));
		}
		FrameRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)GetStockObject(BLACK_BRUSH));
		return;
	}

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

// 上一张
void CImageViewDlg::OnBnClickedButton1()
{
	int index = m_curImageIndex - 1;
	if (index < 0)
		index = m_imageURL->size() - 1;
	SetCurImage(index);
}

// 下一张
void CImageViewDlg::OnBnClickedButton2()
{
	int index = m_curImageIndex + 1;
	if ((UINT)index >= m_imageURL->size())
		index = 0;
	SetCurImage(index);
}

// 保存
void CImageViewDlg::OnBnClickedButton3()
{
	if (m_curImage.IsNull())
		return;

	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.lpszTitle = _T("保存目录：");
	bi.ulFlags = BIF_STATUSTEXT;
	LPITEMIDLIST pidlSel = SHBrowseForFolder(&bi);

	if (pidlSel != NULL)
	{
		CString folder;
		SHGetPathFromIDList(pidlSel, folder.GetBuffer(MAX_PATH));
		folder.ReleaseBuffer();
		
		CString imgName = GetImageName((*m_imageURL)[m_curImageIndex]);
		m_curImage.Save(folder + _T("\\") + imgName);
	}
}
