#include "stdafx.h"
#include <TBMScanEvent.h>


CCheckIllegalEventBase::CCheckIllegalEventBase(CString& msg, BOOL& forceToConfirm, int& pos, int& length) :
	m_msg(msg),
	m_forceToConfirm(forceToConfirm),
	m_pos(pos),
	m_length(length)
{

}

CCheckThreadIllegalEvent::CCheckThreadIllegalEvent(const ThreadInfo& thread, CString& msg, BOOL& forceToConfirm, int& pos, int& length) :
	CCheckIllegalEventBase(msg, forceToConfirm, pos, length),
	m_thread(thread)
{

}

CCheckPostIllegalEvent::CCheckPostIllegalEvent(const PostInfo& post, CString& msg, BOOL& forceToConfirm, int& pos, int& length) :
	CCheckIllegalEventBase(msg, forceToConfirm, pos, length),
	m_post(post)
{

}

CScanPostThreadEvent::CScanPostThreadEvent(int threadID) :
	m_threadID(threadID)
{

}

CPreScanThreadEvent::CPreScanThreadEvent(const ThreadInfo& thread, int threadID) :
	CScanPostThreadEvent(threadID),
	m_thread(thread)
{

}

CScanPostPageEvent::CScanPostPageEvent(const ThreadInfo& thread, int threadID, int page) :
	CPreScanThreadEvent(thread, threadID),
	m_page(page)
{
	
}
