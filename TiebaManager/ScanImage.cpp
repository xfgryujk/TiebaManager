/*
Copyright (C) 2015  xfgryujk
http://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "stdafx.h"
#include "ScanImage.h"
#include <opencv2\imgproc\imgproc.hpp>

#include <ImageHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>

#include "TBMConfig.h"
#include "TBMConfigPath.h"
#include "TiebaManager.h"


// 检查图片违规1，检测信任用户、获取图片地址
BOOL CScanImage::CheckImageIllegal(const CString& author, std::function<void(std::vector<CString>&)> getImages, CString& msg)
{
	if (theApp.m_plan->m_images.empty())
		return FALSE;

	// 信任用户
	theApp.m_plan->m_optionsLock.Lock();
	if (theApp.m_plan->m_whiteList->find(author) != theApp.m_plan->m_whiteList->end())
	{
		theApp.m_plan->m_optionsLock.Unlock();
		return FALSE;
	}
	theApp.m_plan->m_optionsLock.Unlock();

	std::vector<CString> imgs;
	getImages(imgs);
	return DoCheckImageIllegal(imgs, msg);
}

// 检查图片违规2，下载图片、比较图片
BOOL CScanImage::DoCheckImageIllegal(std::vector<CString>& imgs, CString& msg)
{
	for (const CString& img : imgs)
	{
		CString imgName = GetImageName(img);

		// 检查缓存结果
		if (m_leagalImage.find(imgName) != m_leagalImage.end())
			continue;
		if (m_illegalImage.find(imgName) != m_illegalImage.end())
			return TRUE;

		// 读取图片
		cv::Mat image;
		if (PathFileExists(IMG_CACHE_PATH + imgName))
		{
			// 读取图片缓存
			ReadImage(IMG_CACHE_PATH + imgName, image);
		}
		else
		{
			// 下载图片
			std::unique_ptr<BYTE[]> buffer;
			ULONG size;
			if (HTTPGetRaw(img, &buffer, &size) == NET_SUCCESS)
			{
				ReadImage(buffer.get(), size, image);

				CreateDir(IMG_CACHE_PATH);
				CFile file;
				if (file.Open(IMG_CACHE_PATH + imgName, CFile::modeCreate | CFile::modeWrite))
					file.Write(buffer.get(), size);
			}
		}

		if (image.data == NULL || image.cols < 30 || image.rows < 30) // 尺寸太小不比较
			continue;
		// 判断和违规图片比较大于阈值
		theApp.m_plan->m_optionsLock.Lock();
		for (const auto& i : theApp.m_plan->m_images)
		{
			if (i.img.cols < 30 || i.img.rows < 30) // 尺寸太小不比较
				continue;
			double mssim = GetMSSIM(image, i.img);
			if (mssim > theApp.m_plan->m_SSIMThreshold)
			{
				msg.Format(_T("<font color=red> 触发违规图片 </font>%s<font color=red> 相似度%.3lf</font>"),
					(LPCTSTR)i.name, mssim);
				m_illegalImage.insert(imgName);
				theApp.m_plan->m_optionsLock.Unlock();
				return TRUE;
			}
		}
		m_leagalImage.insert(imgName);
		theApp.m_plan->m_optionsLock.Unlock();
	}

	return FALSE;
}

// SSIM算法比较图片
double CScanImage::GetMSSIM(const cv::Mat& i1, const cv::Mat& i2)
{
	static const double C1 = 6.5025, C2 = 58.5225;
	static const int d = CV_32F;
	try
	{
		cv::Mat I1, I2;
		i1.convertTo(I1, d);           // 不能在单字节像素上进行计算，范围不够。
		i2.convertTo(I2, d);

		if (I1.rows * I1.cols < I2.rows * I2.cols)
			cv::resize(I2, I2, I1.size());
		else
			cv::resize(I1, I1, I2.size());

		cv::Mat I2_2 = I2.mul(I2);        // I2^2
		cv::Mat I1_2 = I1.mul(I1);        // I1^2
		cv::Mat I1_I2 = I1.mul(I2);        // I1 * I2

		///////////////////////// 初步计算 ///////////////////////////////

		cv::Mat mu1, mu2;
		GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
		I1.release();
		GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);
		I2.release();

		cv::Mat mu1_2 = mu1.mul(mu1);
		cv::Mat mu2_2 = mu2.mul(mu2);
		cv::Mat mu1_mu2 = mu1.mul(mu2);
		mu1.release();
		mu2.release();

		cv::Mat sigma1_2, sigma2_2, sigma12;

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
		cv::Mat t1, t2, t3;

		t1 = 2 * mu1_mu2 + C1;
		t2 = 2 * sigma12 + C2;
		t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

		t1 = mu1_2 + mu2_2 + C1;
		t2 = sigma1_2 + sigma2_2 + C2;
		t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))
		t2.release();

		cv::Mat ssim_map;
		divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;
		t1.release();
		t3.release();

		cv::Scalar mssim = mean(ssim_map); // mssim = ssim_map的平均值
		return mssim.val[0] + mssim.val[1] + mssim.val[2];
	}
	catch (...) // GaussianBlur创建Mat时可能抛出异常-215？
	{
		return 0;
	}
}

void CScanImage::ClearCache()
{
	m_leagalImage.clear();
	m_illegalImage.clear();
}
