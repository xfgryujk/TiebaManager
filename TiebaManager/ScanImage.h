#pragma once
#include <TiebaClawer.h>


extern set<CString> g_leagalImage; // 已检查不违规的图片
extern set<CString> g_illegalImage; // 已检查违规的图片


const CString IMG_CACHE_PATH = _T("ImageCache\\");


void ReadImages(const CString& dir);

class GetImagesBase
{
public:
	virtual ~GetImagesBase() {}
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
