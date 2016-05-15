#pragma once
#include "HelperCommon.h"
#include <functional>


class HELPER_API CEventBase
{
public:
	virtual ~CEventBase() = default;

	BOOL result = FALSE;
	BOOL canceled = FALSE;
};

class HELPER_API CEventBus
{
protected:
	// eventID -> listenerID
	map<int, int> m_nextListenerID;
	// eventID -> listenerID -> listener
	map<int, map<int, std::function<void(CEventBase*)> > > m_listeners;

public:
	// 返回listener ID
	int AddListener(int eventID, std::function<void(CEventBase*)>&& listener);
	void DeleteListener(int eventID, int listenerID);
	// 如果事件被取消返回FALSE
	BOOL Post(int eventID, CEventBase& event_ = CEventBase());
};
