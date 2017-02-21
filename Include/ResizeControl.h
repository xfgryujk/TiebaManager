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
#include "HelperCommon.h"
#include <set>


enum ResizeType{ RT_NULL, RT_KEEP_DIST_TO_LEFT, RT_KEEP_DIST_TO_TOP, RT_KEEP_DIST_TO_RIGHT, RT_KEEP_DIST_TO_BOTTOM };

class ResizeInfo
{
public:
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
	std::set<ResizeInfo> m_controlList;
};
