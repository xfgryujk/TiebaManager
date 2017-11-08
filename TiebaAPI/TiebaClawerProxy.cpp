/*
Copyright (C) 2011-2017  xfgryujk
https://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

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
#include <TiebaClawerProxy.h>
#include <TiebaClawerWeb.h>
#include <TiebaClawerClient.h>


BOOL TiebaClawerProxy::GetThreads(const CString& forumName, const CString& ignoreThread, std::vector<ThreadInfo>& threads)
{
	return GetImplement().GetThreads(forumName, ignoreThread, threads);
}

TiebaClawer::GetPostsResult TiebaClawerProxy::GetPosts(const CString& fid, const CString& tid, const CString& page,
	std::vector<PostInfo>& posts, std::vector<LzlInfo>& lzls, AdditionalThreadInfo* addition)
{
	return GetImplement().GetPosts(fid, tid, page, posts, lzls, addition);
}

TiebaClawer::GetPostsResult TiebaClawerProxy::GetPosts(const CString& fid, const CString& tid, const CString& page, const CString& src,
	std::vector<PostInfo>& posts, std::vector<LzlInfo>& lzls, AdditionalThreadInfo* addition)
{
	return GetImplement().GetPosts(fid, tid, page, src, posts, lzls, addition);
}


TiebaClawer& TiebaClawerProxy::GetImplement()
{
	switch (m_interface)
	{
	case TIEBA_INTERFACE_WEB:       return TiebaClawerWeb::GetInstance();
	case TIEBA_INTERFACE_CLIENT:    return TiebaClawerClient::GetInstance();
	}
	throw std::invalid_argument("No interface");
}
