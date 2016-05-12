#pragma once
#include "TiebaAPICommon.h"


class TIEBA_API_API CTiebaOperate
{
protected:
	CString m_forumID;
	CString m_forumName;
	CString m_encodedForumName;
	CString& m_cookie;
	const CString& m_bduss;
	const CString& m_tbs;
	const int& m_banDuration;
	const CString& m_banReason;

	CString m_randomTid; // 某个tid，确认贴吧时初始化，WAP接口用

public:
	CTiebaOperate(const CString& forumID, const CString& forumName, CString& cookie, const CString& bduss, 
		const CString& tbs, const int& banDuration, const CString& banReason, const CString& randomTid);

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
