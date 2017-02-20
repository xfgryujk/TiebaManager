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
#include <TBMOperate.h>
#include <TBMCoreEvents.h>

#include <TiebaClawerProxy.h>
#include <TiebaOperate.h>
#include <TBMCoreGlobal.h>

#include <MiscHelper.h>
#include <StringHelper.h>

#include <Mmsystem.h>


CTBMOperate::CTBMOperate()
{
	// 线程要在任务队列初始化后启动
	m_operateThread = std::make_unique<std::thread>(&CTBMOperate::OperateThread, this);
	m_confirmThread = std::make_unique<std::thread>(&CTBMOperate::ConfirmThread, this);
}

CTBMOperate::~CTBMOperate()
{
	m_operationQueue.Stop();
	m_confirmQueue.Stop();
	if (m_operateThread != nullptr && m_operateThread->joinable())
		m_operateThread->join();
	if (m_confirmThread != nullptr && m_confirmThread->joinable())
		m_confirmThread->join();
}


// 添加确认
void CTBMOperate::AddConfirm(Operation&& op)
{
	m_confirmQueue.push(std::move(op));
}	

// 确认线程
void CTBMOperate::ConfirmThread()
{
	// 初始化
	if (!CoInitializeHelper())
		return;

	Operation op;
	while (true)
	{
		PopResult res = m_confirmQueue.pop(op);
		if (res == POP_STOP)
			break;
		if (res == POP_UNEXPECTED)
			continue;

		// 没有操作
		if (!g_pTbmCoreConfig->m_delete && !g_pTbmCoreConfig->m_banID && !g_pTbmCoreConfig->m_defriend)
			continue;

		// 确认是否操作
		if (g_pTbmCoreConfig->m_confirm || op.forceToConfirm)
		{
			BOOL res = TRUE;
			g_comfirmEvent(op, res);
			if (!res)
			{
				switch (op.object->m_type)
				{
				case TBObject::THREAD:
				{
					ThreadInfo* thread = (ThreadInfo*)op.object.get();
					g_pUserCache->m_initIgnoredTID->insert(_ttoi64(thread->tid));
					g_pLog->Log(_T("<font color=green>忽略 </font><a href=\"http://tieba.baidu.com/p/") + thread->tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a>"));
					break;
				}
				case TBObject::POST:
				{
					PostInfo* post = (PostInfo*)op.object.get();
					g_pUserCache->m_initIgnoredPID->insert(_ttoi64(post->pid));
					g_pLog->Log(_T("<font color=green>忽略 </font><a href=\"http://tieba.baidu.com/p/") + post->tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + post->floor + _T("楼"));
					break;
				}
				case TBObject::LZL:
				{
					LzlInfo* lzl = (LzlInfo*)op.object.get();
					g_pUserCache->m_initIgnoredLZLID->insert(_ttoi64(lzl->cid));
					g_pLog->Log(_T("<font color=green>忽略 </font><a href=\"http://tieba.baidu.com/p/") + lzl->tid
						+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + lzl->floor + _T("楼回复"));
					break;
				}
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
	m_operationQueue.push(std::move(operation));
}

// 操作线程
void CTBMOperate::OperateThread()
{
	// 初始化
	if (!CoInitializeHelper())
		return;

	Operation op;
	while (true)
	{
		PopResult res = m_operationQueue.pop(op);
		if (res == POP_STOP)
			break;
		if (res == POP_UNEXPECTED)
			continue;

		// 没有操作
		if (!g_pTbmCoreConfig->m_delete && !g_pTbmCoreConfig->m_banID && !g_pTbmCoreConfig->m_defriend)
			continue;

		BOOL pass = TRUE;
		g_preOperateEvent(op, pass);
		if (!pass)
			continue;

		// 增加违规次数
		auto countIt = g_pUserCache->m_userTrigCount->find(op.object->author);
		BOOL hasHistory = countIt != g_pUserCache->m_userTrigCount->end();
		int count = hasHistory ? (countIt->second + 1) : 1;
		if (hasHistory)
			countIt->second = count;
		else
			(*g_pUserCache->m_userTrigCount)[op.object->author] = 1;

		// 封禁
		if (g_pTbmCoreConfig->m_banID && count >= g_pTbmCoreConfig->m_banTrigCount
			&& g_pUserCache->m_bannedUser->find(op.object->author) == g_pUserCache->m_bannedUser->end()) // 达到封禁违规次数且未封
		{
			pass = TRUE;
			g_preBanEvent(op, pass);
			if (pass)
			{
				BOOL result = FALSE;
				CString pid;
				// 不使用客户端接口必须获取PID
				if (!g_pTbmCoreConfig->m_banClientInterface)
				{
					switch (op.object->m_type)
					{
					case TBObject::THREAD:
					{
						std::vector<PostInfo> posts;
						std::vector<LzlInfo> lzls;
						TiebaClawerProxy::GetInstance().GetPosts(g_pTiebaOperate->GetForumID(), op.object->tid, _T("1"), posts, lzls);
						if (posts.size() > 0)
							pid = posts[0].pid;
						break;
					}
					case TBObject::POST:
						pid = ((PostInfo*)op.object.get())->pid;
						break;
					case TBObject::LZL:
						pid = ((LzlInfo*)op.object.get())->cid;
						break;
					}

					if (pid == _T(""))
						g_pLog->Log(_T("<font color=red>封禁 </font>") + op.object->author + _T("<font color=red> 失败！(获取帖子ID失败)</font>"));
				}

				CString code = (g_pTbmCoreConfig->m_banClientInterface || pid == _T("")) ?
					g_pTiebaOperate->BanIDClient(op.object->author) : g_pTiebaOperate->BanID(op.object->author, pid);
				if (code != _T("0"))
				{
					CString content;
					content.Format(_T("<font color=red>封禁 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
						_T("\"bd:%s,%s\">重试</a>"), (LPCTSTR)op.object->author, (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code),
						(LPCTSTR)op.object->author, (LPCTSTR)pid);
					g_pLog->Log(content);
				}
				else
				{
					result = TRUE;
					sndPlaySound(_T("封号.wav"), SND_ASYNC | SND_NODEFAULT);
					g_pUserCache->m_bannedUser->insert(op.object->author);
					g_pLog->Log(_T("<font color=red>封禁 </font>") + op.object->author);
				}

				g_postBanEvent(op, result);
			}
		}

		// 拉黑
		if (g_pTbmCoreConfig->m_defriend && count >= g_pTbmCoreConfig->m_defriendTrigCount
			&& g_pUserCache->m_defriendedUser->find(op.object->author) == g_pUserCache->m_defriendedUser->end()) // 达到拉黑违规次数且未拉黑
		{
			pass = TRUE;
			g_preDefriendEvent(op, pass);
			if (pass)
			{
				BOOL result = FALSE;
				CString code = g_pTiebaOperate->Defriend(op.object->authorID);
				if (code != _T("0"))
				{
					CString content;
					content.Format(_T("<font color=red>拉黑 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
						_T("\"df:%s\">重试</a>"), (LPCTSTR)op.object->author, (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code),
						(LPCTSTR)op.object->authorID);
					g_pLog->Log(content);
				}
				else
				{
					result = TRUE;
					sndPlaySound(_T("封号.wav"), SND_ASYNC | SND_NODEFAULT);
					g_pUserCache->m_defriendedUser->insert(op.object->author);
					g_pLog->Log(_T("<font color=red>拉黑 </font>") + op.object->author);
				}

				g_postDefriendEvent(op, result);
			}
		}

		// 主题已被删则不再删帖
		__int64 tid = _ttoi64(op.object->tid);
		if (g_pUserCache->m_deletedTID.find(tid) != g_pUserCache->m_deletedTID.end())
			continue;

		// 删帖
		if (g_pTbmCoreConfig->m_delete)
		{
			pass = TRUE;
			g_preDeleteEvent(op, pass);
			if (pass)
			{
				BOOL result = FALSE;
				if (op.object->m_type == TBObject::THREAD) // 主题
				{
				CaseThread:
					CString code = g_pTiebaOperate->DeleteThread(op.object->tid);
					if (code != _T("0"))
					{
						CString content;
						content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a><font color=red> 删除失败！错误代码：%s(%s)</font><a href=")
							_T("\"dt:%s\">重试</a>"), (LPCTSTR)op.object->tid, (LPCTSTR)HTMLEscape(op.title), (LPCTSTR)code,
							(LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)op.object->tid);
						g_pLog->Log(content);
					}
					else
					{
						result = TRUE;
						sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
						g_pUserCache->m_deletedTID.insert(_ttoi64(op.object->tid));
						g_pLog->Log(_T("<font color=red>删除 </font><a href=\"http://tieba.baidu.com/p/") + op.object->tid
							+ _T("\">") + HTMLEscape(op.title) + _T("</a>"));
						Sleep((DWORD)(g_pTbmCoreConfig->m_deleteInterval * 1000));
					}
				}
				else if (op.object->m_type == TBObject::POST) // 帖子
				{
					PostInfo* post = (PostInfo*)op.object.get();
					if (post->floor == _T("1"))
						goto CaseThread;

					CString code = g_pTiebaOperate->DeletePost(op.object->tid, post->pid);
					if (code != _T("0"))
					{
						CString content;
						content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a> %s楼<font color=red> 删除失败！错误代码：%s(%s)</font>")
							_T("<a href=\"dp:%s,%s\">重试</a>"), (LPCTSTR)op.object->tid, (LPCTSTR)HTMLEscape(op.title), (LPCTSTR)post->floor,
							(LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)op.object->tid, (LPCTSTR)post->pid);
						g_pLog->Log(content);
					}
					else
					{
						result = TRUE;
						sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
						g_pLog->Log(_T("<font color=red>删除 </font><a href=\"http://tieba.baidu.com/p/") + op.object->tid
							+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + post->floor + _T("楼"));
						Sleep((DWORD)(g_pTbmCoreConfig->m_deleteInterval * 1000));
					}
				}
				else if (op.object->m_type == TBObject::LZL) // 楼中楼
				{
					LzlInfo* lzl = (LzlInfo*)op.object.get();
					CString code = g_pTiebaOperate->DeleteLZL(op.object->tid, lzl->cid);
					if (code != _T("0"))
					{
						CString content;
						content.Format(_T("<a href=\"http://tieba.baidu.com/p/%s\">%s</a> %s楼回复<font color=red> 删除失败！错误代码：")
							_T("%s(%s)</font><a href=\"dl:%s,%s\">重试</a>"), (LPCTSTR)lzl->tid, (LPCTSTR)HTMLEscape(op.title),
							(LPCTSTR)lzl->floor, (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)lzl->tid, (LPCTSTR)lzl->cid);
						g_pLog->Log(content);
					}
					else
					{
						result = TRUE;
						sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
						g_pLog->Log(_T("<font color=red>删除 </font><a href=\"http://tieba.baidu.com/p/") + lzl->tid
							+ _T("\">") + HTMLEscape(op.title) + _T("</a> ") + lzl->floor + _T("楼回复"));
						Sleep((DWORD)(g_pTbmCoreConfig->m_deleteInterval * 1000));
					}
				}

				g_postDeleteEvent(op, result);
			}
		}
	}

	CoUninitialize();

	TRACE(_T("操作线程结束\n"));
}
