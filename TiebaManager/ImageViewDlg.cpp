// ImageViewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageViewDlg.h"
#include "ScanImage.h"
#include "NetworkHelper.h"
#include "MiscHelper.h"


// CImageViewDlg 对话框

IMPLEMENT_DYNAMIC(CImageViewDlg, CNormalDlg)

// 构造函数
CImageViewDlg::CImageViewDlg(CImageViewDlg** pThis, CWnd* pParent /*=NULL*/)
	: CNormalDlg(CImageViewDlg::IDD, pParent)
{
	m_pThis = pThis;
	m_imageURL = NULL;
	m_curImageIndex = 0;
}

#pragma region MFC
CImageViewDlg::~CImageViewDlg()
{
}

void CImageViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_imageStatic);
	DDX_Control(pDX, IDC_BUTTON1, m_prevButton);
	DDX_Control(pDX, IDC_BUTTON2, m_nextButton);
	DDX_Control(pDX, IDC_BUTTON3, m_saveButton);
}


BEGIN_MESSAGE_MAP(CImageViewDlg, CNormalDlg)
	ON_WM_CLOSE()
	ON_WM_GETMINMAXINFO()
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BUTTON1, &CImageViewDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CImageViewDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CImageViewDlg::OnBnClickedButton3)
END_MESSAGE_MAP()
#pragma endregion

// CImageViewDlg 消息处理程序

#pragma region UI
// 销毁窗口
void CImageViewDlg::OnClose()
{
	DestroyWindow();
}

// 释放this
void CImageViewDlg::PostNcDestroy()
{
	CNormalDlg::PostNcDestroy();

	*m_pThis = NULL;
	delete this;
}

// 限制最小尺寸
void CImageViewDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	/*lpMMI->ptMinTrackSize.x = 275;
	lpMMI->ptMinTrackSize.y = 275;*/

	CNormalDlg::OnGetMinMaxInfo(lpMMI);
}
#pragma endregion

// 初始化
BOOL CImageViewDlg::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	SetClassLong(m_imageStatic.m_hWnd, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_ARROW));

	m_resize.AddControl(&m_imageStatic, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_prevButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_imageStatic);
	m_resize.AddControl(&m_nextButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_imageStatic);
	m_resize.AddControl(&m_saveButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_imageStatic);

	RECT rect;
	GetParent()->GetWindowRect(&rect);
	SetWindowPos(NULL, rect.right, rect.top, 0, 0, SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 设置图片
void CImageViewDlg::SetImages(unique_ptr<vector<CString> >& imageURL)
{
	m_imageURL = std::move(imageURL);
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
		unique_ptr<BYTE[]> buffer;
		ULONG size;
		if (HTTPGetRaw((*m_imageURL)[m_curImageIndex], &buffer, &size) == NET_SUCCESS)
		{
			ReadImage(buffer.get(), size, m_curImage);

			CreateDir(IMG_CACHE_PATH);
			CFile file;
			if (file.Open(IMG_CACHE_PATH + imgName, CFile::modeCreate | CFile::modeWrite))
				file.Write(buffer.get(), size);
		}
	}

	CString caption;
	caption.Format(_T("(%d/%d) %s"), m_curImageIndex + 1, m_imageURL->size(), (LPCTSTR)imgName);
	SetWindowText(caption);
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

	CNormalDlg::OnDrawItem(nIDCtl, lpDrawItemStruct);
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
		if (imgName.Right(4).CompareNoCase(_T(".jpg")) != 0)
			imgName += _T(".jpg");
		m_curImage.Save(folder + _T("\\") + imgName);
	}
}
