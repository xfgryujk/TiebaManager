#pragma once
#include "TiebaManagerCommon.h"
#include <EventHelper.h>


enum TIEBA_MANAGER_API TBMEventID
{
	MainDialogPostInitEvent,		// CEventBase，主对话框初始化后触发
	MainDialogCloseEvent,			// CEventBase，主对话框关闭
	MainDialogDestroyEvent,			// CEventBase，主对话框销毁

	PreSetTiebaEvent,				// CSetTiebaEvent，准备确认贴吧
	PostSetTiebaEvent				// CSetTiebaEvent，确认贴吧成功后
};

class TIEBA_MANAGER_API CSetTiebaEvent : public CEventBase
{
public:
	const CString& m_forumName;

	CSetTiebaEvent(const CString& forumName);
};
