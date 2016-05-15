﻿#include "stdafx.h"
#include <TBMOperate.h>
#include <TBMOperateEvent.h>

#include <TiebaClawer.h>
#include <TiebaOperate.h>
#include <TBMCoreConfig.h>

#include <MiscHelper.h>
#include <StringHelper.h>

#include <Mmsystem.h>


CTBMOperate::CTBMOperate(CString& cookie, const int& banDuration, const CString& banReason) : 
	m_tiebaOperate(new CTiebaOperate(cookie, banDuration, banReason)),
	m_confirmThread(&CTBMOperate::ConfirmThread, this),
	m_operateThread(&CTBMOperate::OperateThread, this)
{
	
}

CTBMOperate::~CTBMOperate()
{
	m_operationQueue.Stop();
	m_confirmQueue.Stop();
	if (m_operateThread.joinable())
		m_operateThread.join();
	if (m_confirmThread.joinable())
		m_confirmThread.join();
}


// 添加确认
void CTBMOperate::AddConfirm(BOOL forceToConfirm, const CString& msg, Operation::TBObject object, const CString& tid, const CString& title,
	const CString& floor, const CString& pid, const CString& author, const CString& authorID,
	const CString& authorPortrait, int pos, int length)
{
	Operation tmp;
	tmp.forceToConfirm = forceToConfirm;
	tmp.msg = msg;
	tmp.pos = pos;
	tmp.length = length;
	tmp.object = object;
	tmp.tid = tid;
	tmp.title = title;
	tmp.floor = floor;
	tmp.pid = pid;
	tmp.author = author;
	tmp.authorID = authorID;
	tmp.authorPortrait = authorPortrait;
	m_confirmQueue.push(std::move(tmp));
}

// 确认线程
void CTBMOperate::ConfirmThread()
{
	// 初始化
	if (!CoInitializeHelper())
		return;

	while (TRUE)
	{
		Operation op;
		PopResult res = m_confirmQueue.pop(op);
		if (res == POP_STOP)
			break;
		if (res == POP_UNEXPECTED)
			continue;

		// 没有操作
		if (!m_config->m_delete && !m_config->m_banID && !m_config->m_defriend)
			continue;

		// 确认是否操作
		if (m_config->m_confirm || op.forceToConfirm)
		{
			COperateEvent event_(op);
			event_.result = TRUE;
			if (!m_eventBus.Post(ComfirmEvent, event_) || !event_.result)
			{
				switch (op.object)
				{
				case Operation::TBOBJ_THREAD:
					if (op.floor == _T("1"))
						goto CasePost;
					m_userCache->m_initIgnoredTID->insert(_ttoi64(op.tid));
					m_log->Log(_T("<font color=green>忽略 </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a>"));
					break;
				case Operation::TBOBJ_POST:
				CasePost:
					m_userCache->m_initIgnoredPID->insert(_ttoi64(op.pid));
					m_log->Log(_T("<font color=green>忽略 </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("楼"));
					break;
				case Operation::TBOBJ_LZL:
					m_userCache->m_initIgnoredLZLID->insert(_ttoi64(op.pid));
					m_log->Log(_T("<font color=green>忽略 </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("楼回复"));
					break;
				}
				continue;
			}
		}

		// 加入操作
		AddOperation(std::move(op));
	}

	CoUninitialize();
	
	TRACE(_T("确认线程结束\n"));
}

// 添加操作
void CTBMOperate::AddOperation(Operation&& operation)
{
	m_operationQueue.push(operation);
}

// 操作线程
void CTBMOperate::OperateThread()
{
	// 初始化
	if (!CoInitializeHelper())
		return;

	while (TRUE)
	{
		Operation op;
		PopResult res = m_operationQueue.pop(op);
		if (res == POP_STOP)
			break;
		if (res == POP_UNEXPECTED)
			continue;

		// 没有操作
		if (!m_config->m_delete && !m_config->m_banID && !m_config->m_defriend)
			continue;

		if (!m_eventBus.Post(PreOperateEvent, COperateEvent(op)))
			continue;

		// 增加违规次数
		auto countIt = m_userCache->m_userTrigCount->find(op.author);
		BOOL hasHistory = countIt != m_userCache->m_userTrigCount->end();
		int count = hasHistory ? (countIt->second + 1) : 1;
		if (hasHistory)
			countIt->second = count;
		else
			(*m_userCache->m_userTrigCount)[op.author] = 1;

		// 封禁
		if (m_config->m_banID && count >= m_config->m_banTrigCount
			&& m_userCache->m_bannedUser->find(op.author) == m_userCache->m_bannedUser->end() // 达到封禁违规次数且未封
			&& m_eventBus.Post(PreBanEvent, COperateEvent(op)))
		{
			BOOL result = FALSE;
			// 不使用客户端接口必须获取PID
			if ((!m_config->m_wapBanInterface /*|| m_config->m_banDuration != 1*/) && op.pid == _T(""))
			{
				vector<PostInfo> posts;
				GetPosts(op.tid, _T(""), _T("1"), posts);
				if (posts.size() > 0)
					op.pid = posts[0].pid;
			}
			if ((!m_config->m_wapBanInterface /*|| m_config->m_banDuration != 1*/) && op.pid == _T(""))
			{
				m_log->Log(_T("<font color=red>封禁 </font>") + op.author + _T("<font color=red> 失败！(获取帖子ID失败)</font>"));
			}
			else
			{
				CString code = (m_config->m_wapBanInterface || op.pid == _T("")) ? 
					m_tiebaOperate->BanIDClient(op.author) : m_tiebaOperate->BanID(op.author, op.pid);
				if (code != _T("0"))
				{
					CString content;
					content.Format(_T("<font color=red>封禁 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
								   _T("\"bd:%s,%s\">重试</a>"), (LPCTSTR)op.author, (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)op.author, (LPCTSTR)op.pid);
					m_log->Log(content);
				}
				else
				{
					result = TRUE;
					sndPlaySound(_T("封号.wav"), SND_ASYNC | SND_NODEFAULT);
					m_userCache->m_bannedUser->insert(op.author);
					m_log->Log(_T("<font color=red>封禁 </font>") + op.author);
				}
			}

			COperateEvent event_(op);
			event_.result = result;
			m_eventBus.Post(PostBanEvent, event_);
		}

		// 拉黑
		if (m_config->m_defriend && count >= m_config->m_defriendTrigCount
			&& m_userCache->m_defriendedUser->find(op.author) == m_userCache->m_defriendedUser->end() // 达到拉黑违规次数且未拉黑
			&& m_eventBus.Post(PreDefriendEvent, COperateEvent(op)))
		{
			BOOL result = FALSE;
			CString code = m_tiebaOperate->Defriend(op.authorID);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<font color=red>拉黑 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
							   _T("\"df:%s\">重试</a>"), (LPCTSTR)op.author, (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)op.authorID);
				m_log->Log(content);
			}
			else
			{
				result = TRUE;
				sndPlaySound(_T("封号.wav"), SND_ASYNC | SND_NODEFAULT);
				m_userCache->m_defriendedUser->insert(op.author);
				m_log->Log(_T("<font color=red>拉黑 </font>") + op.author);
			}

			COperateEvent event_(op);
			event_.result = result;
			m_eventBus.Post(PostDefriendEvent, event_);
		}

		// 主题已被删则不再删帖
		__int64 tid = _ttoi64(op.tid);
		if (m_userCache->m_deletedTID.find(tid) != m_userCache->m_deletedTID.end())
			continue;

		// 删帖
		if (m_config->m_delete
			&& m_eventBus.Post(PreDeleteEvent, COperateEvent(op)))
		{
			BOOL result = FALSE;
			if (op.object == Operation::TBOBJ_THREAD) // 主题
			{
				CString code = m_tiebaOperate->DeleteThread(op.tid);
				if (code != _T("0"))
				{
					CString content;
					content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a><font color=red> 删除失败！错误代码：%s(%s)</font><a href=")
						_T("\"dt:%s\">重试</a>"), (LPCTSTR)op.tid, (LPCTSTR)HTMLEscape(op.title), (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)op.tid);
					m_log->Log(content);
				}
				else
				{
					result = TRUE;
					sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
					m_userCache->m_deletedTID.insert(_ttoi64(op.tid));
					m_log->Log(_T("<font color=red>删除 </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a>"));
					Sleep((DWORD)(m_config->m_deleteInterval * 1000));
				}
			}
			else if (op.object == Operation::TBOBJ_POST) // 帖子
			{
				CString code = m_tiebaOperate->DeletePost(op.tid, op.pid);
				if (code != _T("0"))
				{
					CString content;
					content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a> %s楼<font color=red> 删除失败！错误代码：%s(%s)</font>")
						_T("<a href=\"dp:%s,%s\">重试</a>"), (LPCTSTR)op.tid, (LPCTSTR)HTMLEscape(op.title), (LPCTSTR)op.floor, (LPCTSTR)code,
						(LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)op.tid, (LPCTSTR)op.pid);
					m_log->Log(content);
				}
				else
				{
					result = TRUE;
					sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
					m_log->Log(_T("<font color=red>删除 </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("楼"));
					Sleep((DWORD)(m_config->m_deleteInterval * 1000));
				}
			}
			else /*if (op.object == Operation::TBOBJ_POST)*/ // 楼中楼
			{
				CString code = m_tiebaOperate->DeleteLZL(op.tid, op.pid);
				if (code != _T("0"))
				{
					CString content;
					content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a> %s楼回复<font color=red> 删除失败！错误代码：")
						_T("%s(%s)</font><a href=\"dl:%s,%s\">重试</a>"), (LPCTSTR)op.tid, (LPCTSTR)HTMLEscape(op.title),
						(LPCTSTR)op.floor, (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)op.tid, (LPCTSTR)op.pid);
					m_log->Log(content);
				}
				else
				{
					result = TRUE;
					sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
					m_log->Log(_T("<font color=red>删除 </font><a href=\"http://tieba.baidu.com/p/") + op.tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + op.floor + _T("楼回复"));
					Sleep((DWORD)(m_config->m_deleteInterval * 1000));
				}
			}

			COperateEvent event_(op);
			event_.result = result;
			m_eventBus.Post(PostDeleteEvent, event_);
		}
	}

	CoUninitialize();

	TRACE(_T("操作线程结束\n"));
}