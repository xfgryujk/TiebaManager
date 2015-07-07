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

void GetThreadImage(const CString& preview, vector<CString>& img);
void GetPostImage(const CString& content, vector<CString>& img);

BOOL CheckImageIllegal(const CString& content, const CString& author, void(*GetImage)(const CString& content,
	vector<CString>& img), CString& msg);
BOOL DoCheckImageIllegal(vector<CString>& imgs, CString& msg);
