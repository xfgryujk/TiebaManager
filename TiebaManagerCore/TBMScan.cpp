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
#include <TBMScan.h>
#include <TBMCoreEvents.h>

#include <TBMCoreGlobal.h>
#include <TiebaClawerProxy.h>
#include <TiebaOperate.h>
#include <TBMOperate.h>

#include <StringHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>


CTBMScan::~CTBMScan()
{
	StopScan();
	if (m_scanThread != nullptr && m_scanThread->joinable())
		m_scanThread->join();
}

// 开始扫描
void CTBMScan::StartScan(const CString& sPage)
{
	StopScan();
	if (m_scanThread != nullptr && m_scanThread->joinable())
		m_scanThread->join();
	m_scanThread = std::make_unique<std::thread>(&CTBMScan::ScanThread, this, sPage);
}

// 结束扫描
void CTBMScan::StopScan()
{
	m_stopScanFlag = TRUE;
}

// 正在扫描
BOOL CTBMScan::IsScanning()
{
	return m_scanThread != nullptr && IsThreadRunning(*m_scanThread);
}

// 总扫描线程
void CTBMScan::ScanThread(CString sPage)
{
	BOOL pass = TRUE;
	g_scanThreadStartEvent(pass);
	if (!pass)
		goto ScanThreadEnd;

	m_stopScanFlag = FALSE;

	// 初始化
	if (!CoInitializeHelper())
		return;

	{
		// 初始化页数
		int iPage = _ttoi(sPage);
		CString ignoreThread; // 忽略前几个主题
		ignoreThread.Format(_T("%d"), (iPage - 1) * 50);


		while (!m_stopScanFlag)
		{
			pass = TRUE;
			g_scanOnceStartEvent(pass);
			if (!pass)
				goto ScanOnceEnd;

#pragma warning(suppress: 28159)
			DWORD startTime = GetTickCount();

			// 获取主题列表
			if (!TiebaClawerProxy::GetInstance().GetThreads(g_pTiebaOperate->GetForumName(), ignoreThread, m_threads))
			{
				if (m_stopScanFlag)
					break;
				if (!g_pTbmCoreConfig->m_briefLog)
					g_pLog->Log(_T("<font color=red>获取主题列表失败，重新开始本轮</font>"));
				continue;
			}

			// 扫描主题
			for (const ThreadInfo& thread : m_threads)
			{
				if (m_stopScanFlag)
					break;
				__int64 tid = _ttoi64(thread.tid);
				if (g_pUserCache->m_ignoredTID.find(tid) == g_pUserCache->m_ignoredTID.end())
				{
					BOOL res = FALSE;
					CString msg;
					BOOL forceToConfirm = FALSE;
					int pos = 0, length = 0;
					g_checkThreadIllegalEvent(thread, res, msg, forceToConfirm, pos, length);
					if (res)
					{
						CTBMOperate::GetInstance().AddConfirm(Operation(forceToConfirm, pos, length, thread.title,
							std::make_unique<ThreadInfo>(thread)));
						g_pLog->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">")
							+ HTMLEscape(thread.title) + _T("</a>") + msg);
						g_pUserCache->m_ignoredTID.insert(tid);
					}
				}
			}

			// 扫描帖子
			if (!g_pTbmCoreConfig->m_onlyScanTitle)
			{
				pass = TRUE;
				g_preScanAllThreadsEvent(pass);
				if (pass)
				{
					m_threadIndex = 0;

					// 创建线程扫描帖子
					int threadCount = g_pTbmCoreConfig->m_threadCount; // g_pTbmCoreConfig->m_threadCount会变
					std::vector<std::thread> threads;
					for (int i = 0; i < threadCount; i++)
						threads.push_back(std::thread(&CTBMScan::ScanPostThread, this, i));

					// 等待扫描帖子线程结束
					for (auto& i : threads)
						i.join();
				}
			}

			if (!g_pTbmCoreConfig->m_briefLog)
			{
				CString content;
#pragma warning(suppress: 28159)
				content.Format(_T("<font color=green>本轮扫描结束，用时%.3f秒</font>"), (float)(GetTickCount() - startTime) / 1000.0f);
				g_pLog->Log(content);
			}

		ScanOnceEnd:
			g_scanOnceEndEvent();

			// 延时
			int count = g_pTbmCoreConfig->m_scanInterval * 10;
			for (int i = 0; i < count; i++)
			{
				if (m_stopScanFlag)
					break;
				Sleep(100);
			}
		}
	}

	m_stopScanFlag = FALSE;

	if (!g_pTbmCoreConfig->m_briefLog)
		g_pLog->Log(_T("<font color=green>扫描结束</font>"));

	CoUninitialize();

ScanThreadEnd:
	g_scanThreadEndEvent();

	TRACE(_T("总扫描线程结束\n"));
}

// 扫描帖子线程
void CTBMScan::ScanPostThread(int threadID)
{
	BOOL pass = TRUE;
	g_scanPostThreadStartEvent(threadID, pass);
	if (!pass)
		goto ScanPostThreadEnd;

	// 初始化
	if (!CoInitializeHelper())
		return;

	{
		AdditionalThreadInfo addition;
		m_threadIndexLock.lock();
		while (!m_stopScanFlag && m_threadIndex < (int)m_threads.size())
		{
			ThreadInfo& thread = m_threads[m_threadIndex++];
			m_threadIndexLock.unlock();
			if (g_pUserCache->m_deletedTID.find(_ttoi64(thread.tid)) != g_pUserCache->m_deletedTID.end()) // 已删
				goto Next;

			__int64 tid = _ttoi64(thread.tid);
			int reply = _ttoi(thread.reply);
			BOOL hasHistoryReply = FALSE;
			{
				auto historyReplyIt = g_pUserCache->m_reply->find(tid);
				hasHistoryReply = historyReplyIt != g_pUserCache->m_reply->end();
				if (hasHistoryReply
					&& reply == historyReplyIt->second // 回复数减少时也扫描，防止漏掉
					&& thread.lastAuthor == (*g_pUserCache->m_lastAuthor)[tid]) // 判断最后回复人，防止回复数-1然后有新回复+1
				{
					// 无新回复，跳过
					historyReplyIt->second = reply;
					goto Next;
				}
			}

			pass = TRUE;
			g_preScanThreadEvent(threadID, thread, pass);
			if (!pass)
				goto Next;


			// 获取第一页
			{
				std::vector<PostInfo> posts;
				std::vector<LzlInfo> lzls;
				if (TiebaClawerProxy::GetInstance().GetPosts(g_pTiebaOperate->GetForumID(), thread.tid, _T("1"), posts, lzls, &addition) != TiebaClawer::GET_POSTS_SUCCESS)
				{
					if (!g_pTbmCoreConfig->m_briefLog)
					{
						g_pLog->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">") + thread.title
							+ _T("</a> <font color=red>获取贴子列表失败，暂时跳过</font>"));
					}
					goto Next;
				}
			}

			// 判断贴吧ID，避免百度乱插其他吧的帖子
			if (addition.fid != g_pTiebaOperate->GetForumID())
				goto Next;

			// 扫描帖子页
			int iPageCount = _ttoi(addition.pageCount);
			BOOL res = ScanPostPage(thread, 1, hasHistoryReply, 0, addition.src, threadID); // 扫描第一页
			if (iPageCount > 1 && !m_stopScanFlag)
				res = ScanPostPage(thread, iPageCount, hasHistoryReply, 0, _T(""), threadID); // 从最后一页扫描


			// 记录历史回复
			if (res)
			{
				(*g_pUserCache->m_reply)[tid] = reply;
				(*g_pUserCache->m_lastAuthor)[tid] = thread.lastAuthor;
			}

		Next:
			m_threadIndexLock.lock();
		}
		m_threadIndexLock.unlock();
	}

	CoUninitialize();
	
ScanPostThreadEnd:
	g_scanPostThreadEndEvent(threadID);
	TRACE(_T("扫描帖子线程结束\n"));
}

// 扫描帖子页
BOOL CTBMScan::ScanPostPage(const ThreadInfo& thread, int page, BOOL hasHistoryReply,
	int ScanedCount, const CString& src, int threadID)
{
	BOOL pass = TRUE;
	g_scanPostPageEvent(threadID, thread, page, pass);
	if (!pass)
		return FALSE;

	CString sPage;
	sPage.Format(_T("%d"), page);

	// 获取帖子列表
	std::vector<PostInfo> posts;
	std::vector<LzlInfo> lzls;
	TiebaClawer::GetPostsResult res;
	if (src == _T(""))
		res = TiebaClawerProxy::GetInstance().GetPosts(g_pTiebaOperate->GetForumID(), thread.tid, sPage, posts, lzls);
	else
		res = TiebaClawerProxy::GetInstance().GetPosts(g_pTiebaOperate->GetForumID(), thread.tid, sPage, src, posts, lzls);
	switch (res)
	{
	case TiebaClawer::GET_POSTS_TIMEOUT:
	case TiebaClawer::GET_POSTS_DELETED:
		g_pLog->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">") + thread.title
			+ _T("</a> <font color=red>获取贴子列表失败(") + (res == TiebaClawer::GET_POSTS_TIMEOUT ? _T("超时") :
			_T("可能已被删")) + _T(")，暂时跳过</font>"));
		return FALSE;
	}

	auto& operate = CTBMOperate::GetInstance();

	// 扫描帖子
	for (const PostInfo& post : posts)
	{
		if (m_stopScanFlag)
			return FALSE;
		__int64 pid = _ttoi64(post.pid);
		if (g_pUserCache->m_ignoredPID.find(pid) == g_pUserCache->m_ignoredPID.end())
		{
			BOOL res = FALSE;
			CString msg;
			BOOL forceToConfirm = FALSE;
			int pos = 0, length = 0;
			g_checkPostIllegalEvent(post, res, msg, forceToConfirm, pos, length);
			if (res)
			{
				operate.AddConfirm(Operation(forceToConfirm, pos, length, thread.title,
					std::make_unique<PostInfo>(post)));
				g_pLog->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">") + HTMLEscape(thread.title) +
					_T("</a> ") + post.floor + _T("楼") + msg);
				g_pUserCache->m_ignoredPID.insert(pid);
			}
		}
	}

	// 扫描楼中楼
	for (const LzlInfo& lzl : lzls)
	{
		if (m_stopScanFlag)
			return FALSE;
		BOOL res = FALSE;
		CString msg;
		BOOL forceToConfirm = FALSE;
		int pos = 0, length = 0;
		g_checkLzlIllegalEvent(lzl, res, msg, forceToConfirm, pos, length);
		if (res)
		{
			__int64 cid = _ttoi64(lzl.cid);
			if (g_pUserCache->m_ignoredLZLID.find(cid) == g_pUserCache->m_ignoredLZLID.end())
			{
				operate.AddConfirm(Operation(forceToConfirm, pos, length, thread.title,
					std::make_unique<LzlInfo>(lzl)));
				g_pLog->Log(_T("<a href=\"http://tieba.baidu.com/p/") + thread.tid + _T("\">") + HTMLEscape(thread.title) +
					_T("</a> ") + lzl.floor + _T("楼回复") + msg);
				g_pUserCache->m_ignoredLZLID.insert(cid);
			}
		}
	}

	// 递归扫描上一页
	if (!hasHistoryReply) // 如果有历史回复前面几页很可能被扫描过了，不递归
	{
		if (++ScanedCount < g_pTbmCoreConfig->m_scanPageCount) // 没达到最大扫描页数
		{
			if (--page < 2) // 扫描完
				return TRUE;
			return ScanPostPage(thread, page, FALSE, ScanedCount, _T(""), threadID);
		}
	}
	return TRUE;
}
