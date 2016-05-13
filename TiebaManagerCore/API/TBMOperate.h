#pragma once
#include <TiebaOperate.h>


struct Operation
{
	enum TBObject { TBOBJ_THREAD, TBOBJ_POST, TBOBJ_LZL };

	BOOL forceToConfirm; // 强制确认
	CString msg;		// 提示消息
	int pos;			// 高亮位置
	int length;			// 高亮长度
	TBObject object;	// 操作对象
	CString tid;		// 主题ID
	CString title;		// 主题标题
	CString floor;		// 楼层
	CString pid;		// 帖子ID
	CString author;		// 帖子作者
	CString authorID;	// 帖子作者ID
	CString authorPortrait;	// 帖子作者头像哈希
};


void AddConfirm(BOOL forceToConfirm, const CString& msg, Operation::TBObject object, const CString& tid, const CString& title,
	const CString& floor, const CString& pid, const CString& author, const CString& authorID,
	const CString& authorPortrait = _T(""), int pos = 0, int length = 0);
UINT AFX_CDECL ConfirmThread(LPVOID mainDlg);
void AddOperation(const Operation& op);
UINT AFX_CDECL OperateThread(LPVOID mainDlg);

extern unique_ptr<CTiebaOperate> g_tiebaOperate;
