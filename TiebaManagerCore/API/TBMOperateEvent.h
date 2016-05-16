#pragma once
#include "TiebaManagerCoreCommon.h"
#include <EventHelper.h>
#include <TBMOperate.h>


enum TIEBA_MANAGER_CORE_API TBMOperateEventID
{
	ComfirmEvent,					// COperateEvent，确认是否操作，使用result

	PreOperateEvent,				// COperateEvent，准备操作时触发
	PreBanEvent,					// COperateEvent，准备封禁时触发
	PostBanEvent,					// COperateEvent，已封禁时触发，result为封禁结果
	PreDefriendEvent,				// COperateEvent，准备拉黑时触发
	PostDefriendEvent,				// COperateEvent，已拉黑时触发，result为拉黑结果
	PreDeleteEvent,					// COperateEvent，准备删除时触发
	PostDeleteEvent					// COperateEvent，已删除时触发，result为删除结果
};

class TIEBA_MANAGER_CORE_API COperateEvent : public CEventBase
{
public:
	const Operation& m_op;

	COperateEvent(const Operation& op);
};
