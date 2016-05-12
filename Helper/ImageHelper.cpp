#include "stdafx.h"
#include <ImageHelper.h>
#include <opencv2\imgcodecs.hpp>


static BOOL CImageToMat(const CImage& image, Mat& img)
{
	img.create(image.GetHeight(), image.GetWidth(), CV_8UC3);
	if (img.data == NULL)
		return FALSE;

	// 支持24位、32位图
	int bpp = image.GetBPP() / 8;
	if (bpp < 3)
		return FALSE;
	for (int y = 0; y < image.GetHeight(); y++)
	{
		BYTE* src = (BYTE*)image.GetPixelAddress(0, y);
		BYTE* dst = img.ptr<BYTE>(y);
		for (int x = 0; x < image.GetWidth(); x++)
		{
			dst[0] = src[0];
			dst[1] = src[1];
			dst[2] = src[2];
			src += bpp;
			dst += 3;
		}
	}
	return TRUE;
}

// 从文件加载图片
HELPER_API BOOL ReadImage(const CString& path, Mat& img)
{
	img = cv::imread((LPCSTR)(CStringA)path);
	if (img.data != NULL)
		return TRUE;
	
	// 加载OpenCV不支持的格式(GIF)
	CImage image;
	image.Load(path);
	if (image.IsNull())
		return FALSE;

	return CImageToMat(image, img);
}

// 从内存加载图片
HELPER_API BOOL ReadImage(const BYTE* buffer, ULONG size, CImage& img)
{
	// 创建流
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, size);
	if (m_hMem == NULL)
		return FALSE;
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	if (pmem == NULL)
		return FALSE;
	memcpy(pmem, buffer, size);
	IStream* pstm = NULL;
	if (FAILED(CreateStreamOnHGlobal(m_hMem, FALSE, &pstm)) || pstm == NULL)
		return FALSE;

	// 加载到CImage
	if (!img.IsNull())
		img.Destroy();
	img.Load(pstm);
	
	// 释放流
	GlobalUnlock(m_hMem);
	pstm->Release();
	return !img.IsNull();
}

// 从内存加载图片
HELPER_API BOOL ReadImage(const BYTE* buffer, ULONG size, Mat& img)
{
	try
	{
		vector<BYTE> _imgBuffer(buffer, buffer + size);
		img = cv::imdecode(Mat(_imgBuffer), cv::IMREAD_COLOR);
	}
	catch (...)
	{
	}
	if (img.data != NULL)
		return TRUE;

	// 加载OpenCV不支持的格式(GIF)
	CImage image;
	if (!ReadImage(buffer, size, image))
		return FALSE;
	return CImageToMat(image, img);
}

// 从图片地址取图片名
HELPER_API CString GetImageName(const CString& img)
{
	LPTSTR pos = StrRChr(img, NULL, _T('/'));
	CString imgName = (pos == NULL ? img : pos + 1);
	int right = imgName.Find(_T("?"));
	if (right != -1)
		imgName = imgName.Left(right);
	if (imgName.Right(4).CompareNoCase(_T(".jpg")) != 0)
		imgName += _T(".jpg");
	return imgName;
}
