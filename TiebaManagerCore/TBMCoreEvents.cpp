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
#include <TBMCoreEvents.h>


#define DEF_VARIABLE(name) decltype(name) name

TBM_CORE_API DEF_VARIABLE(g_comfirmEvent);

TBM_CORE_API DEF_VARIABLE(g_preOperateEvent);
TBM_CORE_API DEF_VARIABLE(g_preBanEvent);
TBM_CORE_API DEF_VARIABLE(g_postBanEvent);
TBM_CORE_API DEF_VARIABLE(g_preDefriendEvent);
TBM_CORE_API DEF_VARIABLE(g_postDefriendEvent);
TBM_CORE_API DEF_VARIABLE(g_preDeleteEvent);
TBM_CORE_API DEF_VARIABLE(g_postDeleteEvent);

TBM_CORE_API DEF_VARIABLE(g_checkThreadIllegalEvent);
TBM_CORE_API DEF_VARIABLE(g_checkThreadImageIllegalEvent);
TBM_CORE_API DEF_VARIABLE(g_checkPostIllegalEvent);
TBM_CORE_API DEF_VARIABLE(g_checkPostImageIllegalEvent);
TBM_CORE_API DEF_VARIABLE(g_checkLzlIllegalEvent);
TBM_CORE_API DEF_VARIABLE(g_checkLzlImageIllegalEvent);

TBM_CORE_API DEF_VARIABLE(g_scanThreadStartEvent);
TBM_CORE_API DEF_VARIABLE(g_scanThreadEndEvent);
TBM_CORE_API DEF_VARIABLE(g_scanOnceStartEvent);
TBM_CORE_API DEF_VARIABLE(g_scanOnceEndEvent);
TBM_CORE_API DEF_VARIABLE(g_preScanAllThreadsEvent);

TBM_CORE_API DEF_VARIABLE(g_scanPostThreadStartEvent);
TBM_CORE_API DEF_VARIABLE(g_scanPostThreadEndEvent);
TBM_CORE_API DEF_VARIABLE(g_preScanThreadEvent);

TBM_CORE_API DEF_VARIABLE(g_scanPostPageEvent);


// 外部模块可注册的事件集合，用来自动卸载外部listener
TBM_CORE_API std::vector<EventBase*> g_tbmCoreEvents{
	&g_comfirmEvent,

	&g_preOperateEvent,
	&g_preBanEvent,
	&g_postBanEvent,
	&g_preDefriendEvent,
	&g_postDefriendEvent,
	&g_preDeleteEvent,
	&g_postDeleteEvent,

	&g_checkThreadIllegalEvent,
	&g_checkThreadImageIllegalEvent,
	&g_checkPostIllegalEvent,
	&g_checkPostImageIllegalEvent,
	&g_checkLzlIllegalEvent,
	&g_checkLzlImageIllegalEvent,

	&g_scanThreadStartEvent,
	&g_scanThreadEndEvent,
	&g_scanOnceStartEvent,
	&g_scanOnceEndEvent,
	&g_preScanAllThreadsEvent,

	&g_scanPostThreadStartEvent,
	&g_scanPostThreadEndEvent,
	&g_preScanThreadEvent
};
