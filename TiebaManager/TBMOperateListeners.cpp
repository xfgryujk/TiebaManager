#include "stdafx.h"
#include "TBMOperateListeners.h"

#include <TBMOperate.h>
#include <TBMOperateEvent.h>

#include "TBMConfig.h"
#include "TBMConfigPath.h"

#include "ConfirmDlg.h"
#include "TiebaManager.h"


CTBMOperateListeners::CTBMOperateListeners(CTBMOperate& operate) :
	m_operate(operate)
{
#define ADD_LISTENER(event_, listener) \
	m_operate.m_eventBus.AddListener(event_, std::bind(&CTBMOperateListeners::listener, this, std::placeholders::_1))
	ADD_LISTENER(ComfirmEvent, OnComfirm);
}


void CTBMOperateListeners::OnComfirm(CEventBase* event__)
{
	COperateEvent* event_ = (COperateEvent*)event__;
	event_->result = CConfirmDlg(&event_->m_op).DoModal() != IDCANCEL;
}
