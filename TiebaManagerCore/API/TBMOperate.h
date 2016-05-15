#pragma once
#include "TiebaManagerCoreCommon.h"
#include <EventHelper.h>
class ILog;
class CTBMCoreConfig;
class CUserCache;
class CTiebaOperate;
#include <BlockingQueue.h>


class TIEBA_MANAGER_CORE_API CTBMOperate
{
public:
	// 可以注册事件监听，见TBMOperateEvent.h
	CEventBus m_eventBus;

	// 改变本对象行为的一些配置
	ILog* m_log = NULL;
	CTBMCoreConfig* m_config = NULL;
	CUserCache* m_userCache = NULL;
	unique_ptr<CTiebaOperate> m_tiebaOperate;

	// 关于操作
	thread m_confirmThread;
	thread m_operateThread;

	struct Operation
	{
		enum TBObject { TBOBJ_THREAD, TBOBJ_POST, TBOBJ_LZL };

		BOOL forceToConfirm;	// 强制确认
		CString msg;			// 提示消息
		int pos;				// 高亮位置
		int length;				// 高亮长度
		TBObject object;		// 操作对象
		CString tid;			// 主题ID
		CString title;			// 主题标题
		CString floor;			// 楼层
		CString pid;			// 帖子ID
		CString author;			// 帖子作者
		CString authorID;		// 帖子作者ID
		CString authorPortrait;	// 帖子作者头像哈希
	};
	BlockingQueue<Operation> m_confirmQueue; // 确认队列
	BlockingQueue<Operation> m_operationQueue; // 操作队列


	CTBMOperate(CString& cookie, const int& banDuration, const CString& banReason);
	virtual ~CTBMOperate();

	void AddConfirm(BOOL forceToConfirm, const CString& msg, Operation::TBObject object, const CString& tid, const CString& title,
		const CString& floor, const CString& pid, const CString& author, const CString& authorID,
		const CString& authorPortrait = _T(""), int pos = 0, int length = 0);
protected:
	void ConfirmThread();
	void AddOperation(Operation&& op);
	void OperateThread();
};
