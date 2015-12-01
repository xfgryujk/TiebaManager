#pragma once
#include <zlib.h>
#include <vector>
using std::vector;
#include <set>
using std::set;
#include <opencv2\core\mat.hpp>
using cv::Mat;
#include "TiebaCollect.h"


extern set<CString> g_leagalImage; // 已检查不违规的图片
extern set<CString> g_illegalImage; // 已检查违规的图片


const CString IMG_CACHE_PATH = _T("ImageCache\\");


BOOL ReadImage(const CString& path, Mat& img);
BOOL ReadImage(const BYTE* buffer, ULONG size, CImage& img);
BOOL ReadImage(const BYTE* buffer, ULONG size, Mat& img);
void ReadImages(const CString& dir);
CString GetImageName(const CString& imgUrl);

class GetImagesBase
{
public:
	virtual void GetImage(vector<CString>& img) = 0;
};

class GetThreadImage : public GetImagesBase
{
private:
	const CString& m_preview;
public:
	GetThreadImage(const ThreadInfo& thread) : m_preview(thread.preview) {}
	GetThreadImage(const CString& preview) : m_preview(preview) {}
	void GetImage(vector<CString>& img);
};

class GetPostImage : public GetImagesBase
{
private:
	const CString& m_content, m_portrait;
public:
	GetPostImage(const PostInfo& post) : m_content(post.content), m_portrait(post.authorPortrait) {}
	GetPostImage(const CString& content, const CString& portrait) : m_content(content), m_portrait(portrait) {}
	void GetImage(vector<CString>& img);
};

BOOL CheckImageIllegal(const CString& author, GetImagesBase& getImage, CString& msg);
BOOL DoCheckImageIllegal(vector<CString>& imgs, CString& msg);
