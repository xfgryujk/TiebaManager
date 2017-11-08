/*
Copyright (C) 2011-2017  xfgryujk
https://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

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
#include <functional>
#include <map>
#include <crtdbg.h>


class EventBase
{
public:
	virtual ~EventBase() = default;

	virtual void DeleteListener(int listenerID) = 0;
	virtual void DeleteListenersOfModule(HMODULE module) = 0;
};

template<class KeyCmp, class... ArgTypes>
class Event final : public EventBase
{
public:
	typedef std::function<void(ArgTypes...)> FunctionType;

private:
	class Listener
	{
	public:
		FunctionType m_function;
		HMODULE m_module;
	};

	std::map<int, Listener, KeyCmp> m_listeners;
	int m_nextListenerID = 0;


public:
	// 返回listener ID，如果指定module则在插件卸载时会自动删除，否则需要手动删除
	int AddListener(FunctionType callback, HMODULE module = NULL)
	{
		int listenerID = m_nextListenerID++;
		Listener& listener = m_listeners[listenerID];
		listener.m_function = std::move(callback);
		listener.m_module = module;
		return listenerID;
	}

	void DeleteListener(int listenerID)
	{
		m_listeners.erase(listenerID);
	}

	void DeleteListenersOfModule(HMODULE module)
	{
		for (auto it = m_listeners.begin(); it != m_listeners.end();)
		{
			if (it->second.m_module == module)
				it = m_listeners.erase(it);
			else
				++it;
		}
	}

	void operator () (ArgTypes... args)
	{
		for (auto it = m_listeners.cbegin(); it != m_listeners.cend(); ++it)
		{
			try
			{
				it->second.m_function(std::forward<ArgTypes>(args)...);
			}
			catch (std::bad_function_call&)
			{
				_RPTF0(_CRT_WARN, "回调事件时出现bad_function_call\n");
			}

			if (m_listeners._Isnil(it._Mynode()))
			{
				_RPTF0(_CRT_WARN, "迭代调用事件时当前结点被删除\n"); // 出现这个说明有BUG...
				break;
			}
		}
	}
};

// 先监听先被调用
template<class... ArgTypes>
using PreEvent = Event<std::less<int>, ArgTypes...>;

// 先监听后被调用
template<class... ArgTypes>
using PostEvent = Event<std::greater<int>, ArgTypes...>;
