/*
Copyright (C) 2011-2017  xfgryujk
https://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

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

#include "stdafx.h"
#include <MiscHelper.h>


// 不阻塞消息的延迟
HELPER_API void Delay(DWORD time)
{
#pragma warning(suppress: 28159)
	DWORD startTime = GetTickCount();
#pragma warning(suppress: 28159)
	while (GetTickCount() - startTime < time)
	{
		DoEvents();
		Sleep(1); // 防止占用CPU
	}
}

// 处理消息
HELPER_API void DoEvents()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}
}

// 创建目录
HELPER_API BOOL CreateDir(const CString& path)
{
	if (PathFileExists(path))
		return TRUE;
	int pos = path.ReverseFind(_T('\\'));
	if (pos != -1)
	{
		CString parent = path.Left(pos);
		if (!CreateDir(parent))
			return FALSE;
	}
	return CreateDirectory(path, NULL);
}

// 初始化COM库
HELPER_API BOOL CoInitializeHelper()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		CString tmp;
		tmp.Format(_T("CoInitializeEx失败！\r\n错误代码0x%08X"), hr);
		AfxMessageBox(tmp, MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

// 判断线程是否在运行
HELPER_API BOOL IsThreadRunning(std::thread& thread_)
{
	DWORD exitCode = 0;
	if (!GetExitCodeThread(thread_.native_handle(), &exitCode))
	{
		TRACE(_T("GetExitCodeThread失败！错误代码0x%08X\n"), GetLastError());
		return WaitForSingleObject(thread_.native_handle(), 0) == WAIT_TIMEOUT;
	}
	return exitCode == STILL_ACTIVE;
}
