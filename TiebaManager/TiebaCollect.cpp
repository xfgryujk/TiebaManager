#include "stdafx.h"
#include "TiebaCollect.h"

#include "TiebaVariable.h"

#include "StringHelper.h"
#include "NetworkHelper.h"


// 采集贴吧用的常量
// 正则表达式太慢所以不用
#pragma region 主题列表
const TCHAR THREAD_SPLIT[] = _T("data-field='{&quot;id&quot;:");
const TCHAR THREAD_END[] = _T("<div id=\"frs_list_pager\"");
//const TCHAR THREAD_TID_LEFT[] = _T("&quot;id&quot;:");
const TCHAR THREAD_TID_RIGHT[] = _T(",");
const TCHAR THREAD_REPLY_LEFT[] = _T("&quot;reply_num&quot;:");
const TCHAR THREAD_REPLY_RIGHT[] = _T(",");
const TCHAR THREAD_TITLE_LEFT[] = _T(R"(class="j_th_tit ">)");
const TCHAR THREAD_TITLE_RIGHT[] = _T("</a>");
const TCHAR THREAD_PREVIEW_LEFT[] = _T(R"(threadlist_abs_onlyline ">)");
const TCHAR THREAD_PREVIEW_RIGHT[] = _T("</div>");
const TCHAR THREAD_MEDIA_LEFT[] = _T("<ul class=\"threadlist_media");
const TCHAR THREAD_MEDIA_RIGHT[] = _T("</ul>");
const TCHAR THREAD_AUTHOR_LEFT[] = _T("&quot;author_name&quot;:&quot;");
const TCHAR THREAD_AUTHOR_RIGHT[] = _T("&quot;");
const TCHAR THREAD_AUTHOR_ID_LEFT[] = _T("&quot;user_id&quot;:");
const TCHAR THREAD_AUTHOR_ID_RIGHT[] = _T("}");
const TCHAR THREAD_LAST_AUTHOR_LEFT[] = _T(R"(title="最后回复人: )");
const TCHAR THREAD_LAST_AUTHOR_RIGHT[] = _T("\"");
#pragma endregion
#pragma region 帖子列表
const TCHAR POST_SPLIT[] = _T("data-field='{&quot;author&quot;:");
const TCHAR POST_PID_LEFT[] = _T("&quot;post_id&quot;:");
const TCHAR POST_PID_RIGHT[] = _T(",");
const TCHAR POST_FLOOR_LEFT[] = _T("&quot;post_no&quot;:");
const TCHAR POST_FLOOR_RIGHT[] = _T(",");
const TCHAR POST_AUTHOR_LEFT[] = _T("&quot;user_name&quot;:&quot;");
const TCHAR POST_AUTHOR_RIGHT[] = _T("&quot;");
const TCHAR POST_AUTHOR_ID_LEFT[] = _T("&quot;user_id&quot;:");
const TCHAR POST_AUTHOR_ID_RIGHT[] = _T(",");
const TCHAR POST_AUTHOR_PORTRAIT_LEFT[] = _T("=\"http://tb.himg.baidu.com/sys/portrait/item/");
const TCHAR POST_AUTHOR_PORTRAIT_RIGHT[] = _T("\"");
const TCHAR POST_AUTHOR_LEVEL_LEFT[] = _T("<div class=\"d_badge_lv\">");
const TCHAR POST_AUTHOR_LEVEL_RIGHT[] = _T("</div>");
const TCHAR POST_CONTENT_LEFT[] = _T("<cc>");
const TCHAR POST_CONTENT_RIGHT[] = _T("</cc>");
const TCHAR POST_SIGN_LEFT[] = _T("<img class=\"j_user_sign\"");
const TCHAR POST_SIGN_RIGHT[] = _T("/>");
#pragma endregion
#pragma region 楼中楼列表
const wregex LZL_FLOOR_REG(_T("\"(\\d+)\":.*?\"comment_info\":\\[(.*?)during_time\":\\d+\\}\\]"));

const TCHAR LZL_USER_SPLIT[] = _T("\"user_name\":\"");
const TCHAR LZL_USER_NAME_RIGHT[] = _T("\"");
const TCHAR LZL_USER_PORTRAIT_LEFT[] = _T("\"portrait\":\"");
const TCHAR LZL_USER_PORTRAIT_RIGHT[] = _T("\"");

const TCHAR LZL_SPLIT[] = _T("{\"thread_id\"");
const TCHAR LZL_PID_LEFT[] = _T("\"comment_id\":\"");
const TCHAR LZL_PID_RIGHT[] = _T("\"");
const TCHAR LZL_AUTHOR_LEFT[] = _T("\"username\":\"");
const TCHAR LZL_AUTHOR_RIGHT[] = _T("\"");
const TCHAR LZL_AUTHOR_ID_LEFT[] = _T("\"user_id\":\"");
const TCHAR LZL_AUTHOR_ID_RIGHT[] = _T("\"");
const TCHAR LZL_CONTENT_LEFT[] = _T("\"content\":\"");
const TCHAR LZL_CONTENT_RIGHT[] = _T("\",\"");
const TCHAR LZL_PTYPE_LEFT[] = _T("\"ptype\":");
const TCHAR LZL_PTYPE_RIGHT[] = _T(",");
const TCHAR LZL_DURING_TIME_LEFT[] = _T("\"during_time\":");
const TCHAR LZL_DURING_TIME_RIGHT[] = _T("}");
#pragma endregion


// 取主题列表
BOOL GetThreads(LPCTSTR forumName, LPCTSTR ignoreThread, vector<ThreadInfo>& threads)
{
	CString src = HTTPGet(_T("http://tieba.baidu.com/f?ie=UTF-8&kw=") + EncodeURI(forumName)
		+ _T("&pn=") + ignoreThread, FALSE, &g_stopScanFlag);

	CStringArray rawThreads;
	SplitString(rawThreads, src, THREAD_SPLIT);
	if (rawThreads.GetSize() < 2)
	{
		if (src != NET_STOP_TEXT && src != NET_TIMEOUT_TEXT)
			WriteString(src, _T("forum.txt"));
		return FALSE;
	}

	rawThreads[rawThreads.GetSize() - 1] = GetStringBefore(rawThreads[rawThreads.GetSize() - 1], THREAD_END);
	threads.resize(rawThreads.GetSize() - 1);
	for (int iRawThreads = 1, iThreads = 0; iRawThreads < rawThreads.GetSize(); iRawThreads++, iThreads++)
	{
		//threads[iThreads].tid = GetStringBetween(rawThreads[iRawThreads], THREAD_TID_LEFT, THREAD_TID_RIGHT);
		threads[iThreads].tid = GetStringBefore(rawThreads[iRawThreads], THREAD_TID_RIGHT);
		threads[iThreads].reply = GetStringBetween(rawThreads[iRawThreads], THREAD_REPLY_LEFT, THREAD_REPLY_RIGHT);
		threads[iThreads].title = HTMLUnescape(GetStringBetween(rawThreads[iRawThreads], THREAD_TITLE_LEFT, THREAD_TITLE_RIGHT));
		threads[iThreads].preview = HTMLUnescape(GetStringBetween(rawThreads[iRawThreads], THREAD_PREVIEW_LEFT, THREAD_PREVIEW_RIGHT).Trim())
			+ _T("\r\n") + GetStringBetween2(rawThreads[iRawThreads], THREAD_MEDIA_LEFT, THREAD_MEDIA_RIGHT);
		threads[iThreads].authorID = GetStringBetween(rawThreads[iRawThreads], THREAD_AUTHOR_ID_LEFT, THREAD_AUTHOR_ID_RIGHT);
		//threads[iThreads].author = JSUnescape(GetStringBefore(rawThreads[iRawThreads], THREAD_AUTHOR_RIGHT));
		threads[iThreads].author = JSUnescape(GetStringBetween(rawThreads[iRawThreads], THREAD_AUTHOR_LEFT, THREAD_AUTHOR_RIGHT));
		threads[iThreads].lastAuthor = GetStringBetween(rawThreads[iRawThreads], THREAD_LAST_AUTHOR_LEFT, THREAD_LAST_AUTHOR_RIGHT);

		//OutputDebugString(_T("\n"));
		//OutputDebugString(rawThreads[iRawThreads]);
		//OutputDebugString(_T("\n----------------------------------"));
	}

	return TRUE;
}

// 取帖子列表
GetPostsResult GetPosts(const CString& tid, const CString& _src, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls)
{
	CString src = _src != _T("") ? _src : HTTPGet(_T("http://tieba.baidu.com/p/") + tid + _T("?pn=") + page, FALSE, &g_stopScanFlag);
	if (src == NET_STOP_TEXT)
		return GET_POSTS_STOP;
	if (src == NET_TIMEOUT_TEXT)
		return GET_POSTS_TIMEOUT;

	CStringArray rawPosts;
	SplitString(rawPosts, src, POST_SPLIT);
	if (rawPosts.GetSize() < 2)
		return GET_POSTS_DELETED;

	posts.resize(rawPosts.GetSize() - 1);
	for (int iRawPosts = 1, iPosts = 0; iRawPosts < rawPosts.GetSize(); iRawPosts++, iPosts++)
	{
		posts[iPosts].pid = GetStringBetween(rawPosts[iRawPosts], POST_PID_LEFT, POST_PID_RIGHT);
		posts[iPosts].floor = GetStringBetween(rawPosts[iRawPosts], POST_FLOOR_LEFT, POST_FLOOR_RIGHT);
		posts[iPosts].author = JSUnescape(GetStringBetween(rawPosts[iRawPosts], POST_AUTHOR_LEFT, POST_AUTHOR_RIGHT));
		posts[iPosts].authorID = GetStringBetween(rawPosts[iRawPosts], POST_AUTHOR_ID_LEFT, POST_AUTHOR_ID_RIGHT);
		posts[iPosts].authorLevel = GetStringBetween(rawPosts[iRawPosts], POST_AUTHOR_LEVEL_LEFT, POST_AUTHOR_LEVEL_RIGHT);
		posts[iPosts].authorPortrait = GetStringBetween(rawPosts[iRawPosts], POST_AUTHOR_PORTRAIT_LEFT, POST_AUTHOR_PORTRAIT_RIGHT);

		int left = rawPosts[iRawPosts].Find(POST_CONTENT_LEFT) + _tcslen(POST_CONTENT_LEFT);
		left = rawPosts[iRawPosts].Find(_T(">"), left) + 1;
		// 去掉首空格
		while (left < rawPosts[iRawPosts].GetLength() && rawPosts[iRawPosts][left] == _T(' '))
			left++;
		int right = rawPosts[iRawPosts].Find(POST_CONTENT_RIGHT, left + 1);
		// CString不支持反向查找字符串？
		posts[iPosts].content = rawPosts[iRawPosts].Mid(left, right - left);
		LPCTSTR pos = StrRStrI(posts[iPosts].content, NULL, _T("</div>"));
		if (pos != NULL)
		{
			right = ((DWORD)pos - (DWORD)(LPCTSTR)posts[iPosts].content) / sizeof(TCHAR)-1;
			// 去掉尾空格
			while (right >= 0 && posts[iPosts].content[right] == _T(' '))
				right--;
			posts[iPosts].content = posts[iPosts].content.Left(right + 1);
		}
		// 签名档
		posts[iPosts].content += _T("\r\n") + GetStringBetween2(rawPosts[iRawPosts], POST_SIGN_LEFT, POST_SIGN_RIGHT);

		//OutputDebugString(_T("\n"));
		//OutputDebugString(rawPosts[iRawPosts]);
		//OutputDebugString(_T("\n----------------------------------"));
	}

	GetLzls(tid, page, posts, lzls);

	return GET_POSTS_SUCCESS;
}

// 取楼中楼列表
void GetLzls(const CString& tid, const CString& page, vector<PostInfo>& posts, vector<PostInfo>& lzls)
{
	time_t timestamp;
	time(&timestamp);
	CString url;
	url.Format(_T("http://tieba.baidu.com/p/totalComment?t=%I64d&tid=%s&fid=%s&pn=%s&see_lz=0"), 
		timestamp, (LPCTSTR)tid, (LPCTSTR)g_userTiebaInfo.m_forumID, (LPCTSTR)page);
	CString src = HTTPGet(url, FALSE, &g_stopScanFlag);
	//WriteString(src, _T("lzl.txt"));
	CStringArray splitedSrc; // 0楼中楼，1用户
	SplitString(splitedSrc, src, _T("\"user_list\":{"));
	lzls.clear();
	if (splitedSrc.GetSize() != 2)
		return;

	// 遍历用户采集头像哈希
	CStringArray users;
	SplitString(users, splitedSrc[1], LZL_USER_SPLIT);
	map<CString, CString> portrait;
	for (int i = 1; i < users.GetSize(); i++)
	{
		CString id = JSUnescape(GetStringBefore(users[i], LZL_USER_NAME_RIGHT));
		portrait[id] = GetStringBetween(users[i], LZL_USER_PORTRAIT_LEFT, LZL_USER_PORTRAIT_RIGHT);
	}

	// 遍历楼层
	int iLzls = 0;
	for (std::regex_iterator<LPCTSTR> it((LPCTSTR)splitedSrc[0], (LPCTSTR)splitedSrc[0] + splitedSrc[0].GetLength(), LZL_FLOOR_REG), end; it != end; ++it)
	{
		// 查找该层楼层
		CString pid = (*it)[1].str().c_str(); // 该层PID
		CString floor;
		for (PostInfo post : posts)
		if (post.pid == pid)
		{
			floor = post.floor;
			break;
		}

		// 遍历该层楼中楼
		CStringArray rawLzls;
		SplitString(rawLzls, (*it)[2].str().c_str(), LZL_SPLIT);
		lzls.resize(lzls.size() + rawLzls.GetSize() - 1);
		for (int iRawLzls = 1; iRawLzls < rawLzls.GetSize(); iRawLzls++, iLzls++)
		{
			lzls[iLzls].pid = GetStringBetween(rawLzls[iRawLzls], LZL_PID_LEFT, LZL_PID_RIGHT);
			lzls[iLzls].floor = floor;
			lzls[iLzls].author = JSUnescape(GetStringBetween(rawLzls[iRawLzls], LZL_AUTHOR_LEFT, LZL_AUTHOR_RIGHT));
			lzls[iLzls].authorID = GetStringBetween(rawLzls[iRawLzls], LZL_AUTHOR_ID_LEFT, LZL_AUTHOR_ID_RIGHT);
			lzls[iLzls].authorPortrait = portrait[lzls[iLzls].author];
			lzls[iLzls].content = HTMLUnescape(JSUnescape(GetStringBetween(rawLzls[iRawLzls], LZL_CONTENT_LEFT, LZL_CONTENT_RIGHT)));

			if (GetStringBetween(rawLzls[iRawLzls], LZL_PTYPE_LEFT, LZL_PTYPE_RIGHT) == _T("1")) // 语音贴
			{
				lzls[iLzls].content += _T(R"(<div class="voice_player voice_player_mini voice_player_lzl"><a class="voice_player_inner" href="#"><span class="before">&nbsp;</span><span class="middle"><span class="speaker speaker_animate">&nbsp;</span><span class="time" style="width: 65px;"><span class="second">)");
				lzls[iLzls].content += GetStringBetween(rawLzls[iRawLzls], LZL_DURING_TIME_LEFT, LZL_DURING_TIME_RIGHT);
				lzls[iLzls].content += _T(R"(</span>"</span></span><span class="after">&nbsp;</span></a></div>)");
			}
		}
	}
}

// 取用户发的帖子ID
CString GetPIDFromUser(const CString& userName)
{
	CString src = HTTPGet(_T("http://tieba.baidu.com/f/search/ures?ie=utf-8&kw=") + g_userTiebaInfo.m_encodedForumName + _T("&qw=&rn=10&un=")
		+ userName + _T("&only_thread=&sm=1&sd=&ed=&pn=1"), FALSE);
	if (src == NET_TIMEOUT_TEXT)
		return NET_TIMEOUT_TEXT;
	CString pid = GetStringBetween(src, _T("<div class=\"s_post\">"), _T("target=\"_blank\" >"));
	pid = GetStringBetween(pid, _T("?pid="), _T("&"));
	return pid;
}
