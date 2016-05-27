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

#include "stdafx.h"
#include "TBMListeners.h"

#include "TiebaManager.h"
#include <TBMEvent.h>

#include <StringHelper.h>

#include "TBMConfig.h"
#include <TiebaOperate.h>
#include "TiebaManagerDlg.h"


CTBMListeners::CTBMListeners()
{
	CEventBus& eventBus = *theApp.m_tbmEventBus;
#define ADD_LISTENER(event_, listener) \
	eventBus.AddListener(event_, std::bind(&CTBMListeners::listener, this, std::placeholders::_1))
	ADD_LISTENER(OpenLinkInLogEvent, OnOpenLinkInLog);
}


void CTBMListeners::OnOpenLinkInLog(CEventBase* event__)
{
	COpenLinkEvent* event_ = (COpenLinkEvent*)event__;
	CTiebaOperate& tiebaOperate = *theApp.m_tiebaOperate;
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)theApp.m_pMainWnd;

	CString prefix = event_->m_url.Left(3);
	if (prefix == _T("dt:")) // 删主题
	{
		CString code = tiebaOperate.DeleteThread(event_->m_url.Right(event_->m_url.GetLength() - 3));
		if (code == _T("0"))
			dlg->m_log.Log(_T("<font color=green>删除成功！</font>"));
		else
			dlg->m_log.Log(_T("<font color=red>删除失败！</font>"));
		event_->canceled = TRUE;
	}
	else if (prefix == _T("dp:")) // 删帖子
	{
		CStringArray args;
		SplitString(args, event_->m_url.Right(event_->m_url.GetLength() - 3), _T(","));
		CString code = tiebaOperate.DeletePost(args[0], args[1]);
		if (code == _T("0"))
			dlg->m_log.Log(_T("<font color=green>删除成功！</font>"));
		else
			dlg->m_log.Log(_T("<font color=red>删除失败！</font>"));
		event_->canceled = TRUE;
	}
	else if (prefix == _T("dl:")) // 删楼中楼
	{
		CStringArray args;
		SplitString(args, event_->m_url.Right(event_->m_url.GetLength() - 3), _T(","));
		CString code = tiebaOperate.DeleteLZL(args[0], args[1]);
		if (code == _T("0"))
			dlg->m_log.Log(_T("<font color=green>删除成功！</font>"));
		else
			dlg->m_log.Log(_T("<font color=red>删除失败！</font>"));
		event_->canceled = TRUE;
	}
	else if (prefix == _T("bd:")) // 封ID
	{
		CStringArray args;
		SplitString(args, event_->m_url.Right(event_->m_url.GetLength() - 3), _T(","));
		CString code = (theApp.m_plan->m_wapBanInterface /*|| theApp.m_plan->m_banDuration == 1*/ || args[1] == _T("")) ?
			tiebaOperate.BanIDClient(args[0]) : tiebaOperate.BanID(args[0], args[1]);
		if (code == _T("0"))
			dlg->m_log.Log(_T("<font color=green>封禁成功！</font>"));
		else
			dlg->m_log.Log(_T("<font color=red>封禁失败！</font>"));
		event_->canceled = TRUE;
	}
	else if (prefix == _T("df:")) // 拉黑
	{
		CString code = tiebaOperate.Defriend(event_->m_url.Right(event_->m_url.GetLength() - 3));
		if (code == _T("0"))
			dlg->m_log.Log(_T("<font color=green>拉黑成功！</font>"));
		else
			dlg->m_log.Log(_T("<font color=red>拉黑失败！</font>"));
		event_->canceled = TRUE;
	}
}
