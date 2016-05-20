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
