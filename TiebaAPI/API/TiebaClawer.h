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

#pragma once
#include "TiebaAPICommon.h"


// 采集贴吧用的常量
#pragma region 用户信息
// 3是fid，7是贴吧名
const wregex FORUM_ID_NAME_REG(_T("id('|\")?:\\s*('|\")?(\\d+)('|\")?,")
							   _T(".*?name('|\")?:\\s*('|\")(.*?)('|\")"));
// 3是用户名
const wregex USER_NAME_REG(_T("name('|\")?:\\s*('|\")(.*?)('|\")"));
const TCHAR _TBS_LEFT[] = _T("PageData.tbs = \"");
const TCHAR _TBS_RIGHT[] = _T("\"");
// 3是tbs
const wregex TBS_REG(_T("tbs('|\")?:\\s*('|\")(.*?)('|\")"));
#pragma endregion


// 主题信息
struct ThreadInfo
{
	CString tid;		// 主题ID
	CString reply;		// 回复数
	CString title;		// 标题
	CString preview;	// 预览
	CString author;		// 作者
	CString authorID;	// 作者ID
	CString lastAuthor; // 最后回复
};

// 帖子信息
struct PostInfo
{
	CString pid;			// 帖子ID
	CString floor;			// 楼层
	CString author;			// 作者
	CString authorID;		// 作者ID
	CString authorLevel;	// 作者等级
	CString authorPortrait;	// 作者头像哈希
	CString content;		// 内容
};


TIEBA_API_API BOOL GetThreads(LPCTSTR forumName, LPCTSTR ignoreThread, vector<ThreadInfo>& threads);
enum GetPostsResult { GET_POSTS_SUCCESS, GET_POSTS_TIMEOUT, GET_POSTS_DELETED };
TIEBA_API_API GetPostsResult GetPosts(const CString& tid, const CString& _src, const CString& page, vector<PostInfo>& posts);
TIEBA_API_API void GetLzls(const CString& fid, const CString& tid, const CString& page, const vector<PostInfo>& posts, vector<PostInfo>& lzls);
