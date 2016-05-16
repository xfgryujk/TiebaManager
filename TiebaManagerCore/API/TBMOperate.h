#pragma once
#include "TiebaManagerCoreCommon.h"
#include <EventHelper.h>
class ILog;
class CTBMCoreConfig;
class CUserCache;
class CTiebaOperate;
#include <BlockingQueue.h>


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

	Operation() = default;
	Operation(BOOL forceToConfirm_, const CString& msg_, TBObject object_, const CString& tid_, const CString& title_,
		const CString& floor_, const CString& pid_, const CString& author_, const CString& authorID_,
		const CString& authorPortrait_ = _T(""), int pos_ = 0, int length_ = 0) :
		forceToConfirm(forceToConfirm_),
		msg(msg_),
		object(object_),
		tid(tid_),
		title(title_),
		floor(floor_),
		pid(pid_),
		author(author_),
		authorID(authorID_),
		authorPortrait(authorPortrait_)
	{ }
};

class TIEBA_MANAGER_CORE_API CTBMOperate
{
public:
	// 可以注册事件监听，见TBMOperateEvent.h
	CEventBus m_eventBus;

	// 记得依赖注入哦
	ILog* m_log = NULL;
	CTBMCoreConfig* m_config = NULL;
	CUserCache* m_userCache = NULL;
	CTiebaOperate* m_tiebaOperate = NULL;


	CTBMOperate(CTBMCoreConfig* config = NULL, CUserCache* userCache = NULL, CTiebaOperate* tiebaOperate = NULL, ILog* log = NULL);
	virtual ~CTBMOperate();

	void AddConfirm(Operation&& op);
	void AddOperation(Operation&& op);

protected:
	unique_ptr<thread> m_confirmThread;
	unique_ptr<thread> m_operateThread;
	BlockingQueue<Operation> m_confirmQueue; // 确认队列
	BlockingQueue<Operation> m_operationQueue; // 操作队列


	void ConfirmThread();
	void OperateThread();
};
