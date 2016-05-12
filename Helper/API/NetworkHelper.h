#pragma once
#include "HelperCommon.h"


enum HTTPRequestResult { NET_SUCCESS, NET_FAILED_TO_CREATE_INSTANCE, NET_TIMEOUT, };
const TCHAR NET_FAILED_TO_CREATE_INSTANCE_TEXT[] = _T("failed to create instance");
const TCHAR NET_TIMEOUT_TEXT[] = _T("timeout");

HELPER_API CString HTTPGet(const CString& URL, CString* cookie = NULL);
HELPER_API CString HTTPPost(const CString& URL, const CString& data, CString* cookie = NULL);
HELPER_API HTTPRequestResult HTTPGetRaw(const CString& URL, unique_ptr<BYTE[]>* buffer = NULL, ULONG* size = NULL, CString* cookie = NULL);
