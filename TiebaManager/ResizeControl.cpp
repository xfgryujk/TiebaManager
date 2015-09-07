#include "stdafx.h"
#include "ResizeControl.h"


CResizeControl::CResizeControl(CWnd* parentWnd)
{
	m_parentWnd = parentWnd;
}

void CResizeControl::AddControl(CWnd* wnd, ResizeType lType, CWnd* lWnd, ResizeType tType, CWnd* tWnd,
	ResizeType rType, CWnd* rWnd, ResizeType bType, CWnd* bWnd)
{
	if (rType == RT_NULL || rWnd == NULL)
	{
		rType = lType;
		rWnd = lWnd;
	}
	if (bType == RT_NULL || bWnd == NULL)
	{
		bType = tType;
		bWnd = tWnd;
	}

	ResizeInfo info = { wnd, lType, tType, rType, bType, lWnd, tWnd, rWnd, bWnd };
	RECT rect1, rect2;
	wnd->GetWindowRect(&rect1);
	m_parentWnd->ScreenToClient(&rect1);

	if (lType != RT_NULL && lWnd != NULL)
	{
		lWnd->GetWindowRect(&rect2);
		m_parentWnd->ScreenToClient(&rect2);
		switch (lType)
		{
		case RT_KEEP_DIST_TO_LEFT:
			info.lDist = rect1.left - rect2.left;
			break;
		case RT_KEEP_DIST_TO_RIGHT:
			info.lDist = rect1.left - rect2.right;
			break;
		}
	}

	if (tType != RT_NULL && tWnd != NULL)
	{
		tWnd->GetWindowRect(&rect2);
		m_parentWnd->ScreenToClient(&rect2);
		switch (tType)
		{
		case RT_KEEP_DIST_TO_TOP:
			info.tDist = rect1.top - rect2.top;
			break;
		case RT_KEEP_DIST_TO_BOTTOM:
			info.tDist = rect1.top - rect2.bottom;
			break;
		}
	}

	if (rType != RT_NULL && rWnd != NULL)
	{
		rWnd->GetWindowRect(&rect2);
		m_parentWnd->ScreenToClient(&rect2);
		switch (rType)
		{
		case RT_KEEP_DIST_TO_LEFT:
			info.rDist = rect1.right - rect2.left;
			break;
		case RT_KEEP_DIST_TO_RIGHT:
			info.rDist = rect1.right - rect2.right;
			break;
		}
	}

	if (bType != RT_NULL && bWnd != NULL)
	{
		bWnd->GetWindowRect(&rect2);
		m_parentWnd->ScreenToClient(&rect2);
		switch (bType)
		{
		case RT_KEEP_DIST_TO_TOP:
			info.bDist = rect1.bottom - rect2.top;
			break;
		case RT_KEEP_DIST_TO_BOTTOM:
			info.bDist = rect1.bottom - rect2.bottom;
			break;
		}
	}

	m_controlList.AddTail(info);
}

void CResizeControl::Resize()
{
	int count = m_controlList.GetCount();
	if (count <= 0)
		return;

	POSITION pos = m_controlList.GetHeadPosition();
	for (int i = 0; i < count; i++)
	{
		ResizeInfo& info = m_controlList.GetNext(pos);
		RECT rect1, rect2;
		info.wnd->GetWindowRect(&rect1);
		m_parentWnd->ScreenToClient(&rect1);

		if (info.lType != RT_NULL && info.lWnd != NULL)
		{
			info.lWnd->GetWindowRect(&rect2);
			m_parentWnd->ScreenToClient(&rect2);
			switch (info.lType)
			{
			case RT_KEEP_DIST_TO_LEFT:
				rect1.left = rect2.left + info.lDist;
				break;
			case RT_KEEP_DIST_TO_RIGHT:
				rect1.left = rect2.right + info.lDist;
				break;
			}
		}

		if (info.tType != RT_NULL && info.tWnd != NULL)
		{
			info.tWnd->GetWindowRect(&rect2);
			m_parentWnd->ScreenToClient(&rect2);
			switch (info.tType)
			{
			case RT_KEEP_DIST_TO_TOP:
				rect1.top = rect2.top + info.tDist;
				break;
			case RT_KEEP_DIST_TO_BOTTOM:
				rect1.top = rect2.bottom + info.tDist;
				break;
			}
		}

		if (info.rType != RT_NULL && info.rWnd != NULL)
		{
			info.rWnd->GetWindowRect(&rect2);
			m_parentWnd->ScreenToClient(&rect2);
			switch (info.rType)
			{
			case RT_KEEP_DIST_TO_LEFT:
				rect1.right = rect2.left + info.rDist;
				break;
			case RT_KEEP_DIST_TO_RIGHT:
				rect1.right = rect2.right + info.rDist;
				break;
			}
		}

		if (info.bType != RT_NULL && info.bWnd != NULL)
		{
			info.bWnd->GetWindowRect(&rect2);
			m_parentWnd->ScreenToClient(&rect2);
			switch (info.bType)
			{
			case RT_KEEP_DIST_TO_TOP:
				rect1.bottom = rect2.top + info.bDist;
				break;
			case RT_KEEP_DIST_TO_BOTTOM:
				rect1.bottom = rect2.bottom + info.bDist;
				break;
			}
		}

		info.wnd->SetWindowPos(NULL, rect1.left, rect1.top, rect1.right - rect1.left, rect1.bottom - rect1.top, SWP_NOREDRAW);
	}

	m_parentWnd->Invalidate();
}
