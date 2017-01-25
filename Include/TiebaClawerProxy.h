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
#include "TiebaAPICommon.h"
#include "TiebaClawer.h"
#include "Singleton.h"


// 根据状态选择哪个接口采集贴吧
class TIEBA_API_API TiebaClawerProxy final : public TiebaClawer, public Singleton<TiebaClawerProxy>
{
	DECL_SINGLETON_DEFAULT(TiebaClawerProxy);
public:
	virtual BOOL GetThreads(const CString& forumName, const CString& ignoreThread, std::vector<ThreadInfo>& threads) override;
	virtual GetPostsResult GetPosts(const CString& fid, const CString& tid, const CString& page, std::vector<PostInfo>& posts,
		std::vector<LzlInfo>& lzls, AdditionalThreadInfo* addition = NULL) override;
	virtual GetPostsResult GetPosts(const CString& fid, const CString& tid, const CString& page, const CString& src,
		std::vector<PostInfo>& posts, std::vector<LzlInfo>& lzls, AdditionalThreadInfo* addition = NULL) override;

	TiebaClawer& GetImplement();


	TiebaInterface m_interface = TIEBA_INTERFACE_WEB;
};
