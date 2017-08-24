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
#include <TiebaClawerWeb.h>

#include <StringHelper.h>
#include <NetworkHelper.h>

#include <document.h>
#include <stringbuffer.h>
#include <writer.h>
using namespace rapidjson;


// 采集贴吧用的常量
// 正则表达式太慢所以不用
#pragma region 主题列表
const TCHAR THREAD_SPLIT[]                      = _T(R"(<li class=" j_thread_list)");
const TCHAR THREAD_END[]                        = _T(R"(<div class="thread_list_bottom)");
const TCHAR THREAD_DATA_FIELD_LEFT[]            = _T("data-field='");
const TCHAR THREAD_DATA_FIELD_RIGHT[]           = _T("'");
const TCHAR THREAD_AUTHOR_SHOW_NAME_LEFT[]      = _T(R"( title="主题作者: )");
const TCHAR THREAD_AUTHOR_SHOW_NAME_RIGHT[]     = _T("\"");
const TCHAR THREAD_AUTHOR_ID_LEFT[]             = _T("&quot;user_id&quot;:");
const TCHAR THREAD_AUTHOR_ID_RIGHT[]            = _T("}");
const TCHAR THREAD_TITLE_LEFT[]                 = _T(R"(class="j_th_tit ">)");
const TCHAR THREAD_TITLE_RIGHT[]                = _T("</a>");
const TCHAR THREAD_PREVIEW_LEFT[]               = _T(R"(threadlist_abs_onlyline ">)");
const TCHAR THREAD_PREVIEW_RIGHT[]              = _T("</div>");
const TCHAR THREAD_MEDIA_LEFT[]                 = _T(R"(<ul class="threadlist_media)");
const TCHAR THREAD_MEDIA_RIGHT[]                = _T("</ul>");
const TCHAR THREAD_LAST_AUTHOR_LEFT[]           = _T(R"(title="最后回复人: )");
const TCHAR THREAD_LAST_AUTHOR_RIGHT[]          = _T("\"");
#pragma endregion
#pragma region 帖子列表
const TCHAR POST_SPLIT[]                        = _T(R"(<div class="l_post)");
const TCHAR POST_DATA_FIELD_LEFT[]              = _T("data-field='");
const TCHAR POST_DATA_FIELD_RIGHT[]             = _T("'");
const TCHAR POST_AUTHOR_SHOW_NAME_LEFT1[]       = _T(R"(class="p_author_name)");
const TCHAR POST_AUTHOR_SHOW_NAME_RIGHT1[]      = _T("</li>");
const TCHAR POST_AUTHOR_SHOW_NAME_LEFT2[]       = _T(R"( target="_blank">)");
const TCHAR POST_AUTHOR_SHOW_NAME_RIGHT2[]      = _T("</a>");
const TCHAR POST_AUTHOR_PORTRAIT_LEFT1[]        = _T(R"(data-tb-lazyload=")");
const TCHAR POST_AUTHOR_PORTRAIT_RIGHT1[]       = _T("\"");
const TCHAR POST_AUTHOR_PORTRAIT_LEFT2[]        = _T(R"(src=")");
const TCHAR POST_AUTHOR_PORTRAIT_RIGHT2[]       = _T("\"");
const TCHAR POST_AUTHOR_LEVEL_LEFT[]            = _T(R"(<div class="d_badge_lv">)");
const TCHAR POST_AUTHOR_LEVEL_RIGHT[]           = _T("</div>");
const TCHAR POST_TIME_LEFT[]                    = _T(R"(楼</span><span class="tail-info">)");
const TCHAR POST_TIME_RIGHT[]                   = _T("</span>");
const TCHAR POST_CONTENT_LEFT[]                 = _T("<cc>");
const TCHAR POST_CONTENT_RIGHT[]                = _T("</cc>");
const TCHAR POST_SIGN_LEFT[]                    = _T(R"(<img class="j_user_sign")");
const TCHAR POST_SIGN_RIGHT[]                   = _T("/>");
#pragma endregion
#pragma region 附加主题信息
const TCHAR ADDITION_THREAD_FORUM_DATA_LEFT[]   = _T("PageData.forum");
const TCHAR ADDITION_THREAD_FORUM_DATA_RIGHT[]  = _T("}");
const TCHAR ADDITION_THREAD_PAGE_COUNT_LEFT[]   = _T(R"(,"total_page":)");
const TCHAR ADDITION_THREAD_PAGE_COUNT_RIGHT[]  = _T("}");
#pragma endregion


BOOL TiebaClawerWeb::GetThreads(const CString& forumName, const CString& ignoreThread, std::vector<ThreadInfo>& threads)
{
	CString src = HTTPGet(_T("http://tieba.baidu.com/f?ie=UTF-8&kw=") + EncodeURI(forumName)
		+ _T("&pn=") + ignoreThread);

	CStringArray rawThreads;
	SplitString(rawThreads, src, THREAD_SPLIT);
	if (rawThreads.GetSize() < 2)
	{
		if (src != NET_TIMEOUT_TEXT)
			WriteString(src, _T("forum.txt"));
		return FALSE;
	}

	rawThreads[rawThreads.GetSize() - 1] = GetStringBefore(rawThreads[rawThreads.GetSize() - 1], THREAD_END);
	threads.resize(rawThreads.GetSize() - 1);
	for (int iRawThreads = 1, iThreads = 0; iRawThreads < rawThreads.GetSize(); ++iRawThreads, ++iThreads)
	{
		GenericDocument<UTF16<> > dataField;
		dataField.Parse(HTMLUnescape(GetStringBetween(rawThreads[iRawThreads], THREAD_DATA_FIELD_LEFT, THREAD_DATA_FIELD_RIGHT)));
		if (dataField.HasParseError() || !dataField.IsObject())
			continue;
		
		const auto& rawThread = rawThreads[iRawThreads];
		auto& thread = threads[iThreads];
		thread.rawData = rawThread;
		thread.tid.Format(_T("%I64u"), dataField[L"id"].GetUint64());
		if (dataField[L"author_name"].IsString()) // 远古时期的IP发帖作者为null
			thread.author = dataField[L"author_name"].GetString();
		else
			thread.author = _T("");
		thread.authorShowName = GetStringBetween(rawThread, THREAD_AUTHOR_SHOW_NAME_LEFT, THREAD_AUTHOR_SHOW_NAME_RIGHT);
		thread.authorID = GetStringBetween(rawThread, THREAD_AUTHOR_ID_LEFT, THREAD_AUTHOR_ID_RIGHT);
		thread.authorPortraitUrl = _T("");
		thread.timestamp = 0;
		thread.reply.Format(_T("%u"), dataField[L"reply_num"].GetUint());
		thread.title = HTMLUnescape(GetStringBetween(rawThread, THREAD_TITLE_LEFT, THREAD_TITLE_RIGHT));
		thread.preview = HTMLUnescape(GetStringBetween(rawThread, THREAD_PREVIEW_LEFT, THREAD_PREVIEW_RIGHT).Trim())
			+ _T("\r\n") + GetStringBetween2(rawThread, THREAD_MEDIA_LEFT, THREAD_MEDIA_RIGHT);
		thread.lastAuthor = GetStringBetween(rawThread, THREAD_LAST_AUTHOR_LEFT, THREAD_LAST_AUTHOR_RIGHT);

		//OutputDebugString(_T("\n"));
		//OutputDebugString(rawThread);
		//OutputDebugString(_T("\n----------------------------------"));
	}

	return TRUE;
}

TiebaClawer::GetPostsResult TiebaClawerWeb::GetPosts(const CString& fid, const CString& tid, const CString& page,
	std::vector<PostInfo>& posts, std::vector<LzlInfo>& lzls, AdditionalThreadInfo* addition)
{
	CString src = HTTPGet(_T("http://tieba.baidu.com/p/") + tid + _T("?pn=") + page + _T("&fid=") + fid);
	if (src == NET_TIMEOUT_TEXT)
		return GET_POSTS_TIMEOUT;
	return GetPosts(fid, tid, page, src, posts, lzls, addition);
}

TiebaClawer::GetPostsResult TiebaClawerWeb::GetPosts(const CString& fid, const CString& tid, const CString& page, const CString& src,
	std::vector<PostInfo>& posts, std::vector<LzlInfo>& lzls, AdditionalThreadInfo* addition)
{
	// 取帖子列表
	GetPostsResult res = GetPosts(tid, page, src, posts, addition);
	if (res != GET_POSTS_SUCCESS)
		return res;
	// 取楼中楼列表
	GetLzls(fid, tid, page, posts, lzls);
	return res;
}


TiebaClawer::GetPostsResult TiebaClawerWeb::GetPosts(const CString& tid, const CString& page, 
	std::vector<PostInfo>& posts, AdditionalThreadInfo* addition)
{
	CString src = HTTPGet(_T("http://tieba.baidu.com/p/") + tid + _T("?pn=") + page);
	if (src == NET_TIMEOUT_TEXT)
		return GET_POSTS_TIMEOUT;
	return GetPosts(tid, page, src, posts, addition);
}

TiebaClawer::GetPostsResult TiebaClawerWeb::GetPosts(const CString& tid, const CString& page, const CString& src, 
	std::vector<PostInfo>& posts, AdditionalThreadInfo* addition)
{
	//WriteString(src, _T("thread.txt"));
	CStringArray rawPosts;
	SplitString(rawPosts, src, POST_SPLIT);
	if (rawPosts.GetSize() < 2)
		return GET_POSTS_DELETED;

	size_t size = rawPosts.GetSize() - 1;
	posts.resize(size);
	for (int iRawPosts = 1, iPosts = 0; iRawPosts < rawPosts.GetSize(); ++iRawPosts, ++iPosts)
	{
		GenericDocument<UTF16<> > dataField;
		dataField.Parse(HTMLUnescape(GetStringBetween(rawPosts[iRawPosts], POST_DATA_FIELD_LEFT, POST_DATA_FIELD_RIGHT)));
		if (dataField.HasParseError() || !dataField.IsObject()) // 去掉广告
		{
			--size;
			--iPosts;
			continue;
		}

		const auto& rawPost = rawPosts[iRawPosts];
		auto& post = posts[iPosts];
		post.rawData = rawPost;
		post.tid = tid;
		post.author = dataField[L"author"][L"user_name"].GetString();
		post.authorShowName = GetStringBetween(rawPost, POST_AUTHOR_SHOW_NAME_LEFT1, POST_AUTHOR_SHOW_NAME_RIGHT1);
		post.authorShowName = GetStringBetween(post.authorShowName, POST_AUTHOR_SHOW_NAME_LEFT2, POST_AUTHOR_SHOW_NAME_RIGHT2);
		if (dataField[L"author"].HasMember(L"user_id"))
			post.authorID.Format(_T("%I64u"), dataField[L"author"][L"user_id"].GetUint64());
		else
			post.authorID = _T("null");
		post.authorPortraitUrl = GetStringBetween(rawPost, POST_AUTHOR_PORTRAIT_LEFT1, POST_AUTHOR_PORTRAIT_RIGHT1);
		if (post.authorPortraitUrl == _T(""))
			post.authorPortraitUrl = GetStringBetween(rawPost, POST_AUTHOR_PORTRAIT_LEFT2, POST_AUTHOR_PORTRAIT_RIGHT2);

		tm time; // 本地时间（假设为北京时间），应该没有人把系统时区设置为其他的吧...
		if (_stscanf_s(GetStringBetween(rawPost, POST_TIME_LEFT, POST_TIME_RIGHT), _T("%d-%d-%d %d:%d"),
			&time.tm_year, &time.tm_mon, &time.tm_mday, &time.tm_hour, &time.tm_min) != 5)
			post.timestamp = 0;
		else
		{
			time.tm_sec = 0;
			time.tm_year -= 1900;
			time.tm_mon -= 1;
			post.timestamp = mktime(&time);
		}

		post.pid.Format(_T("%I64u"), dataField[L"content"][L"post_id"].GetUint64());
		post.floor.Format(_T("%u"), dataField[L"content"][L"post_no"].GetUint());
		post.authorLevel = GetStringBetween(rawPost, POST_AUTHOR_LEVEL_LEFT, POST_AUTHOR_LEVEL_RIGHT);

		int left = rawPost.Find(POST_CONTENT_LEFT) + _countof(POST_CONTENT_LEFT) - 1;
		left = rawPost.Find(_T(">"), left) + 1;
		// 去掉首空格
		while (left < rawPost.GetLength() && rawPost[left] == _T(' '))
			left++;
		int right = rawPost.Find(POST_CONTENT_RIGHT, left + 1);
		post.content = rawPost.Mid(left, right - left);
		LPCTSTR pos = StrRStrI(post.content, NULL, _T("</div>"));
		if (pos != NULL)
		{
			right = ((DWORD)pos - (DWORD)(LPCTSTR)post.content) / sizeof(TCHAR)-1;
			// 去掉尾空格
			while (right >= 0 && post.content[right] == _T(' '))
				right--;
			post.content = post.content.Left(right + 1);
		}
		// 签名档
		post.content += _T("\r\n") + GetStringBetween2(rawPost, POST_SIGN_LEFT, POST_SIGN_RIGHT);

		//OutputDebugString(_T("\n"));
		//OutputDebugString(rawPost);
		//OutputDebugString(_T("\n----------------------------------"));
	}
	posts.resize(size); // 去掉广告

	// 附加信息
	if (addition != NULL)
	{
		addition->src = src;

		CString tmp = GetStringBetween(src, ADDITION_THREAD_FORUM_DATA_LEFT, ADDITION_THREAD_FORUM_DATA_RIGHT);
		tmp.Replace(_T("\r\n"), _T(""));
		std::wcmatch res;
		if (std::regex_search((LPCTSTR)tmp, res, FORUM_ID_NAME_REG))
			addition->fid = res[3].str().c_str();

		addition->pageCount = GetStringBetween(src, ADDITION_THREAD_PAGE_COUNT_LEFT, ADDITION_THREAD_PAGE_COUNT_RIGHT);
	}

	return GET_POSTS_SUCCESS;
}

void TiebaClawerWeb::GetLzls(const CString& fid, const CString& tid, const CString& page, const std::vector<PostInfo>& posts, std::vector<LzlInfo>& lzls)
{
	time_t timestamp;
	time(&timestamp);
	CString url;
	url.Format(_T("http://tieba.baidu.com/p/totalComment?t=%I64d&tid=%s&fid=%s&pn=%s&see_lz=0"), 
		timestamp, (LPCTSTR)tid, (LPCTSTR)fid, (LPCTSTR)page);
	CString src = HTTPGet(url);
	WriteString(src, _T("lzl.txt"));

	lzls.clear();
	GenericDocument<UTF16<> > document;
	document.Parse(src);
	if (document.HasParseError() || !document.IsObject())
		return;

	int iLzls = 0;
	const auto& commentList = document[L"data"][L"comment_list"];
	const auto& userList = document[L"data"][L"user_list"];
	if (!commentList.IsObject())
		return;

	// 遍历楼层
	for (auto it = commentList.MemberBegin(); it != commentList.MemberEnd(); ++it)
	{
		// 查找该层楼层
		CString pid = it->name.GetString(); // 该层PID
		CString floor;
		for (const PostInfo& post : posts)
		{
			if (post.pid == pid)
			{
				floor = post.floor;
				break;
			}
		}

		// 遍历该层楼中楼
		const auto& comments = it->value[L"comment_info"].GetArray();
		lzls.resize(lzls.size() + comments.Size());
		for (const auto& comment : comments)
		{
			auto& lzl = lzls[iLzls];
			GenericStringBuffer<UTF16<> > buffer;
			Writer<decltype(buffer), UTF16<>, UTF16<> > writer(buffer);
			comment.Accept(writer);
			lzl.rawData = buffer.GetString();

			lzl.tid = tid;
			lzl.author = comment[L"username"].GetString();
			lzl.authorID = comment[L"user_id"].GetString();
			const auto& user = userList[(LPCWSTR)lzl.authorID];
			lzl.authorShowName = user[L"nickname"].GetString();
			lzl.authorPortraitUrl = CString(_T("http://tb.himg.baidu.com/sys/portrait/item/")) + user[L"portrait"].GetString();
			lzl.timestamp = comment[L"now_time"].GetInt64();
			lzl.cid = comment[L"comment_id"].GetString();
			lzl.floor = floor;
			lzl.content = comment[L"content"].GetString();
			// 语音贴
			if (comment.HasMember(L"ptype") && comment[L"ptype"].GetInt() == 1)
			{
				CString tmp;
				tmp.Format(_T("\r\n") _T(R"(<div class="voice_player voice_player_mini voice_player_lzl"><a class="voice_player_inner" href="#"><span class="before">&nbsp;</span><span class="middle"><span class="speaker speaker_animate">&nbsp;</span><span class="time" style="width: 65px;"><span class="second">%u</span>"</span></span><span class="after">&nbsp;</span></a></div>)"), 
					comment[L"during_time"].GetUint());
				lzls[iLzls].content += tmp;
			}

			++iLzls;
		}
	}
}
