/*
Copyright (C) 2015  xfgryujk
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


enum HTTPRequestResult { NET_SUCCESS, NET_FAILED_TO_CREATE_INSTANCE, NET_TIMEOUT, };
const TCHAR NET_FAILED_TO_CREATE_INSTANCE_TEXT[] = _T("failed to create instance");
const TCHAR NET_TIMEOUT_TEXT[] = _T("timeout");

HELPER_API CString HTTPGet(const CString& URL, CString* cookie = NULL);
HELPER_API CString HTTPPost(const CString& URL, const CString& data, CString* cookie = NULL);
HELPER_API HTTPRequestResult HTTPGetRaw(const CString& URL, unique_ptr<BYTE[]>* buffer = NULL, ULONG* size = NULL, CString* cookie = NULL);
