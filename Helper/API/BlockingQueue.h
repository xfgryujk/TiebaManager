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
		queue::push(value);
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

		out = front();
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
