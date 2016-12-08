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
#include <queue>
#include <mutex>
#include <condition_variable>


enum PopResult{ POP_OK, POP_STOP, POP_UNEXPECTED };

template<class T>
class BlockingQueue : public std::queue<T>
{
public:
	std::mutex m_lock;
	std::condition_variable m_cond;
	bool m_stopFlag = false;


	virtual ~BlockingQueue() = default;

	void push(const T& value)
	{
		std::lock_guard<decltype(m_lock)> lock(m_lock);
		queue::push(value);
		m_cond.notify_one();
	}

	void push(T&& value)
	{
		std::lock_guard<decltype(m_lock)> lock(m_lock);
		queue::push(std::move(value));
		m_cond.notify_one();
	}

	PopResult pop(T& out)
	{
		std::unique_lock<decltype(m_lock)> lock(m_lock);
		if (m_stopFlag) // 停止
			return POP_STOP;
		if (empty())
			m_cond.wait(lock);
		if (m_stopFlag) // 停止
			return POP_STOP;
		if (empty()) // 意外唤醒？
			return POP_UNEXPECTED;

		out = std::move(front());
		queue::pop();
		return POP_OK;
	}

	void Stop()
	{
		std::lock_guard<decltype(m_lock)> lock(m_lock);
		m_stopFlag = true;
		m_cond.notify_all();
	}
};
