#include "stdafx.h"
#include <EventHelper.h>


int CEventBus::AddListener(int eventID, std::function<void(CEventBase*)>&& listener)
{
	int& listenerID = m_nextListenerID[eventID];
	m_listeners[eventID][listenerID] = listener;
	return listenerID++;
}

void CEventBus::DeleteListener(int eventID, int listenerID)
{
	m_listeners[eventID].erase(listenerID);
}

BOOL CEventBus::Post(int eventID, CEventBase& event_)
{
	for (const auto& i : m_listeners[eventID])
	{
		i.second(&event_);
		if (event_.canceled)
			return FALSE;
	}
	return TRUE;
}
