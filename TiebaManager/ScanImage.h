#pragma once
#include <zlib.h>
#include <vector>
using std::vector;
#include <set>
using std::set;
#include <opencv2\core\mat.hpp>
using cv::Mat;
#include "Tieba.h"
#include "Global.h"


extern set<CString> g_leagalImage; // 已检查不违规的图片
extern set<CString> g_illegalImage; // 已检查违规的图片


const CString IMG_CACHE_PATH = _T("ImageCache\\");


struct NameImage
{
	CString name;
	Mat img;
};


BOOL ReadImage(const CString& path, Mat& img);
BOOL ReadImage(const BYTE* buffer, ULONG size, CImage& img);
BOOL ReadImage(const BYTE* buffer, ULONG size, Mat& img);
void ReadImages(const CString& dir);
CString GetImageName(const CString& imgUrl);

void GetThreadImage(const CString& preview, vector<CString>& img);
void GetPostImage(const CString& content, vector<CString>& img);

BOOL CheckImageIllegal(const CString& content, const CString& author, void(*GetImage)(const CString& content,
	vector<CString>& img), CString& msg);
BOOL DoCheckImageIllegal(vector<CString>& imgs, CString& msg);
