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
