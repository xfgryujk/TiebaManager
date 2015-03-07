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

#include "stdafx.h"
#include "Global.h"
#include "Tieba.h"
#include <vector>
#include <oleacc.h>
#include <msxml2.h>
#include <regex>
using std::wregex;
using std::regex_iterator;
#import "msscript.ocx" no_namespace



// 分割字符串
void SplitString(CStringArray& dst, const CString& src, LPCTSTR slipt)
{
	dst.RemoveAll();
	const int len = _tcslen(slipt);

	int start = 0, end = 0;
	while ((end = src.Find(slipt, end)) != -1)
	{
		dst.Add(src.Mid(start, end - start));
		start = end += len;
	}
	dst.Add(src.Right(src.GetLength() - start));
}

// 取字符串之间的字符串
CString GetStringBetween(const CString& src, const CString& left, LPCTSTR right, int startPos)
{
	int leftPos = src.Find(left, startPos);
	if (leftPos == -1)
		return _T("");
	leftPos += left.GetLength();
	int rightPos = src.Find(right, leftPos);
	return src.Mid(leftPos, rightPos - leftPos);
}

// 取字符串之前的字符串
CString GetStringBefore(const CString& src, LPCTSTR right, int startPos)
{
	int rightPos = src.Find(right, startPos);
	if (rightPos == -1)
		return _T("");
	return src.Left(rightPos);
}

// 写字符串到文件
BOOL WriteString(const CString& src, LPCTSTR path)
{
	CFile file;
	if (!file.Open(path, CFile::modeCreate | CFile::modeWrite))
		return FALSE;
#ifdef _UNICODE
	static const BYTE UNICODE_HEADER[] = { 0xFF, 0xFE };
	file.Write(UNICODE_HEADER, sizeof(UNICODE_HEADER));
#endif
	file.Write((LPCTSTR)src, src.GetLength() * sizeof(TCHAR));
	return TRUE;
}

// URL编码
CString EncodeURI(const CString& src)
{
	CComPtr<IScriptControl> script;
	if (FAILED(script.CoCreateInstance(__uuidof(ScriptControl))))
		return _T("");
	script->PutLanguage("JScript");
	_variant_t param = src;
	SAFEARRAY* params = SafeArrayCreateVector(VT_VARIANT, 0, 1);
	LONG index = 0;
	SafeArrayPutElement(params, &index, &param);
	_variant_t result = script->Run(_bstr_t(_T("encodeURIComponent")), &params);
	SafeArrayDestroy(params);
	return (LPCTSTR)(_bstr_t)result;
}

// URL编码 GBK版
CString EncodeURI_GBK(const CString& _src)
{
	CString result, tmp;
	CStringA src(_src);
	const int len = src.GetLength();
	for (int i = 0; i < len; i++)
	{
		tmp.Format(_T("%%%02X"), src[i] & 0xFF);
		result += tmp;
	}
	return result;
}

// HTML转义
CString HTMLEscape(const CString& src)
{
	CString result = src;
	result.Replace(_T("&"), _T("&amp;"));
	result.Replace(_T(" "), _T("&nbsp;"));
	result.Replace(_T("<"), _T("&lt;"));
	result.Replace(_T(">"), _T("&gt;"));
	return result;
}

// HTML反转义
CString HTMLUnescape(const CString& src)
{
	CString result = src;
	//result.Replace(_T("<br>"), _T("\r\n")); // 不转换行符
	result.Replace(_T("&nbsp;"), _T(" "));
	result.Replace(_T("&quot;"), _T("\""));
	result.Replace(_T("&&#039;"), _T("'"));
	result.Replace(_T("&lt;"), _T("<"));
	result.Replace(_T("&gt;"), _T(">"));
	result.Replace(_T("&amp;"), _T("&"));
	return result;
}

// JS反转义，需自行转义双引号
CString JSUnescape(const CString& src)
{
	CComPtr<IScriptControl> script;
	if (FAILED(script.CoCreateInstance(__uuidof(ScriptControl))))
		return _T("");
	script->PutLanguage("JScript");
	_variant_t result = script->Eval((LPCTSTR)(_T("\"") + src + _T("\"")));
	return (LPCTSTR)(_bstr_t)result;
}


// 从HTTP头提取Cookie并修改g_cookie
static void ReceiveCookie(LPCTSTR headers)
{
	static const wregex cookieExp(_T("Set-Cookie: (.*?)=(.*?);"));
	for (regex_iterator<LPCTSTR> it(headers, headers + _tcslen(headers), cookieExp), end; it != end; it++)
	{
		CString name = (*it)[1].str().c_str();
		CString value = (*it)[2].str().c_str();
		int start = g_cookie.Find(name + _T("="));
		if (start == -1)
			g_cookie += name + _T("=") + value + _T(";");
		else
		{
			start += name.GetLength() + 1;
			int end = g_cookie.Find(_T(';'), start);
			g_cookie = g_cookie.Left(start) + value + g_cookie.Right(g_cookie.GetLength() - end);
		}
	}
}

// HTTP请求
static HTTPRequestResult HTTPRequestBase(BOOL postMethod, CComPtr<IServerXMLHTTPRequest>& xml, 
	LPCTSTR URL, LPCTSTR data, BOOL useCookie, volatile BOOL* stopFlag)
{
	if (FAILED(xml.CoCreateInstance(__uuidof(ServerXMLHTTP))))
		return NET_FAILED_TO_CREATE_INSTANCE;

	if (postMethod)
	{
		xml->open(_bstr_t(_T("POST")), _bstr_t(URL), _variant_t(true), _variant_t(), _variant_t());
		xml->setRequestHeader(_bstr_t(_T("Content-Type")), _bstr_t(_T("application/x-www-form-urlencoded")));
	}
	else
		xml->open(_bstr_t(_T("GET")), _bstr_t(URL), _variant_t(true), _variant_t(), _variant_t());
	if (useCookie)
		xml->setRequestHeader(_bstr_t(_T("Cookie")), _bstr_t(g_cookie));
	xml->send(_variant_t(data));

	// 等待
	DWORD startTime = GetTickCount();
	for (long state = 0; state != 4; xml->get_readyState(&state))
	{
		Delay(1);
		if (stopFlag != NULL && *stopFlag)
		{
			xml->abort();
			return NET_STOP;
		}
		if (GetTickCount() - startTime > 10000)
		{
			xml->abort();
			return NET_TIMEOUT;
		}
	}

	// 接收Cookie
	if (useCookie)
	{
		_bstr_t headers;
		xml->getAllResponseHeaders(headers.GetAddress());
		ReceiveCookie((LPCTSTR)headers);
	}

	// 重定向
	long status;
	xml->get_status(&status);
	if (status == 302)
	{
		_bstr_t location;
		xml->getResponseHeader(_bstr_t(_T("Location")), location.GetAddress());
		return HTTPRequestBase(postMethod, xml, URL, data, useCookie, stopFlag);
	}

	return NET_SUCCESS;
}

// HTTP GET请求
CString HTTPGet(LPCTSTR URL, BOOL useCookie, volatile BOOL* stopFlag)
{
	CComPtr<IServerXMLHTTPRequest> xml;
	HTTPRequestResult ret = HTTPRequestBase(FALSE, xml, URL, NULL, useCookie, stopFlag);
	if (ret != NET_SUCCESS)
		switch (ret)
		{
		case NET_FAILED_TO_CREATE_INSTANCE:
			return NET_FAILED_TO_CREATE_INSTANCE_TEXT;
		case NET_STOP:
			return NET_STOP_TEXT;
		case NET_TIMEOUT:
			return NET_TIMEOUT_TEXT;
		}

	_bstr_t result;
	xml->get_responseText(result.GetAddress());
	return (LPCTSTR)result;
}

// HTTP POST请求
CString HTTPPost(LPCTSTR URL, LPCTSTR data, BOOL useCookie, volatile BOOL* stopFlag)
{
	CComPtr<IServerXMLHTTPRequest> xml;
	HTTPRequestResult ret = HTTPRequestBase(TRUE, xml, URL, data, useCookie, stopFlag);
	if (ret != NET_SUCCESS)
		switch (ret)
	{
		case NET_FAILED_TO_CREATE_INSTANCE:
			return NET_FAILED_TO_CREATE_INSTANCE_TEXT;
		case NET_STOP:
			return NET_STOP_TEXT;
		case NET_TIMEOUT:
			return NET_TIMEOUT_TEXT;
	}

	_bstr_t result;
	xml->get_responseText(result.GetAddress());
	return (LPCTSTR)result;
}

// HTTP GET请求，取得原始数据，注意自行delete buffer!!!
HTTPRequestResult HTTPGetRaw(LPCTSTR URL, BYTE** buffer, ULONG* size, BOOL useCookie, volatile BOOL* stopFlag)
{
	if (buffer != NULL)
		*buffer = NULL;
	if (size != NULL)
		*size = 0;

	CComPtr<IServerXMLHTTPRequest> xml;
	HTTPRequestResult ret = HTTPRequestBase(FALSE, xml, URL, NULL, useCookie, stopFlag);
	if (ret != NET_SUCCESS)
		return ret;

	// 返回
	if (buffer != NULL && size != NULL)
	{
		_variant_t body;
		xml->get_responseBody(body.GetAddress());
		BYTE* p;
		if (SUCCEEDED(SafeArrayAccessData(body.parray, (void**)&p)))
		{
			*size = body.parray->rgsabound[0].cElements;
			*buffer = new BYTE[*size];
			memcpy(*buffer, p, *size);
			SafeArrayUnaccessData(body.parray);
		}
	}
	return NET_SUCCESS;
}

// 检查更新
CheckUpdateResult CheckUpdate()
{
	static const wregex CHECK_UPDATE_REG(_T("\"fs_id\":(\\d+),\"path\":\"\\\\/\\\\u6211\\\\u7684\\\\\
u5206\\\\u4eab\\\\/\\\\u767e\\\\u5ea6\\\\u8d34\\\\u5427\\\\u76f8\\\\u5173\\\\/\\\\u8d34\\\\u5427\\\\\
u7ba1\\\\u7406\\\\u5668.zip\",\"server_filename\":\"\\\\u8d34\\\\u5427\\\\u7ba1\\\\u7406\\\\u5668.zip\
\".*?\"server_ctime\":(\\d+),"));
	CString src = HTTPGet(_T("http://pan.baidu.com/share/list?channel=chunlei&clienttype=0&web=1\
&num=100&page=1&dir=%2F%E6%88%91%E7%9A%84%E5%88%86%E4%BA%AB%2F%E7%99%BE%E5%BA%A6%E8%B4%B4%E5%90%A7\
%E7%9B%B8%E5%85%B3&uk=436464474&shareid=497149087"), FALSE);
	std::wcmatch res;
	if (!std::regex_search((LPCTSTR)src, res, CHECK_UPDATE_REG))
		return UPDATE_FAILED_TO_GET_FILE_ID;

	// 文件ID
	CString fs_id = res[1].str().c_str();

	// 上传时间戳
	CString server_ctime = res[2].str().c_str();
	time_t tServer_ctime = _ttoi64(server_ctime);
	tm tmServer_ctime;
	localtime_s(&tmServer_ctime, &tServer_ctime);
	// 上传时间
	CString time;
	time.Format(_T("%d-%02d-%02d"), tmServer_ctime.tm_year % 100, tmServer_ctime.tm_mon + 1, tmServer_ctime.tm_mday);
	if (time == UPDATE_CURRENT_VERSION)
		return UPDATE_NO_UPDATE;

	if (AfxMessageBox(_T("最新版本") + time + _T("，是否更新？"), MB_ICONQUESTION | MB_YESNO) == IDNO)
		return UPDATE_HAS_UPDATE;

	static const TCHAR TIME_STAMP_LEFT[]		= _T("yunData.TIMESTAMP = \"");
	static const TCHAR TIME_STAMP_RIGHT[]		= _T("\"");
	static const TCHAR SIGN_LEFT[]				= _T("yunData.SIGN = \"");
	static const TCHAR SIGN_RIGHT[]				= _T("\"");
	static const TCHAR BDSTOKEN_LEFT[]			= _T("yunData.MYBDSTOKEN = \"");
	static const TCHAR BDSTOKEN_RIGHT[]			= _T("\"");
	static const TCHAR DLINK_LEFT[]				= _T("\"dlink\":\"");
	static const TCHAR DLINK_RIGHT[]			= _T("\"");
	src = HTTPGet(_T("http://pan.baidu.com/s/1hq86os8"), FALSE);
	CString timeStamp = GetStringBetween(src, TIME_STAMP_LEFT, TIME_STAMP_RIGHT);
	CString sign = GetStringBetween(src, SIGN_LEFT, SIGN_RIGHT);
	CString bdstoken = GetStringBetween(src, BDSTOKEN_LEFT, BDSTOKEN_RIGHT);
	CString url;
	url.Format(_T("http://pan.baidu.com/api/sharedownload?sign=%s&timestamp=%s\
&bdstoken=%s&channel=chunlei&clienttype=0&web=1&app_id=250528"),
	sign, timeStamp, bdstoken);
	src = HTTPPost(url, _T("encrypt=0&product=share&uk=436464474&primaryid=497149087&fid_list=%5B")
		+ fs_id + _T("%5D"), FALSE);

	// 文件下载链接
	CString dlink = GetStringBetween(src, DLINK_LEFT, DLINK_RIGHT);
	if (dlink == _T(""))
		return UPDATE_FAILED_TO_GET_LINK;

	dlink.Replace(_T("\\/"), _T("/"));
	ShellExecute(NULL, _T("open"), dlink, NULL, NULL, SW_NORMAL);
	return UPDATE_HAS_UPDATE;
}


// 不阻塞消息的延迟
void Delay(DWORD time)
{
	DWORD startTime = GetTickCount();
	while (GetTickCount() - startTime < time)
	{
		DoEvents();
		Sleep(1); // 防止占用CPU
	}
}

// 处理消息
void DoEvents()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}
}
