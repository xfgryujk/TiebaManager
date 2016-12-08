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
