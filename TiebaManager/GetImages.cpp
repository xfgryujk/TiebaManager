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
#include <GetImages.h>

#include <TiebaClawer.h>

#include "TiebaManager.h"
#include <TBMEvent.h>


// 1是图片地址
static const wregex THREAD_IMG_REG(_T("<img .*?bpic=\"(.*?)\".*?/>"));
// 2是图片地址
static const wregex POST_IMG_REG(_T("<img .*?class=\"(BDE_Image|j_user_sign)\".*?src=\"(.*?)\".*?>"));


CGetThreadImages::CGetThreadImages(const ThreadInfo& thread) : 
	m_preview(thread.preview)
{

}

CGetThreadImages::CGetThreadImages(const CString& preview) : 
	m_preview(preview)
{

}

// 从主题预览取图片地址
void CGetThreadImages::operator () (vector<CString>& img)
{
	if (!theApp.m_tbmEventBus->Post(GetThreadImagesEvent, CGetThreadImagesEvent(*this, img)))
		return;
	for (std::regex_iterator<LPCTSTR> it((LPCTSTR)m_preview, (LPCTSTR)m_preview + m_preview.GetLength(), THREAD_IMG_REG),
		end; it != end; ++it)
		img.push_back((*it)[1].str().c_str());
}


CGetPostImages::CGetPostImages(const PostInfo& post) : 
	m_content(post.content), 
	m_portrait(post.authorPortrait)
{

}

CGetPostImages::CGetPostImages(const CString& content, const CString& portrait) : 
	m_content(content), 
	m_portrait(portrait)
{

}

// 从帖子取图片地址
void CGetPostImages::operator () (vector<CString>& img)
{
	if (!theApp.m_tbmEventBus->Post(GetPostImagesEvent, CGetPostImagesEvent(*this, img)))
		return;
	for (std::regex_iterator<LPCTSTR> it((LPCTSTR)m_content, (LPCTSTR)m_content + m_content.GetLength(), POST_IMG_REG),
		end; it != end; ++it)
		img.push_back((*it)[2].str().c_str());
	if (m_portrait != _T(""))
		img.push_back(_T("http://tb.himg.baidu.com/sys/portrait/item/") + m_portrait);
}
