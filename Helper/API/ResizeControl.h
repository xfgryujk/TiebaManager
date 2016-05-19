#pragma once
#include "HelperCommon.h"


enum ResizeType{ RT_NULL, RT_KEEP_DIST_TO_LEFT, RT_KEEP_DIST_TO_TOP, RT_KEEP_DIST_TO_RIGHT, RT_KEEP_DIST_TO_BOTTOM };

struct ResizeInfo
{
	CWnd* wnd;
	ResizeType lType; ResizeType tType; ResizeType rType; ResizeType bType;
	CWnd* lWnd; CWnd* tWnd; CWnd* rWnd; CWnd* bWnd;
	int lDist; int tDist; int rDist; int bDist;

	bool operator < (const ResizeInfo& other) const { return wnd < other.wnd; }
};

class HELPER_API CResizeControl
{
public:
	CResizeControl(CWnd* parentWnd);

	void AddControl(CWnd* wnd, ResizeType lType = RT_NULL, CWnd* lWnd = NULL, ResizeType tType = RT_NULL, CWnd* tWnd = NULL,
		ResizeType rType = RT_NULL, CWnd* rWnd = NULL, ResizeType bType = RT_NULL, CWnd* bWnd = NULL);
	void Resize();

	CWnd* m_parentWnd;
	set<ResizeInfo> m_controlList;
};
