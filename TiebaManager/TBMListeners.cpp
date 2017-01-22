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
#include "TBMListeners.h"

#include "TiebaManager.h"
#include <TBMScan.h>
#include <TBMEvents.h>

#include <StringHelper.h>

#include "TBMConfig.h"
#include "GetImages.h"
#include "ScanImage.h"
#include <TiebaOperate.h>

#include "TiebaManagerDlg.h"
#include "ConfirmDlg.h"

using namespace std::placeholders;


CTBMListeners::CTBMListeners()
{
	// 扫描事件

	g_checkThreadIllegalEvent.AddListener(OnCheckThreadIllegal);
	g_checkPostIllegalEvent.AddListener(OnCheckPostIllegal);
	g_checkLzlIllegalEvent.AddListener(OnCheckLzlIllegal);
	g_checkThreadImageIllegalEvent.AddListener(OnCheckThreadImageIllegal);
	g_checkPostImageIllegalEvent.AddListener(OnCheckPostImageIllegal);
	g_checkLzlImageIllegalEvent.AddListener(OnCheckLzlImageIllegal);

	g_scanThreadStartEvent.AddListener(OnScanThreadStart);
	g_scanThreadEndEvent.AddListener(OnScanThreadEnd);
	g_scanOnceStartEvent.AddListener(OnScanOnceStart);
	g_scanOnceEndEvent.AddListener(std::bind(&CTBMListeners::OnScanOnceEnd, this));
	g_preScanAllThreadsEvent.AddListener(std::bind(&CTBMListeners::OnPreScanAllThreads, this, _1));

	g_preScanThreadEvent.AddListener(std::bind(&CTBMListeners::OnPreScanThread, this, _1, _2, _3));
	g_scanPostThreadEndEvent.AddListener(std::bind(&CTBMListeners::OnScanPostThreadEnd, this, _1));

	g_scanPostPageEvent.AddListener(std::bind(&CTBMListeners::OnScanPostPage, this, _1, _2, _3, _4));


	// 操作事件

	g_comfirmEvent.AddListener(OnConfirm);


	// UI事件

	g_openLinkInLogEvent.AddListener(OnOpenLinkInLog);
}


// 扫描事件

BOOL CTBMListeners::CheckIllegal(const CString& content, const CString& author, const CString& authorLevel,
	CString& msg, BOOL& forceToConfirm, int& pos, int& length)
{
	theApp.m_plan->m_optionsLock.Lock();

	// 信任用户
	if (theApp.m_plan->m_whiteList->find(author) != theApp.m_plan->m_whiteList->end())
	{
		theApp.m_plan->m_optionsLock.Unlock();
		return FALSE;
	}

	// 违规等级
	if (theApp.m_plan->m_illegalLevel > 0 && authorLevel != _T("") && _ttoi(authorLevel) <= theApp.m_plan->m_illegalLevel)
	{
		pos = 0;
		length = 0;
		msg.Format(_T("<font color=red> 触发等级小于或等于 </font>%d"), *theApp.m_plan->m_illegalLevel);
		theApp.m_plan->m_optionsLock.Unlock();
		return TRUE;
	}

	// 屏蔽用户
	for (const RegexText& blackList : *theApp.m_plan->m_blackList)
	if (blackList.text != _T("") && StringMatchs(author, blackList))
	{
		pos = 0;
		length = 0;
		msg = _T("<font color=red> 触发屏蔽用户 </font>") + HTMLEscape(blackList.text);
		theApp.m_plan->m_optionsLock.Unlock();
		return TRUE;
	}


	// 信任内容
	for (const RegexText& whiteContent : *theApp.m_plan->m_whiteContent)
	if (whiteContent.text != _T("") && StringIncludes(content, whiteContent))
	{
		theApp.m_plan->m_optionsLock.Unlock();
		return FALSE;
	}

	// 违规内容
	for (CPlan::Keyword& keyword : *theApp.m_plan->m_keywords)
	if (keyword.text != _T("") && StringIncludes(content, keyword, &pos, &length))
	{
		keyword.trigCount++;
		forceToConfirm = keyword.forceToConfirm;
		msg = _T("<font color=red> 触发违禁词 </font>") + HTMLEscape(keyword.text);
		theApp.m_plan->m_optionsLock.Unlock();
		return TRUE;
	}

	theApp.m_plan->m_optionsLock.Unlock();
	return FALSE;
}

void CTBMListeners::OnCheckThreadIllegal(const ThreadInfo& thread, BOOL& res, CString& msg, BOOL& forceToConfirm, int& pos, int& length)
{
	res = CheckIllegal(thread.GetContent(), thread.author, _T(""), msg, forceToConfirm, pos, length);
}

void CTBMListeners::OnCheckPostIllegal(const PostInfo& post, BOOL& res, CString& msg, BOOL& forceToConfirm, int& pos, int& length)
{
	res = CheckIllegal(post.GetContent(), post.author, post.authorLevel, msg, forceToConfirm, pos, length);
}

void CTBMListeners::OnCheckLzlIllegal(const LzlInfo& lzl, BOOL& res, CString& msg, BOOL& forceToConfirm, int& pos, int& length)
{
	res = CheckIllegal(lzl.GetContent(), lzl.author, _T(""), msg, forceToConfirm, pos, length);
}

void CTBMListeners::OnCheckThreadImageIllegal(const ThreadInfo& thread, BOOL& res, CString& msg, BOOL& forceToConfirm, int& pos, int& length)
{
	res = theApp.m_scanImage->CheckImageIllegal(thread.author, CGetImages(thread), msg);
}

void CTBMListeners::OnCheckPostImageIllegal(const PostInfo& post, BOOL& res, CString& msg, BOOL& forceToConfirm, int& pos, int& length)
{
	res = theApp.m_scanImage->CheckImageIllegal(post.author, CGetImages(post), msg);
}

void CTBMListeners::OnCheckLzlImageIllegal(const LzlInfo& lzl, BOOL& res, CString& msg, BOOL& forceToConfirm, int& pos, int& length)
{
	res = theApp.m_scanImage->CheckImageIllegal(lzl.author, CGetImages(lzl), msg);
}


void CTBMListeners::OnScanThreadStart(BOOL& pass)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	dlg->m_startButton.EnableWindow(FALSE);
	dlg->m_pageEdit.EnableWindow(FALSE);
	dlg->m_stopButton.EnableWindow(TRUE);
}

void CTBMListeners::OnScanThreadEnd()
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	dlg->m_stopButton.EnableWindow(FALSE);
	dlg->m_startButton.EnableWindow(TRUE);
	dlg->m_pageEdit.EnableWindow(TRUE);
	dlg->m_stateStatic.SetWindowText(_T("待机中"));
}

void CTBMListeners::OnScanOnceStart(BOOL& pass)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	dlg->m_stateStatic.SetWindowText(_T("扫描主题中"));
	if (!theApp.m_plan->m_briefLog)
		dlg->m_log.Log(_T("<font color=green>本轮扫描开始，使用方案：</font>") + theApp.m_userConfig->m_plan);
}

void CTBMListeners::OnScanOnceEnd()
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	m_stateListLock.Lock();
	dlg->m_stateList.ResetContent();
	m_stateListLock.Unlock();
	dlg->m_stateStatic.SetWindowText(_T("延时中"));
}

void CTBMListeners::OnPreScanAllThreads(BOOL& pass)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	dlg->m_stateStatic.SetWindowText(_T("扫描帖子中"));
	m_stateListLock.Lock();
	for (int i = 0; i < theApp.m_plan->m_threadCount; i++)
		dlg->m_stateList.AddString(_T("准备中"));
	m_stateListLock.Unlock();
}


void CTBMListeners::OnPreScanThread(int threadID, const ThreadInfo& thread, BOOL& pass)
{
	// 检查信任主题
	theApp.m_plan->m_optionsLock.Lock();
	if (theApp.m_plan->m_trustedThread->find(thread.tid) != theApp.m_plan->m_trustedThread->end())
	{
		pass = FALSE;
		theApp.m_plan->m_optionsLock.Unlock();
		return;
	}
	theApp.m_plan->m_optionsLock.Unlock();

	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	m_stateListLock.Lock();
	dlg->m_stateList.DeleteString(threadID);
	dlg->m_stateList.InsertString(threadID, thread.tid + _T(" ") + thread.title);
	m_stateListLock.Unlock();
}

void CTBMListeners::OnScanPostThreadEnd(int threadID)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	m_stateListLock.Lock();
	dlg->m_stateList.DeleteString(threadID);
	dlg->m_stateList.InsertString(threadID, _T("线程结束"));
	m_stateListLock.Unlock();
}


void CTBMListeners::OnScanPostPage(int threadID, const ThreadInfo& thread, int page, BOOL& pass)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	CString sPage;
	sPage.Format(_T("%d"), page);
	m_stateListLock.Lock();
	dlg->m_stateList.DeleteString(threadID);
	dlg->m_stateList.InsertString(threadID, thread.tid + _T(":") + sPage + _T(" ") + thread.title);
	m_stateListLock.Unlock();
}


// 操作事件

void CTBMListeners::OnConfirm(const Operation& op, BOOL& res)
{
	res = CConfirmDlg(&op).DoModal() != IDCANCEL;
}


// UI事件

void CTBMListeners::OnOpenLinkInLog(const CString& url, BOOL& pass)
{
	CTiebaOperate& tiebaOperate = *theApp.m_tiebaOperate;
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;

	CString prefix = url.Left(3);
	if (prefix == _T("dt:")) // 删主题
	{
		CString code = tiebaOperate.DeleteThread(url.Right(url.GetLength() - 3));
		if (code == _T("0"))
			dlg->m_log.Log(_T("<font color=green>删除成功！</font>"));
		else
			dlg->m_log.Log(_T("<font color=red>删除失败！</font>"));
		pass = FALSE;
	}
	else if (prefix == _T("dp:")) // 删帖子
	{
		CStringArray args;
		SplitString(args, url.Right(url.GetLength() - 3), _T(","));
		CString code = tiebaOperate.DeletePost(args[0], args[1]);
		if (code == _T("0"))
			dlg->m_log.Log(_T("<font color=green>删除成功！</font>"));
		else
			dlg->m_log.Log(_T("<font color=red>删除失败！</font>"));
		pass = FALSE;
	}
	else if (prefix == _T("dl:")) // 删楼中楼
	{
		CStringArray args;
		SplitString(args, url.Right(url.GetLength() - 3), _T(","));
		CString code = tiebaOperate.DeleteLZL(args[0], args[1]);
		if (code == _T("0"))
			dlg->m_log.Log(_T("<font color=green>删除成功！</font>"));
		else
			dlg->m_log.Log(_T("<font color=red>删除失败！</font>"));
		pass = FALSE;
	}
	else if (prefix == _T("bd:")) // 封ID
	{
		CStringArray args;
		SplitString(args, url.Right(url.GetLength() - 3), _T(","));
		CString code = (theApp.m_plan->m_wapBanInterface /*|| theApp.m_plan->m_banDuration == 1*/ || args[1] == _T("")) ?
			tiebaOperate.BanIDClient(args[0]) : tiebaOperate.BanID(args[0], args[1]);
		if (code == _T("0"))
			dlg->m_log.Log(_T("<font color=green>封禁成功！</font>"));
		else
			dlg->m_log.Log(_T("<font color=red>封禁失败！</font>"));
		pass = FALSE;
	}
	else if (prefix == _T("df:")) // 拉黑
	{
		CString code = tiebaOperate.Defriend(url.Right(url.GetLength() - 3));
		if (code == _T("0"))
			dlg->m_log.Log(_T("<font color=green>拉黑成功！</font>"));
		else
			dlg->m_log.Log(_T("<font color=red>拉黑失败！</font>"));
		pass = FALSE;
	}
}
