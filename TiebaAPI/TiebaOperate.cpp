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
#include <TiebaOperate.h>
#include <TiebaClawer.h>
#include <StringHelper.h>
#include <NetworkHelper.h>
#include <TiebaClientHelper.h>


CTiebaOperate::CTiebaOperate(CString& cookie, const int& banDuration, const CString& banReason) :
	m_cookie(cookie), 
	m_banDuration(banDuration), 
	m_banReason(banReason)
{
}

// 设置要操作的贴吧
CTiebaOperate::SetTiebaResult CTiebaOperate::SetTieba(const CString& forumName)
{
	if (forumName == _T(""))
		return SET_TIEBA_NOT_FOUND;

	CString src = this->HTTPGet(_T("http://tieba.baidu.com/f?ie=utf-8&kw=") + EncodeURI(forumName));
	if (src == NET_TIMEOUT_TEXT)
		return SET_TIEBA_TIMEOUT;

	// 采集贴吧信息
	CString tmp = GetStringBetween(src, _T("PageData.forum"), _T("}"));
	tmp.Replace(_T("\r\n"), _T(""));
	std::wcmatch res;
	if (!std::regex_search((LPCTSTR)tmp, res, FORUM_ID_NAME_REG))
	{
		WriteString(src, _T("forum.txt"));
		return SET_TIEBA_NOT_FOUND;
	}

	// 取贴吧ID
	m_forumID = res[3].str().c_str();

	// 取贴吧名
	m_forumName = JSUnescape(res[7].str().c_str());
	m_encodedForumName = EncodeURI(m_forumName);

	// 取用户名
	if (std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData.user"), _T("}"))), res, USER_NAME_REG)
		|| std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData"), _T("}"))), res, USER_NAME_REG))
		m_userName = JSUnescape(res[3].str().c_str());
	if (m_userName == _T(""))
	{
		WriteString(src, _T("forum.txt"));
		return SET_TIEBA_NOT_LOGIN;
	}

	// 验证用户权限
	// 旧接口
	//CString src2 = ::HTTPGet(_T("http://tieba.baidu.com/f/bawu/admin_group?kw=") + EncodeURI_GBK(m_forumName));
	CString src2 = ::HTTPGet(_T("http://tieba.baidu.com/bawu2/platform/listBawuTeamInfo?word=") + m_encodedForumName + _T("&ie=utf-8"));
	if (src2 == NET_TIMEOUT_TEXT)
		return SET_TIEBA_TIMEOUT;
	CStringArray bawuList;
	SplitString(bawuList, src2, _T("class=\"bawu_single_type"));
	BOOL hasPower = FALSE;
	if (bawuList.GetSize() > 1)
	{
		bawuList[bawuList.GetSize() - 1] = GetStringBefore(bawuList[bawuList.GetSize() - 1], _T("</div></div>"));
		for (int i = 1; i < bawuList.GetSize(); i++)
		if ((bawuList[i].Find(_T("吧主<span")) != -1 // WTF，怎么有这么多种吧主
			|| bawuList[i].Find(_T(">语音小编<span")) != -1)
			&& bawuList[i].Find(_T(">") + m_userName + _T("<")) != -1)
		{
			hasPower = TRUE;
			break;
		}
	}
	if (!hasPower)
		WriteString(src2, _T("admin.txt"));

	// 取tbs(口令号)
	m_tbs = GetStringBetween(src, _TBS_LEFT, _TBS_RIGHT);
	if (m_tbs == _T("") && std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData"), _T("}"))), res, TBS_REG))
		m_tbs = JSUnescape(res[3].str().c_str());
	if (m_tbs == _T(""))
	{
		WriteString(src, _T("forum.txt"));
		return SET_TIEBA_NO_TBS;
	}

	// 取第一个tid
	m_randomTid = GetStringBetween(src, _T("&quot;id&quot;:"), _T(","));
	if (m_randomTid == _T(""))
		m_randomTid = _T("4426261107");

	// 确定BDUSS
	m_bduss = GetStringBetween(m_cookie, _T("BDUSS="), _T(";"));
	
	return hasPower ? SET_TIEBA_OK : SET_TIEBA_NO_POWER;
}


// 带Cookie
CString CTiebaOperate::HTTPGet(const CString& URL)
{
	return ::HTTPGet(URL, &m_cookie);
}

// 带Cookie
CString CTiebaOperate::HTTPPost(const CString& URL, const CString& data)
{
	return ::HTTPPost(URL, data, &m_cookie);
}


// 取错误代码
static inline CString GetOperationErrorCode(const CString& src)
{
	if (src == NET_TIMEOUT_TEXT /*|| src == NET_STOP_TEXT*/)
		return _T("-65536");
	CString code = GetStringBetween(src, _T("no\":"), _T(","));
	if (code == _T(""))
		code = GetStringBetween(src, _T("code\":\""), _T("\""));
	if (code != _T("0"))
		WriteString(src, _T("operation.txt"));
	return code;
}

// 封ID，返回错误代码
CString CTiebaOperate::BanID(const CString& userName, const CString& pid)
{
	CString data;
	data.Format(_T("day=%d&fid=%s&tbs=%s&ie=gbk&user_name%%5B%%5D=%s&pid%%5B%%5D=%s&reason=%s"),
		m_banDuration, (LPCTSTR)m_forumID, (LPCTSTR)m_tbs, (LPCTSTR)EncodeURI(userName), (LPCTSTR)pid,
		m_banReason != _T("") ? (LPCTSTR)m_banReason : _T("%20"));
	CString src = this->HTTPPost(_T("http://tieba.baidu.com/pmc/blockid"), data);
	return GetOperationErrorCode(src);
}

// 封ID，返回错误代码，不用PID（用户必须为本吧会员）
CString CTiebaOperate::BanID(const CString& userName)
{
	CString data;
	data.Format(_T("day=%d&fid=%s&tbs=%s&ie=gbk&user_name%%5B%%5D=%s&reason=%s"),
		m_banDuration, (LPCTSTR)m_forumID, (LPCTSTR)m_tbs, (LPCTSTR)EncodeURI(userName),
		m_banReason != _T("") ? (LPCTSTR)m_banReason : _T("%20"));
	CString src = this->HTTPPost(_T("http://tieba.baidu.com/pmc/blockid"), data);
	return GetOperationErrorCode(src);
}

// 封ID，返回错误代码，WAP接口，不用PID，只能封1天（否则要用TID）
CString CTiebaOperate::BanIDWap(const CString& userName)
{
	CString url;
	url.Format(_T("http://tieba.baidu.com/mo/q/m?tn=bdFIL&ntn=banid&day=1&un=%s&tbs=%s")
			   _T("&word=%s&fid=%s&z=%s&$el=%%5Bobject%%20Array%%5D&reason=%s"),
		(LPCTSTR)EncodeURI(userName), (LPCTSTR)m_tbs, (LPCTSTR)m_encodedForumName,
		(LPCTSTR)m_forumID, (LPCTSTR)m_randomTid, m_banReason != _T("") ? (LPCTSTR)m_banReason : _T("%20"));
	CString src = this->HTTPGet(url);
	return GetOperationErrorCode(src);
}

// 封ID，返回错误代码，客户端接口，不用PID，小吧可封10天
CString CTiebaOperate::BanIDClient(const CString& userName)
{
	// 客户端POST要带数字签名，参数按字典序排列，去掉&，加上"tiebaclient!!!"，转成UTF-8，取MD5
	CString data;
	data.Format(_T("BDUSS=%s&day=%d&fid=%s&ntn=banid&tbs=%s&un=%s&word=%s&z=1111111111"),
		(LPCTSTR)m_bduss, m_banDuration, (LPCTSTR)m_forumID,
		(LPCTSTR)m_tbs, userName, (LPCTSTR)m_forumName);
	CString src = TiebaClientHTTPPost(_T("http://c.tieba.baidu.com/c/c/bawu/commitprison"), data, &m_cookie);
	return GetOperationErrorCode(src);
}

// 拉黑，返回错误代码
CString CTiebaOperate::Defriend(const CString& userID)
{
	CString src = this->HTTPPost(_T("http://tieba.baidu.com/bawu2/platform/addBlack"), _T("ie=utf-8&tbs=") + m_tbs
		+ _T("&user_id=") + userID + _T("&word=") + m_encodedForumName);
	return GetOperationErrorCode(src);
}

// 删主题，返回错误代码
CString CTiebaOperate::DeleteThread(const CString& tid)
{
	CString src = this->HTTPPost(_T("http://tieba.baidu.com/f/commit/thread/delete"), _T("kw=") + m_encodedForumName
		+ _T("&fid=") + m_forumID + _T("&tid=") + tid + _T("&ie=utf-8&tbs=") + m_tbs);
	return GetOperationErrorCode(src);
}

// 删帖子，返回错误代码
CString CTiebaOperate::DeletePost(const CString& tid, const CString& pid)
{
	CString data;
	data.Format(_T("commit_fr=pb&ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&is_vipdel=0&pid=%s&is_finf=false"),
		(LPCTSTR)m_tbs, (LPCTSTR)m_encodedForumName, (LPCTSTR)m_forumID, tid, pid);
	CString src = this->HTTPPost(_T("http://tieba.baidu.com/f/commit/post/delete"), data);
	return GetOperationErrorCode(src);
}

// 删楼中楼，返回错误代码
CString CTiebaOperate::DeleteLZL(const CString& tid, const CString& lzlid)
{
	CString data;
	data.Format(_T("ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&pid=%s&is_finf=1"),
		(LPCTSTR)m_tbs, (LPCTSTR)m_encodedForumName, (LPCTSTR)m_forumID, tid, lzlid);
	CString src = this->HTTPPost(_T("http://tieba.baidu.com/f/commit/post/delete"), data);
	return GetOperationErrorCode(src);
}

// 取错误文本
TIEBA_API_API CString GetTiebaErrorText(const CString& errorCode)
{
	if (errorCode == _T("-65536"))
		return _T("超时");
	if (errorCode == _T("-1"))
		return _T("权限不足");
	if (errorCode == _T("4"))
		return _T("参数校验失败");
	if (errorCode == _T("11"))
		return _T("度娘抽了");
	if (errorCode == _T("14") || errorCode == _T("12"))
		return _T("已被系统封禁");
	if (errorCode == _T("74"))
		return _T("用户不存在(可能帖子已被删且用户已退出本吧会员且用户已隐藏动态)");
	if (errorCode == _T("77"))
		return _T("操作失败");
	if (errorCode == _T("78"))
		return _T("参数错误");
	if (errorCode == _T("308"))
		return _T("你被封禁或失去权限");
	if (errorCode == _T("871"))
		return _T("高楼不能删");
	if (errorCode == _T("872"))
		return _T("精品贴不能删");
	if (errorCode == _T("890"))
		return _T("贴子已删");
	if (errorCode == _T("4011"))
		return _T("需要验证码(操作太快？)");
	return _T("未知错误");
}
