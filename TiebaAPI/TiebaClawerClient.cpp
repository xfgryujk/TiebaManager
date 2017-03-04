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
#include <TiebaClawerClient.h>

#include <StringHelper.h>
#include <NetworkHelper.h>
#include <TiebaClientHelper.h>

#include <document.h>
#include <stringbuffer.h>
#include <writer.h>
using namespace rapidjson;

#include <map>


BOOL TiebaClawerClient::GetThreads(const CString& forumName, const CString& ignoreThread, std::vector<ThreadInfo>& threads)
{
	CString data;
	data.Format(_T("_client_type=2&_client_version=5.7.0&from=tieba&kw=%s&net_type=1&pn=%d&q_type=2&rn=50&st_type=tb_forumlist&with_group=0"), 
		(LPCTSTR)forumName, _ttoi(ignoreThread) / 50 + 1);
	CString src = TiebaClientHTTPPost(_T("http://c.tieba.baidu.com/c/f/frs/page"), data);
	//WriteString(src, _T("forum.txt"));
	
	threads.clear();
	GenericDocument<UTF16<> > document;
	document.Parse(src);
	if (document.HasParseError() || !document.IsObject())
		return FALSE;

	const auto& threadList = document[L"thread_list"];
	threads.resize(threadList.Size());
	int iThread = 0;
	for (const auto& rawThread : threadList.GetArray())
	{
		if (!rawThread.HasMember(L"tid")) // 去掉广告
			continue;

		auto& thread = threads[iThread];
		GenericStringBuffer<UTF16<> > buffer;
		Writer<decltype(buffer), UTF16<>, UTF16<> > writer(buffer);
		rawThread.Accept(writer);
		thread.rawData = buffer.GetString();

		thread.tid = rawThread[L"tid"].GetString();
		thread.author = rawThread[L"author"][L"name"].GetString();
		thread.authorID = rawThread[L"author"][L"id"].GetString();
		thread.authorPortraitUrl = CString(_T("http://tb.himg.baidu.com/sys/portrait/item/")) + rawThread[L"author"][L"portrait"].GetString();
		if (rawThread.HasMember(L"create_time")) // 直播贴没有create_time
			thread.timestamp = _ttoi64(rawThread[L"create_time"].GetString());
		else
			thread.timestamp = 0;
		thread.reply = rawThread[L"reply_num"].GetString();
		thread.title = rawThread[L"title"].GetString();

		if (!rawThread[L"abstract"].Empty() && rawThread[L"abstract"][0].HasMember(L"text"))
			thread.preview = rawThread[L"abstract"][0][L"text"].GetString();
		else
			thread.preview = _T("");
		thread.preview += _T("\r\n");
		// 多媒体
		if (rawThread.HasMember(L"media")) // 直播贴没有media
		{
			for (const auto& media : rawThread[L"media"].GetArray())
			{
				CString tmp;
				switch (_ttoi(media[L"type"].GetString()))
				{
				case 3: // 图片
					tmp.Format(_T(R"(<li><a class="thumbnail vpic_wrap"><img src="" attr="1234" data-original="%s"  bpic="%s" class="threadlist_pic j_m_pic "  /></a><div class="threadlist_pic_highlight j_m_pic_light"></div></li>)"),
						media[L"big_pic"].GetString(), media[L"src_pic"].GetString());
					break;
				case 5: // 视频
					tmp.Format(_T(R"(<li><div class="threadlist_video"><img src=""/><a href="#" data-threadid="1234" data-forumid="1234" data-isfive="0" data-video="%s" data-type="" data-duration="" class="threadlist_btn_play j_m_flash"></a>)"),
						media[L"src"].GetString());
					break;
				}
				thread.preview += tmp;
			}
		}
		
		if (rawThread.HasMember(L"last_replyer")) // 直播贴没有last_replyer
			thread.lastAuthor = rawThread[L"last_replyer"][L"name"].GetString();
		else
			thread.lastAuthor = _T("");

		++iThread;
	}
	threads.resize(iThread); // 去掉广告

	return TRUE;
}

TiebaClawer::GetPostsResult TiebaClawerClient::GetPosts(const CString& fid, const CString& tid, const CString& page,
	std::vector<PostInfo>& posts, std::vector<LzlInfo>& lzls, AdditionalThreadInfo* addition)
{
	CString data;
	data.Format(_T("_client_type=2&_client_version=7.0.0&back=0&floor_rn=3&from=tieba&kz=%s&mark=0&net_type=1&pn=%s&rn=30&st_type=tb_bookmarklist&with_floor=1"), 
		(LPCTSTR)tid, (LPCTSTR)page);
	CString src = TiebaClientHTTPPost(_T("http://c.tieba.baidu.com/c/f/pb/page"), data);
	if (src == NET_TIMEOUT_TEXT)
		return GET_POSTS_TIMEOUT;
	//WriteString(src, _T("thread.txt"));
	return GetPosts(fid, tid, page, src, posts, lzls, addition);
}

// 取楼中楼
static void GetLzls(const CString& tid, const GenericDocument<UTF16<> >& document, std::map<CString, int>& userIndex, std::vector<LzlInfo>& lzls)
{
	const auto& userList = document[L"user_list"];

	// lzls.clear();
	int iLzls = 0;
	// 遍历楼层
	for (const auto& post : document[L"post_list"].GetArray())
	{
		if (!post[L"sub_post_list"].IsObject() || !post[L"sub_post_list"].HasMember(L"sub_post_list"))
			continue;

		CString pid = post[L"id"].GetString();
		// 遍历该层楼中楼
		const auto& subPostList = post[L"sub_post_list"][L"sub_post_list"];
		lzls.resize(lzls.size() + subPostList.Size());
		for (const auto& subPost : subPostList.GetArray())
		{
			auto& lzl = lzls[iLzls];
			GenericStringBuffer<UTF16<> > buffer;
			Writer<decltype(buffer), UTF16<>, UTF16<> > writer(buffer);
			subPost.Accept(writer);
			lzl.rawData = buffer.GetString();

			lzl.tid = tid;
			lzl.authorID = subPost[L"author_id"].GetString();
			const auto& user = userList[userIndex[lzl.authorID]];
			lzl.author = user[L"name"].GetString();
			lzl.authorPortraitUrl = CString(_T("http://tb.himg.baidu.com/sys/portrait/item/")) + user[L"portrait"].GetString();
			lzl.timestamp = _ttoi64(subPost[L"time"].GetString());
			lzl.cid = subPost[L"id"].GetString();
			lzl.floor = post[L"floor"].GetString();

			lzl.content = _T("");
			for (const auto& content : subPost[L"content"].GetArray())
			{
				CString tmp;
				switch (_ttoi(content[L"type"].GetString()))
				{
				case 0: // 文字
					tmp = content[L"text"].GetString();
					break;
				case 1: // 超链接
					tmp.Format(_T(R"(<a href="%s"  target="_blank">%s</a>)"), content[L"link"].GetString(), content[L"text"].GetString());
					break;
				case 2: // 表情
					tmp.Format(_T(R"(<img class="BDE_Smiley" width="30" height="30" changedsize="false" src="http://static.tieba.baidu.com/tb/editor/images/client/%s.png" >)"),
						content[L"text"].GetString());
					break;
				case 4: // @
					tmp.Format(_T(R"#(<a href=""  onclick="Stats.sendRequest('fr=tb0_forum&st_mod=pb&st_value=atlink');" onmouseover="showattip(this)" onmouseout="hideattip(this)" username="%s" target="_blank" class="at">%s</a>)#"),
						content[L"text"].GetString() + 1, content[L"text"].GetString());
					break;
				case 10: // 语音
					tmp.Format(_T("\r\n") _T(R"(<div class="voice_player voice_player_mini voice_player_lzl"><a class="voice_player_inner" href="#"><span class="before">&nbsp;</span><span class="middle"><span class="speaker speaker_animate">&nbsp;</span><span class="time" style="width: 65px;"><span class="second">%d</span>"</span></span><span class="after">&nbsp;</span></a></div>)"),
						_ttoi(content[L"during_time"].GetString()) / 1000);
					break;
				}
				lzl.content += tmp;
			}

			++iLzls;
		}
	}
}

TiebaClawer::GetPostsResult TiebaClawerClient::GetPosts(const CString& fid, const CString& tid, const CString& page, const CString& src,
	std::vector<PostInfo>& posts, std::vector<LzlInfo>& lzls, AdditionalThreadInfo* addition)
{
	posts.clear();
	lzls.clear();
	GenericDocument<UTF16<> > document;
	document.Parse(src);
	if (document.HasParseError() || !document.IsObject() || wcscmp(document[L"error_code"].GetString(), L"0") != 0)
		return GET_POSTS_DELETED;

	const auto& userList = document[L"user_list"];
	// 映射用户ID和索引
	std::map<CString, int> userIndex;
	int size = userList.Size();
	for (int i = 0; i < size; ++i)
		userIndex[userList[i][L"id"].GetString()] = i;

	const auto& postList = document[L"post_list"];
	size = postList.Size();
	posts.resize(size);
	// 遍历楼层
	for (int i = 0; i < size; ++i)
	{
		const auto& rawPost = postList[i];
		auto& post = posts[i];
		GenericStringBuffer<UTF16<> > buffer;
		Writer<decltype(buffer), UTF16<>, UTF16<> > writer(buffer);
		rawPost.Accept(writer);
		post.rawData = buffer.GetString();

		post.tid = tid;
		post.authorID = rawPost[L"author_id"].GetString();
		const auto& user = userList[userIndex[post.authorID]];
		post.author = user[L"name"].GetString();
		post.pid = rawPost[L"id"].GetString();
		post.floor = rawPost[L"floor"].GetString();
		post.authorLevel = user[L"level_id"].GetString();
		post.authorPortraitUrl = CString(_T("http://tb.himg.baidu.com/sys/portrait/item/")) + user[L"portrait"].GetString();
		post.timestamp = _ttoi64(rawPost[L"time"].GetString());

		post.content = _T("");
		for (const auto& content : rawPost[L"content"].GetArray())
		{
			CString tmp;
			switch (_ttoi(content[L"type"].GetString()))
			{
			case 0: // 文字
				tmp = content[L"text"].GetString();
				break;
			case 1: // 超链接
				tmp.Format(_T(R"(<a href="%s"  target="_blank">%s</a>)"), content[L"link"].GetString(), content[L"text"].GetString());
				break;
			case 2: // 表情
				tmp.Format(_T(R"(<img class="BDE_Smiley" width="30" height="30" changedsize="false" src="http://static.tieba.baidu.com/tb/editor/images/client/%s.png" >)"), 
					content[L"text"].GetString());
				break;
			case 3: // 图片
			{
				CStringArray size;
				SplitString(size, content[L"bsize"].GetString(), _T(","));
				tmp.Format(_T(R"(<img class="BDE_Image" pic_type="0" width="%s" height="%s" src="%s" >)"), (LPCTSTR)size[0], 
					(LPCTSTR)size[1], content[L"origin_src"].GetString());
				break;
			}
			case 4: // @
				tmp.Format(_T(R"#(<a href=""  onclick="Stats.sendRequest('fr=tb0_forum&st_mod=pb&st_value=atlink');" onmouseover="showattip(this)" onmouseout="hideattip(this)" username="%s" target="_blank" class="at">%s</a>)#"),
					content[L"text"].GetString() + 1, content[L"text"].GetString());
				break;
			case 5: // 视频
				tmp.Format(_T(R"(<embed class="BDE_Flash" type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer" wmode="transparent" play="true" loop="false" menu="false" src="%s" width="500" height="450" allowscriptaccess="never" allowfullscreen="true" scale="noborder">)"),
					content[L"text"].GetString());
				break;
			case 10: // 语音
				tmp.Format(_T(R"(<div class="voice_player voice_player_pb"><a href="#" class="voice_player_inner"><span class="before">&nbsp;</span><span class="middle"><span class="speaker speaker_animate">&nbsp;</span><span class="time"><span class="second">%d</span>&quot;</span></span><span class="after">&nbsp;</span></a></div><img class="j_voice_ad_gif" src="http://tb2.bdstatic.com/tb/static-pb/img/voice_ad.gif" alt="下载贴吧客户端发语音！" /><br/>)"),
					_ttoi(content[L"during_time"].GetString()) / 1000);
				break;
			}
			post.content += tmp;
		}
		// 小尾巴
		if (rawPost[L"signature"].IsObject()
			&& rawPost[L"signature"][L"content"].Size() > 0
			&& rawPost[L"signature"][L"content"][0].HasMember(L"text"))
		{
			post.content += _T("\r\n");
			post.content += rawPost[L"signature"][L"content"][0][L"text"].GetString();
		}
	}

	// 取楼中楼
	GetLzls(tid, document, userIndex, lzls);

	// 附加信息
	if (addition != NULL)
	{
		addition->src = src;
		addition->fid = document[L"forum"][L"id"].GetString();
		addition->pageCount = document[L"page"][L"total_page"].GetString();
	}

	return GET_POSTS_SUCCESS;
}
