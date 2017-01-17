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
TBM_CORE_API std::vector<EventBase*> g_externalEvents{
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
