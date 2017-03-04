/*
Copyright (C) 2011-2017  xfgryujk
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

#pragma once
#include "TiebaManagerCoreCommon.h"

#include "TiebaClawer.h"

#include "Singleton.h"
#pragma warning(disable:4819) // OpenCV头文件包含Unicode字符
#include <opencv2\core\mat.hpp>
using std::min; // 用于GDI+的头文件
using std::max;
#include <atlimage.h>
#include <memory>
#include <thread>


TBM_CORE_API void GetImageUrls(const TBObject& object, std::vector<CString>& urls);


class TBM_CORE_API CImageCache final : public Singleton<CImageCache>
{
	DECL_SINGLETON(CImageCache);
private:
	CImageCache();
	~CImageCache();

public:
	static CString CACHE_PATH;


	BOOL GetImage(CString imgUrl, cv::Mat& mat);
	BOOL GetImage(CString imgUrl, CImage& img);

private:
	volatile BOOL m_stopFlag = FALSE;
	std::unique_ptr<std::thread> m_cleanThread;

	void CleanThread();
};
