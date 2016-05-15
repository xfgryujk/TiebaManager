#include "stdafx.h"
#include "TBMScanListeners.h"

#include <TBMScan.h>
#include <TBMScanEvent.h>

#include <TBMConfig.h>
#include "ScanImage.h"

#include "TiebaManager.h"
#include "TiebaManagerDlg.h"


CTBMScanListeners::CTBMScanListeners(CTBMScan& scan) :
	m_scan(scan)
{
#define ADD_LISTENER(event_, listener) \
	m_scan.m_eventBus.AddListener(event_, std::bind(&CTBMScanListeners::listener, this, std::placeholders::_1))
	ADD_LISTENER(CheckThreadIllegalEvent, OnCheckThreadIllegal);
	ADD_LISTENER(CheckPostIllegalEvent, OnCheckPostIllegal);
	ADD_LISTENER(CheckLzlIllegalEvent, OnCheckLzlIllegal);
	ADD_LISTENER(CheckThreadImageIllegalEvent, OnCheckThreadImageIllegal);
	ADD_LISTENER(CheckPostImageIllegalEvent, OnCheckPostImageIllegal);
	ADD_LISTENER(CheckLzlImageIllegalEvent, OnCheckLzlImageIllegal);

	ADD_LISTENER(ScanThreadStartEvent, OnScanThreadStart);
	ADD_LISTENER(ScanThreadEndEvent, OnScanThreadEnd);
	ADD_LISTENER(ScanOnceStartEvent, OnScanOnceStart);
	ADD_LISTENER(ScanOnceEndEvent, OnScanOnceEnd);
	ADD_LISTENER(PreScanAllThreadsEvent, OnPreScanAllThreads);

	ADD_LISTENER(PreScanThreadEvent, OnPreScanThread);
	ADD_LISTENER(ScanPostThreadEndEvent, OnScanPostThreadEnd);

	ADD_LISTENER(ScanPostPageEvent, OnScanPostPage);
}


// 检查违规
static BOOL CheckIllegal(const CString& content, const CString& author, const CString& authorLevel, CString& msg, BOOL& forceToConfirm, int& pos, int& length)
{
	forceToConfirm = FALSE;
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
	if (StringMatchs(author, blackList))
	{
		pos = 0;
		length = 0;
		msg = _T("<font color=red> 触发屏蔽用户 </font>") + HTMLEscape(blackList.text);
		theApp.m_plan->m_optionsLock.Unlock();
		return TRUE;
	}


	// 信任内容
	for (const RegexText& whiteContent : *theApp.m_plan->m_whiteContent)
	if (StringIncludes(content, whiteContent))
	{
		theApp.m_plan->m_optionsLock.Unlock();
		return FALSE;
	}

	// 违规内容
	for (CPlan::Keyword& keyword : *theApp.m_plan->m_keywords)
	if (StringIncludes(content, keyword, &pos, &length))
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

void CTBMScanListeners::OnCheckThreadIllegal(CEventBase* event__)
{
	CCheckThreadIllegalEvent* event_ = (CCheckThreadIllegalEvent*)event__;
	event_->result = CheckIllegal(event_->m_thread.title + _T("\r\n") + event_->m_thread.preview, event_->m_thread.author, 
		_T(""), event_->m_msg, event_->m_forceToConfirm, event_->m_pos, event_->m_length);
}

void CTBMScanListeners::OnCheckPostIllegal(CEventBase* event__)
{
	CCheckPostIllegalEvent* event_ = (CCheckPostIllegalEvent*)event__;
	event_->result = CheckIllegal(event_->m_post.content, event_->m_post.author, event_->m_post.authorLevel, 
		event_->m_msg, event_->m_forceToConfirm, event_->m_pos, event_->m_length);
}

void CTBMScanListeners::OnCheckLzlIllegal(CEventBase* event__)
{
	CCheckPostIllegalEvent* event_ = (CCheckPostIllegalEvent*)event__;
	event_->result = CheckIllegal(event_->m_post.content, event_->m_post.author, _T(""),
		event_->m_msg, event_->m_forceToConfirm, event_->m_pos, event_->m_length);
}

void CTBMScanListeners::OnCheckThreadImageIllegal(CEventBase* event__)
{
	CCheckThreadIllegalEvent* event_ = (CCheckThreadIllegalEvent*)event__;
	event_->result = CheckImageIllegal(event_->m_thread.author, GetThreadImage(event_->m_thread), event_->m_msg);
}

void CTBMScanListeners::OnCheckPostImageIllegal(CEventBase* event__)
{
	CCheckPostIllegalEvent* event_ = (CCheckPostIllegalEvent*)event__;
	event_->result = CheckImageIllegal(event_->m_post.author, GetPostImage(event_->m_post), event_->m_msg);
}

void CTBMScanListeners::OnCheckLzlImageIllegal(CEventBase* event__)
{
	CCheckPostIllegalEvent* event_ = (CCheckPostIllegalEvent*)event__;
	event_->result = CheckImageIllegal(event_->m_post.author, GetPostImage(event_->m_post), event_->m_msg);
}


void CTBMScanListeners::OnScanThreadStart(CEventBase* event__)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	dlg->m_startButton.EnableWindow(FALSE);
	dlg->m_pageEdit.EnableWindow(FALSE);
	dlg->m_stopButton.EnableWindow(TRUE);
}

void CTBMScanListeners::OnScanThreadEnd(CEventBase* event__)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	dlg->m_stopButton.EnableWindow(FALSE);
	dlg->m_startButton.EnableWindow(TRUE);
	dlg->m_pageEdit.EnableWindow(TRUE);
	dlg->m_stateStatic.SetWindowText(_T("待机中"));
}

void CTBMScanListeners::OnScanOnceStart(CEventBase* event__)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	dlg->m_stateStatic.SetWindowText(_T("扫描主题中"));
	if (!theApp.m_plan->m_briefLog)
		dlg->m_log.Log(_T("<font color=green>本轮扫描开始，使用方案：</font>") + theApp.m_userConfig->m_plan);
}

void CTBMScanListeners::OnScanOnceEnd(CEventBase* event__)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	m_stateListLock.Lock();
	dlg->m_stateList.ResetContent();
	m_stateListLock.Unlock();
	dlg->m_stateStatic.SetWindowText(_T("延时中"));
}

void CTBMScanListeners::OnPreScanAllThreads(CEventBase* event__)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	dlg->m_stateStatic.SetWindowText(_T("扫描帖子中"));
	m_stateListLock.Lock();
	for (int i = 0; i < theApp.m_plan->m_threadCount; i++)
		dlg->m_stateList.AddString(_T("准备中"));
	m_stateListLock.Unlock();
}


void CTBMScanListeners::OnScanPostThreadEnd(CEventBase* event__)
{
	CScanPostThreadEvent* event_ = (CScanPostThreadEvent*)event__;
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	m_stateListLock.Lock();
	dlg->m_stateList.DeleteString(event_->m_threadID);
	dlg->m_stateList.InsertString(event_->m_threadID, _T("线程结束"));
	m_stateListLock.Unlock();
}

void CTBMScanListeners::OnPreScanThread(CEventBase* event__)
{
	CPreScanThreadEvent* event_ = (CPreScanThreadEvent*)event__;
	theApp.m_plan->m_optionsLock.Lock();
	if (theApp.m_plan->m_trustedThread->find(event_->m_thread.tid) != theApp.m_plan->m_trustedThread->end()) // 信任
		event_->canceled = TRUE;
	theApp.m_plan->m_optionsLock.Unlock();
}


void CTBMScanListeners::OnScanPostPage(CEventBase* event__)
{
	CScanPostPageEvent* event_ = (CScanPostPageEvent*)event__;
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;
	CString sPage;
	sPage.Format(_T("%d"), event_->m_page);
	m_stateListLock.Lock();
	dlg->m_stateList.DeleteString(event_->m_threadID);
	dlg->m_stateList.InsertString(event_->m_threadID, event_->m_thread.tid + _T(":") + sPage + _T(" ") + event_->m_thread.title);
	m_stateListLock.Unlock();
}
