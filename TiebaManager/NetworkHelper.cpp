#include "stdafx.h"
#include "NetworkHelper.h"
#include <regex>
using std::wregex;
using std::regex_iterator;
extern CString g_cookie; //#include "Tieba.h"
#include "StringHelper.h"
#include "MiscHelper.h"
#include <msxml2.h>


// 从HTTP头提取Cookie并修改cookie
static void ReceiveCookie(LPCTSTR headers, CString& cookie)
{
	static const wregex cookieExp(_T("Set-Cookie: (.*?)=(.*?);"));
	for (regex_iterator<LPCTSTR> it(headers, headers + _tcslen(headers), cookieExp), end; it != end; it++)
	{
		CString name = (*it)[1].str().c_str();
		CString value = (*it)[2].str().c_str();
		int start = cookie.Find(name + _T("="));
		if (start == -1)
			cookie += name + _T("=") + value + _T(";");
		else
		{
			start += name.GetLength() + 1;
			int end = cookie.Find(_T(';'), start);
			cookie = cookie.Left(start) + value + cookie.Right(cookie.GetLength() - end);
		}
	}
}

// HTTP请求
static HTTPRequestResult HTTPRequestBase(BOOL postMethod, CComPtr<IServerXMLHTTPRequest>& xml,
	LPCTSTR URL, LPCTSTR data, BOOL useCookie, volatile BOOL* stopFlag, CString* cookie)
{
	HRESULT hr = xml.CoCreateInstance(__uuidof(ServerXMLHTTP));
	if (FAILED(hr))
	{
		CString code;
		code.Format(_T("CoCreateInstance: 0x%08X"), hr);
		WriteString(code, _T("error.txt"));
		return NET_FAILED_TO_CREATE_INSTANCE;
	}
	if (cookie == NULL)
		cookie = &g_cookie;

	if (postMethod)
	{
		xml->open(_bstr_t(_T("POST")), _bstr_t(URL), _variant_t(true), _variant_t(), _variant_t());
		xml->setRequestHeader(_bstr_t(_T("Content-Type")), _bstr_t(_T("application/x-www-form-urlencoded")));
	}
	else
		xml->open(_bstr_t(_T("GET")), _bstr_t(URL), _variant_t(true), _variant_t(), _variant_t());
	if (useCookie)
		xml->setRequestHeader(_bstr_t(_T("Cookie")), _bstr_t(*cookie));
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
		ReceiveCookie((LPCTSTR)headers, *cookie);
	}

	// 重定向
	long status;
	xml->get_status(&status);
	if (status == 302)
	{
		_bstr_t location;
		xml->getResponseHeader(_bstr_t(_T("Location")), location.GetAddress());
		return HTTPRequestBase(postMethod, xml, URL, data, useCookie, stopFlag, cookie);
	}

	return NET_SUCCESS;
}

// HTTP GET请求
CString HTTPGet(LPCTSTR URL, BOOL useCookie, volatile BOOL* stopFlag, CString* cookie)
{
	CComPtr<IServerXMLHTTPRequest> xml;
	HTTPRequestResult ret = HTTPRequestBase(FALSE, xml, URL, NULL, useCookie, stopFlag, cookie);
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
CString HTTPPost(LPCTSTR URL, LPCTSTR data, BOOL useCookie, volatile BOOL* stopFlag, CString* cookie)
{
	CComPtr<IServerXMLHTTPRequest> xml;
	HTTPRequestResult ret = HTTPRequestBase(TRUE, xml, URL, data, useCookie, stopFlag, cookie);
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
HTTPRequestResult HTTPGetRaw(LPCTSTR URL, BYTE** buffer, ULONG* size, BOOL useCookie, volatile BOOL* stopFlag, CString* cookie)
{
	if (buffer != NULL)
		*buffer = NULL;
	if (size != NULL)
		*size = 0;

	CComPtr<IServerXMLHTTPRequest> xml;
	HTTPRequestResult ret = HTTPRequestBase(FALSE, xml, URL, NULL, useCookie, stopFlag, cookie);
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
