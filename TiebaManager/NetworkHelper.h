#pragma once

enum HTTPRequestResult { NET_SUCCESS, NET_FAILED_TO_CREATE_INSTANCE, NET_STOP, NET_TIMEOUT, };
const TCHAR NET_FAILED_TO_CREATE_INSTANCE_TEXT[] = _T("failed to create instance");
const TCHAR NET_STOP_TEXT[] = _T("stopped");
const TCHAR NET_TIMEOUT_TEXT[] = _T("timeout");
CString HTTPGet(LPCTSTR URL, BOOL useCookie = TRUE, volatile BOOL* stopFlag = NULL, CString* cookie = NULL);
CString HTTPPost(LPCTSTR URL, LPCTSTR data, BOOL useCookie = TRUE, volatile BOOL* stopFlag = NULL, CString* cookie = NULL);
HTTPRequestResult HTTPGetRaw(LPCTSTR URL, BYTE** buffer = NULL, ULONG* size = NULL, BOOL useCookie = TRUE, volatile BOOL* stopFlag = NULL, CString* cookie = NULL);
