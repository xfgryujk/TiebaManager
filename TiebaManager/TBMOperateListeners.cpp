#include "stdafx.h"
#include "TBMOperateListeners.h"

#include <TBMOperate.h>
#include <TBMOperateEvent.h>

#include "TBMConfig.h"
#include "TBMConfigPath.h"

#include "ConfirmDlg.h"
#include "TiebaManager.h"
#include "SuperFunctionDlg.h"


CTBMOperateListeners::CTBMOperateListeners(CTBMOperate& operate) :
	m_operate(operate)
{
#define ADD_LISTENER(event_, listener) \
	m_operate.m_eventBus.AddListener(event_, std::bind(&CTBMOperateListeners::listener, this, std::placeholders::_1))
	ADD_LISTENER(ComfirmEvent, OnComfirm);

	ADD_LISTENER(PostBanEvent, OnPostBan);
}


void CTBMOperateListeners::OnComfirm(CEventBase* event__)
{
	COperateEvent* event_ = (COperateEvent*)event__;
	event_->result = CConfirmDlg(&event_->m_op).DoModal() != IDCANCEL;
}


void CTBMOperateListeners::OnPostBan(CEventBase* event__)
{
	COperateEvent* event_ = (COperateEvent*)event__;
	// 自动循环封
	if (theApp.m_plan->m_autoLoopBan)
	{
		CLoopBanConfig config;
		config.Load(CURRENT_USER_DIR_PATH + _T("options2.xml"));
		auto it = std::find(config.m_userList->cbegin(), config.m_userList->cend(), event_->m_op.author);
		if (it == config.m_userList->cend())
		{
			config.m_userList->push_back(event_->m_op.author);
			config.m_pidList->push_back(_T(""));
			config.Save(CURRENT_USER_DIR_PATH + _T("options2.xml"));
			DeleteFile(CURRENT_USER_DIR_PATH + _T("LoopBanDate.xml"));
		}
	}
}
