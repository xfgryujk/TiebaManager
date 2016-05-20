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


class TIEBA_API_API CTiebaOperate
{
protected:
	CString m_forumID;			// 贴吧ID，确认贴吧时初始化
	CString m_forumName;		// 贴吧名，确认贴吧时初始化
	CString m_encodedForumName; // UTF8 URL编码的贴吧名，确认贴吧时初始化

	CString m_userName;			// 用户名，确认贴吧时初始化
	CString& m_cookie;
	CString m_bduss;			// Cookie中的BDUSS，确认贴吧时初始化，用于客户端接口
	CString m_tbs;				// 口令号，估计是防止CSRF用的，确认贴吧时初始化，备用获取tbs地址：http://tieba.baidu.com/dc/common/tbs

	const int& m_banDuration;	// 封禁时长
	const CString& m_banReason; // 封禁原因

	CString m_randomTid;		// 某个tid，确认贴吧时初始化，用于WAP接口和客户端接口封禁

public:
	CString GetForumID() { return m_forumID; }
	CString GetForumName() { return m_forumName; }
	CString GetEncodedForumName() { return m_encodedForumName; }
	CString GetUserName_() { return m_userName; }
	CString GetTBS() { return m_tbs; }
	BOOL HasSetTieba() { return m_bduss != _T(""); }

	CTiebaOperate(CString& cookie, const int& banDuration, const CString& banReason);

	enum SetTiebaResult { SET_TIEBA_OK, SET_TIEBA_TIMEOUT, SET_TIEBA_NOT_FOUND, SET_TIEBA_NOT_LOGIN, SET_TIEBA_NO_POWER, SET_TIEBA_NO_TBS };
	// 设置要操作的贴吧
	SetTiebaResult SetTieba(const CString& forumName);

	// 带Cookie
	CString HTTPGet(const CString& URL);
	// 带Cookie
	CString HTTPPost(const CString& URL, const CString& data);

	// 封ID，返回错误代码
	CString BanID(const CString& userName, const CString& pid);
	// 封ID，返回错误代码，不用PID（用户必须为本吧会员）
	CString BanID(const CString& userName);
	// 封ID，返回错误代码，WAP接口，不用PID，只能封1天（否则要用TID）
	CString BanIDWap(const CString& userName);
	// 封ID，返回错误代码，客户端接口，不用PID，小吧可封10天
	CString BanIDClient(const CString& userName);
	// 拉黑，返回错误代码
	CString Defriend(const CString& userID);
	// 删主题，返回错误代码
	CString DeleteThread(const CString& tid);
	// 删帖子，返回错误代码
	CString DeletePost(const CString& tid, const CString& pid);
	// 删楼中楼，返回错误代码
	CString DeleteLZL(const CString& tid, const CString& lzlid);
};

// 取错误文本
TIEBA_API_API CString GetTiebaErrorText(const CString& errorCode);
