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

#include "stdafx.h"
#include "GetImages.h"

#include <StringHelper.h>
#include <TiebaClawer.h>

#include "TiebaManager.h"
#include <TBMEvents.h>


// 1是图片地址
static const std::wregex THREAD_IMG_REG(_T("<img .*?bpic=\"(.*?)\".*?/>"));
// 2是图片地址
static const std::wregex POST_IMG_REG(_T("<img .*?class=\"(BDE_Image|j_user_sign)\".*?src=\"(.*?)\".*?>"));


CGetImages::CGetImages(const TBObject& object) :
	m_object(object)
{
}

void CGetImages::operator () (std::vector<CString>& img)
{
	if (m_object.m_type == TBObject::THREAD)
	{
		// 从主题预览取图片地址
		ThreadInfo& thread = (ThreadInfo&)m_object;
		BOOL useDefault = TRUE;
		g_getThreadImagesEvent(thread, img, useDefault);
		if (!useDefault)
			return;

		for (std::regex_iterator<LPCTSTR> it((LPCTSTR)thread.preview, (LPCTSTR)thread.preview + thread.preview.GetLength(), THREAD_IMG_REG),
			end; it != end; ++it)
			img.push_back((*it)[1].str().c_str());
	}
	else
	{
		// 从帖子或楼中楼获取图片
		if (m_object.m_type == TBObject::POST)
		{
			PostInfo& post = (PostInfo&)m_object;
			BOOL useDefault = TRUE;
			g_getPostImagesEvent(post, img, useDefault);
			if (!useDefault)
				return;
		}
		else if (m_object.m_type == TBObject::LZL)
		{
			LzlInfo& lzl = (LzlInfo&)m_object;
			BOOL useDefault = TRUE;
			g_getLzlImagesEvent(lzl, img, useDefault);
			if (!useDefault)
				return;
		}

		CString content = m_object.GetContent();
		for (std::regex_iterator<LPCTSTR> it((LPCTSTR)content, (LPCTSTR)content + content.GetLength(), POST_IMG_REG),
			end; it != end; ++it)
			img.push_back((*it)[2].str().c_str());
	}
	// 头像
	if (m_object.authorPortraitUrl != _T(""))
		img.push_back(m_object.authorPortraitUrl);
}
