#include "stdafx.h"
#include "ScanImage.h"
#include "Setting.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\imgcodecs.hpp>
#include <regex>
using std::wregex;


set<CString> g_leagalImage; // 已检查不违规的图片
set<CString> g_illegalImage; // 已检查违规的图片


// 1是图片地址
const wregex THREAD_IMG_REG(_T("<img .*?bpic=\"(.*?)\".*?/>"));
// 2是图片地址
const wregex POST_IMG_REG(_T("<img .*?class=\"(BDE_Image|j_user_sign)\".*?src=\"(.*?)\".*?>"));


// 从目录读取图片到g_images
void ReadImages(const CString& dir)
{
	vector<CString> imagePath;

	if (dir == _T(""))
	{
		g_images.clear();
		return;
	}
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

	g_images.resize(imagePath.size());
	UINT i;
	for (i = 0; i < imagePath.size(); i++)
	{
		g_images[i].name = GetImageName(imagePath[i]);
		g_images[i].img = cv::imread((LPCSTR)(CStringA)imagePath[i]);
		if (g_images[i].img.data == NULL)
			i--;
	}
	g_images.resize(i);
}

// 从图片地址取图片名
CString GetImageName(const CString& img)
{
	LPTSTR pos = StrRChr(img, NULL, _T('/'));
	CString imgName = (pos == NULL ? img : pos + 1);
	int right = imgName.Find(_T("?"));
	return right == -1 ? imgName : imgName.Left(right);
}


// 从主题预览取图片地址
void GetThreadImage(const CString& preview, vector<CString>& img)
{
	for (std::regex_iterator<LPCTSTR> it((LPCTSTR)preview, (LPCTSTR)preview
		+ preview.GetLength(), THREAD_IMG_REG), end; it != end; it++)
		img.push_back((*it)[1].str().c_str());
}

// 从帖子取图片地址
void GetPostImage(const CString& content, vector<CString>& img)
{
	for (std::regex_iterator<LPCTSTR> it((LPCTSTR)content, (LPCTSTR)content
		+ content.GetLength(), POST_IMG_REG), end; it != end; it++)
		img.push_back((*it)[2].str().c_str());
}


// 检查图片违规1，检测信任用户、获取图片地址
BOOL CheckImageIllegal(const CString& content, const CString& author, void(*GetImage)(const CString& content,
	vector<CString>& img), CString& msg)
{
	if (g_images.empty())
		return FALSE;

	g_optionsLock.Lock();
	// 信任用户
	for (const CString& whiteList : g_whiteList)
	if (author == whiteList)
	{
		g_optionsLock.Unlock();
		return FALSE;
	}
	g_optionsLock.Unlock();

	vector<CString> imgs;
	GetImage(content, imgs);
	return DoCheckImageIllegal(imgs, msg);
}

// SSIM算法比较图片
static double getMSSIM(const Mat& i1, const Mat& i2)
{
	const double C1 = 6.5025, C2 = 58.5225;
	/***************************** INITS **********************************/
	int d = CV_32F;

	Mat I1, I2;
	i1.convertTo(I1, d);           // 不能在单字节像素上进行计算，范围不够。
	i2.convertTo(I2, d);

	if (I1.rows * I1.cols < I2.rows * I2.cols)
		cv::resize(I2, I2, I1.size());
	else
		cv::resize(I1, I1, I2.size());

	Mat I2_2 = I2.mul(I2);        // I2^2
	Mat I1_2 = I1.mul(I1);        // I1^2
	Mat I1_I2 = I1.mul(I2);        // I1 * I2

	/***********************初步计算 ******************************/

	Mat mu1, mu2;
	GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
	I1.release();
	GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);
	I2.release();

	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);
	mu1.release();
	mu2.release();

	Mat sigma1_2, sigma2_2, sigma12;

	GaussianBlur(I1_2, sigma1_2, cv::Size(11, 11), 1.5);
	I1_2.release();
	sigma1_2 -= mu1_2;

	GaussianBlur(I2_2, sigma2_2, cv::Size(11, 11), 1.5);
	I2_2.release();
	sigma2_2 -= mu2_2;

	GaussianBlur(I1_I2, sigma12, cv::Size(11, 11), 1.5);
	I1_I2.release();
	sigma12 -= mu1_mu2;

	///////////////////////////////// 公式 ////////////////////////////////
	Mat t1, t2, t3;

	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))
	t2.release();

	Mat ssim_map;
	divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;
	t1.release();
	t3.release();

	cv::Scalar mssim = mean(ssim_map); // mssim = ssim_map的平均值
	return mssim.val[0] + mssim.val[1] + mssim.val[2];
}

// 检查图片违规2，下载图片、比较图片
BOOL DoCheckImageIllegal(vector<CString>& imgs, CString& msg)
{
	for (const CString& img : imgs)
	{
		CString imgName = GetImageName(img);

		// 检查缓存结果
		if (g_leagalImage.find(imgName) != g_leagalImage.end())
			continue;
		if (g_illegalImage.find(imgName) != g_illegalImage.end())
			return TRUE;

		// 读取图片
		Mat image;
		if (PathFileExists(IMG_CACHE_PATH + imgName))
		{
			// 读取图片缓存
			image = cv::imread((LPCSTR)(CStringA)(IMG_CACHE_PATH + imgName));
		}
		else
		{
			// 下载图片
			BYTE* buffer;
			ULONG size;
			if (HTTPGetRaw(img, &buffer, &size) == NET_SUCCESS)
			{
				vector<BYTE> _imgBuffer(buffer, buffer + size);
				image = cv::imdecode(Mat(_imgBuffer), cv::IMREAD_COLOR);

				if (!PathFileExists(IMG_CACHE_PATH))
					CreateDirectory(IMG_CACHE_PATH, NULL);
				CFile file;
				if (file.Open(IMG_CACHE_PATH + imgName, CFile::modeCreate | CFile::modeWrite))
				file.Write(buffer, size);
				delete buffer;
			}
		}

		if (image.data == NULL)
			continue;
		// 判断和违规图片比较大于阈值
		g_optionsLock.Lock();
		for (const NameImage i : g_images)
		{
			double mssim = getMSSIM(image, i.img);
			if (mssim > g_SSIMThreshold)
			{
				msg.Format(_T("<font color=red> 触发违规图片 </font>%s<font color=red> 相似度%.3lf</font>"),
					i.name, mssim);
				g_illegalImage.insert(imgName);
				g_optionsLock.Unlock();
				return TRUE;
			}
		}
		g_leagalImage.insert(imgName);
		g_optionsLock.Unlock();
	}

	return FALSE;
}
