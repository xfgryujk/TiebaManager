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

#pragma once
#include "TiebaManagerCoreCommon.h"
#include "Singleton.h"
#include "TBMCoreGlobal.h"
#include "BlockingQueue.h"
#include <memory>
#include <thread>


class TBM_CORE_API Operation final
{
public:
	BOOL forceToConfirm;	// 强制确认
	int pos;				// 高亮位置
	int length;				// 高亮长度

	CString title;			// 主题标题
	std::unique_ptr<TBObject> object; // 操作对象


	Operation() = default;

	Operation& operator = (Operation&& other)
	{
		forceToConfirm = other.forceToConfirm;
		pos = other.pos;
		length = other.length;
		title = std::move(other.title);
		object = std::move(other.object);
		return *this;
	}

	Operation(Operation&& other)
	{
		*this = std::move(other);
	}

	Operation(BOOL forceToConfirm_, int pos_, int length_, const CString& title_, std::unique_ptr<TBObject>&& object_) :
		forceToConfirm(forceToConfirm_),
		pos(pos_),
		length(length_),
		title(title_),
		object(std::move(object_))
	{ }
};

class TBM_CORE_API CTBMOperate final : public Singleton<CTBMOperate>
{
	DECL_SINGLETON(CTBMOperate);
	CTBMOperate();
	virtual ~CTBMOperate();

public:
	void AddConfirm(Operation&& op);
	void AddOperation(Operation&& op);

protected:
	std::unique_ptr<std::thread> m_confirmThread;
	std::unique_ptr<std::thread> m_operateThread;
	BlockingQueue<Operation> m_confirmQueue; // 确认队列
	BlockingQueue<Operation> m_operationQueue; // 操作队列


	void ConfirmThread();
	void OperateThread();
};
