#pragma once
#include <zlib.h>
#include <vector>
using std::vector;
#include <opencv2\core\mat.hpp>
using cv::Mat;
#include "Tieba.h"
#include "Global.h"


const CString IMG_CACHE_PATH = _T("ImageCache\\");


struct NameImage
{
	CString name;
	Mat img;
};


void ReadImages(const CString& dir);

void GetImage(const ThreadInfo& thread, vector<CString>& img);
void GetImage(const PostInfo& post, vector<CString>& img);

BOOL DoCheckImageIllegal(vector<CString>& imgs, CString& msg);
template<typename InfoType>
BOOL CheckImageIllegal(InfoType content, CString& msg)
{
	if (g_images.empty())
		return FALSE;

	g_optionsLock.Lock();
	// 信任用户
	for (const CString& whiteList : g_whiteList)
		if (content.author == whiteList)
		{
			g_optionsLock.Unlock();
			return FALSE;
		}
	g_optionsLock.Unlock();

	vector<CString> imgs;
	GetImage(content, imgs);
	return DoCheckImageIllegal(imgs, msg);
}
