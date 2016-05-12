// ImageViewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageViewDlg.h"
#include <ImageHelper.h>
#include "ScanImage.h"
#include "NetworkHelper.h"
#include <MiscHelper.h>
#include "InputDlg.h"


// CImageViewDlg 对话框

IMPLEMENT_DYNAMIC(CImageViewDlg, CNormalDlg)

// 构造函数
CImageViewDlg::CImageViewDlg(CImageViewDlg** pThis, CWnd* pParent /*=NULL*/)
	: CNormalDlg(CImageViewDlg::IDD, pParent)
{
	m_pThis = pThis;
	m_imageURL = NULL;
}

#pragma region MFC
CImageViewDlg::~CImageViewDlg()
{
}

void CImageViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_imageStatic);
	DDX_Control(pDX, IDC_BUTTON3, m_saveButton);
	DDX_Control(pDX, IDC_SCROLLBAR1, m_imageScrollBar);
}


BEGIN_MESSAGE_MAP(CImageViewDlg, CNormalDlg)
	ON_WM_CLOSE()
	ON_WM_GETMINMAXINFO()
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BUTTON3, &CImageViewDlg::OnBnClickedButton3)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
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

// 滚动
void CImageViewDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode != SB_ENDSCROLL)
	{
		if (pScrollBar->GetDlgCtrlID() == m_imageScrollBar.GetDlgCtrlID())
		{
			m_imageScrollBar.SetScrollPos(nPos);
			m_imageStatic.Invalidate();
		}
	}

	CNormalDlg::OnVScroll(nSBCode, nPos, pScrollBar);
}

// 滚轮滚动
BOOL CImageViewDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	m_imageScrollBar.SetScrollPos(m_imageScrollBar.GetScrollPos() - zDelta);
	m_imageStatic.Invalidate();

	return CNormalDlg::OnMouseWheel(nFlags, zDelta, pt);
}

// 改变尺寸
void CImageViewDlg::OnSize(UINT nType, int cx, int cy)
{
	CNormalDlg::OnSize(nType, cx, cy);

	UpdateScrollRange();
}

// 更新滚动条范围
void CImageViewDlg::UpdateScrollRange()
{
	if (m_imageScrollBar.m_hWnd == NULL)
		return;

	// 计算总高度
	int totalHeight = 0;
	for (const CImage& i : m_image)
		totalHeight += GetImageSize(i).cy;
	RECT rect;
	m_imageStatic.GetWindowRect(&rect);
	m_imageScrollBar.SetScrollRange(0, MAX(0, totalHeight - (int)(rect.bottom - rect.top)));
	m_imageScrollBar.SetScrollPos(0);

	m_imageStatic.Invalidate();
}
#pragma endregion

// 初始化
BOOL CImageViewDlg::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	SetClassLong(m_imageStatic.m_hWnd, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_ARROW));

	m_resize.AddControl(&m_imageStatic, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_imageScrollBar, RT_KEEP_DIST_TO_RIGHT, &m_imageStatic, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_saveButton, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_imageStatic);

	RECT rect;
	GetParent()->GetWindowRect(&rect);
	SetWindowPos(NULL, rect.right, rect.top, 0, 0, SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 设置图片
void CImageViewDlg::SetImages(unique_ptr<vector<CString> > imageURL)
{
	m_imageURL = std::move(imageURL);
	m_image.resize(m_imageURL->size());

	// 读取图片到m_image
	for (UINT i = 0; i < m_imageURL->size(); i++)
	{
		CString imgName = GetImageName((*m_imageURL)[i]);
		if (PathFileExists(IMG_CACHE_PATH + imgName))
		{
			// 读取图片缓存
			if (!m_image[i].IsNull())
				m_image[i].Destroy();
			m_image[i].Load(IMG_CACHE_PATH + imgName);
		}
		else
		{
			// 下载图片
			unique_ptr<BYTE[]> buffer;
			ULONG size;
			if (HTTPGetRaw((*m_imageURL)[i], &buffer, &size) == NET_SUCCESS)
			{
				ReadImage(buffer.get(), size, m_image[i]);

				CreateDir(IMG_CACHE_PATH);
				CFile file;
				if (file.Open(IMG_CACHE_PATH + imgName, CFile::modeCreate | CFile::modeWrite))
					file.Write(buffer.get(), size);
			}
		}
	}

	UpdateScrollRange();
}

// 取图片显示的尺寸
SIZE CImageViewDlg::GetImageSize(const CImage& image)
{
	CRect rect;
	m_imageStatic.GetWindowRect(rect);
	if (image.GetWidth() <= rect.Width())
		return SIZE{ image.GetWidth(), image.GetHeight() };
	else
	{
		float scale = (float)rect.Width() / image.GetWidth();
		return SIZE{ rect.Width(), (int)(image.GetHeight() * scale) };
	}
}

// 画图片
void CImageViewDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == m_imageStatic.GetDlgCtrlID())
	{
		CImage buffer;
		buffer.Create(lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top, 32);
		HDC dc = buffer.GetDC();
		SetStretchBltMode(dc, HALFTONE);

		FillRect(dc, &lpDrawItemStruct->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH));
		CRect rect;
		m_imageStatic.GetWindowRect(rect);
		int yOffset = -m_imageScrollBar.GetScrollPos();
		for (const CImage& i : m_image)
		{
			if (i.IsNull())
				continue;
			SIZE size = GetImageSize(i);
			if (yOffset >= rect.Height())
				break;
			if (yOffset + size.cy > 0)
				i.Draw(dc, 0, yOffset, size.cx, size.cy);

			yOffset += size.cy;
		}
		FrameRect(dc, &lpDrawItemStruct->rcItem, (HBRUSH)GetStockObject(BLACK_BRUSH));

		buffer.ReleaseDC();
		buffer.Draw(lpDrawItemStruct->hDC, 0, 0);
		return;
	}

	CNormalDlg::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

// 保存
void CImageViewDlg::OnBnClickedButton3()
{
	CString content = _T("1");
	CInputDlg dlg(_T("输入图片序号："), content, NULL, FALSE, CInputDlg::IDD, this);
	if (dlg.DoModal() != IDOK || content == _T(""))
		return;
	int index = _ttoi(content) - 1;
	if (index < 0 || (UINT)index >= m_imageURL->size())
		return;

	if (m_image[index].IsNull())
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
		
		CString imgName = GetImageName((*m_imageURL)[index]);
		m_image[index].Save(folder + _T("\\") + imgName);
	}
}
