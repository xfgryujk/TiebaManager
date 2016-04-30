#pragma once

void Delay(DWORD time);
void DoEvents();
LONG WINAPI ExceptionHandler(_EXCEPTION_POINTERS* ExceptionInfo);
BOOL CreateDir(const CString& path);
BOOL CoInitializeHelper();
