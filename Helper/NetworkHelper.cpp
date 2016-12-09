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
#include <NetworkHelper.h>
using std::regex_iterator;
#include <StringHelper.h>
#include <MiscHelper.h>
#include "msxml2.h" // C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Include\MsXml2.h
#import <winhttpcom.dll> no_namespace //#import <winhttp.dll> no_namespace


class CWinHttpBase
{
protected:
	DWORD m_lastError;

	void WriteError(LPCTSTR name, DWORD code)
	{
		m_lastError = code;
		CString error;
		error.Format(_T("%s: 0x%08X"), name, code);
		WriteString(error, _T("error.txt"));
	}

	void WriteError(LPCTSTR name)
	{
		WriteError(name, GetLastError());
	}

public:
	virtual ~CWinHttpBase() {}

	virtual BOOL IsEmpty() = 0;
	virtual HRESULT Open(LPCTSTR method, const CString& uri, BOOL async) = 0;
	virtual HRESULT SetRequestHeader(LPCTSTR header, LPCTSTR value) = 0;
	virtual HRESULT Send(const CString& data) = 0;
	virtual BOOL IsCompleted() = 0;
	virtual HRESULT Abort() = 0;
	virtual HRESULT GetAllResponseHeaders(CString& headers) = 0;
	virtual HRESULT GetStatus(long* status) = 0;
	virtual HRESULT GetResponseLocation(CString& location) = 0;
	virtual HRESULT GetResponseText(CString& body) = 0;
	virtual HRESULT GetResponseBody(unique_ptr<BYTE[]>* buffer, DWORD* size) = 0;

	static unique_ptr<CWinHttpBase> Create();
};

template<class Class>
class CServerXMLHTTPRequest : public CWinHttpBase, private IDispatch
{
	// 实现IUnknown
private:
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **ppvObject)
	{
		if (iid == IID_IDispatch || iid == IID_IUnknown)
		{
			*ppvObject = (iid == IID_IDispatch ? (IDispatch*)this : (IUnknown*)this);
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef() { return 0; }
	ULONG STDMETHODCALLTYPE Release() { return 0; }

	// 实现IDispatch
protected:
	BOOL m_completed = FALSE;

private:
	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *pctinfo) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) { return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) { return E_NOTIMPL; }

	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
		VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
	{
		long state = 0;
		if (FAILED(m_xml->get_readyState(&state)))
			return S_OK;
		if (state == 4)
			m_completed = TRUE;
		return S_OK;
	}

	// 实现CWinHttpBase
protected:
	CComPtr<IServerXMLHTTPRequest> m_xml;

public:
	CServerXMLHTTPRequest(LPCTSTR errorOutput)
	{
		HRESULT hr = m_xml.CoCreateInstance(__uuidof(Class));
		if (FAILED(hr))
			WriteError(errorOutput, hr);

		// 注册回调事件
		m_xml->put_onreadystatechange(this);
	}

	BOOL IsEmpty()
	{
		return m_xml.p == NULL;
	}

	HRESULT Open(LPCTSTR method, const CString& uri, BOOL async)
	{
		return m_xml->open(_bstr_t(method), _bstr_t(uri), _variant_t(async), _variant_t(), _variant_t());
	}
	
	HRESULT SetRequestHeader(LPCTSTR header, LPCTSTR value)
	{
		return m_xml->setRequestHeader(_bstr_t(header), _bstr_t(value));
	}

	HRESULT Send(const CString& data)
	{
		return m_xml->send(_variant_t(data));
	}
	
	BOOL IsCompleted()
	{
		return m_completed;
	}

	HRESULT Abort()
	{
		return m_xml->abort();
	}
	
	HRESULT GetAllResponseHeaders(CString& _headers)
	{
		_bstr_t headers;
		HRESULT res = m_xml->getAllResponseHeaders(headers.GetAddress());
		_headers = (LPCTSTR)headers;
		return res;
	}

	HRESULT GetStatus(long* status)
	{
		return m_xml->get_status(status);
	}

	HRESULT GetResponseLocation(CString& location)
	{
		_bstr_t _location;
		HRESULT res = m_xml->getResponseHeader(_bstr_t(_T("Location")), _location.GetAddress());
		location = (LPCTSTR)_location;
		return res;
	}
	
	HRESULT GetResponseText(CString& text)
	{
		_bstr_t _text;
		HRESULT res = m_xml->get_responseText(_text.GetAddress());
		text = (LPCTSTR)_text;
		return res;
	}

	HRESULT GetResponseBody(unique_ptr<BYTE[]>* buffer, DWORD* size)
	{
		if (buffer != NULL)
			*buffer = NULL;
		if (size != NULL)
			*size = 0;

		HRESULT res = E_FAIL;
		if (buffer != NULL && size != NULL)
		{
			_variant_t body;
			res = m_xml->get_responseBody(body.GetAddress());
			BYTE* p;
			if (SUCCEEDED(SafeArrayAccessData(body.parray, (void**)&p)))
			{
				*size = body.parray->rgsabound[0].cElements;
				buffer->reset(new BYTE[*size]);
				memcpy(buffer->get(), p, *size);
				SafeArrayUnaccessData(body.parray);
			}
		}

		return res;
	}
};

class CWinHttpRequest : public CWinHttpBase, private IWinHttpRequestEvents
{
	// 实现IUnknown
private:
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **ppvObject)
	{
		if (iid == __uuidof(IWinHttpRequestEvents) || iid == IID_IUnknown)
		{
			*ppvObject = (iid == __uuidof(IWinHttpRequestEvents) ? (IWinHttpRequestEvents*)this : (IUnknown*)this);
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef() { return 0; }
	ULONG STDMETHODCALLTYPE Release() { return 0; }

	// 实现IWinHttpRequestEvents
protected:
	BOOL m_completed = FALSE;

private:
	void __stdcall OnResponseStart(long Status, BSTR ContentType) {}
	void __stdcall OnResponseDataAvailable(SAFEARRAY** Data) {}
	void __stdcall OnError(long ErrorNumber, BSTR ErrorDescription) {}

	void __stdcall OnResponseFinished()
	{
		m_completed = TRUE;
	}

	// 实现CWinHttpBase
protected:
	CComPtr<IWinHttpRequest> m_request;
	DWORD m_dwAdvise;

public:
	CWinHttpRequest()
	{
		HRESULT hr = m_request.CoCreateInstance(__uuidof(WinHttpRequest));
		if (FAILED(hr))
		{
			WriteError(_T("CoCreateInstance(WinHttpRequest)"), hr);
			return;
		}

		// 注册回调事件
		CComPtr<IConnectionPointContainer> connectionPointContainer;
		hr = m_request.QueryInterface(&connectionPointContainer);
		if (FAILED(hr))
		{
			WriteError(_T("QueryInterface(IConnectionPointContainer)"), hr);
			return;
		}

		CComPtr<IConnectionPoint> connectionPoint;
		connectionPointContainer->FindConnectionPoint(__uuidof(IWinHttpRequestEvents), &connectionPoint);
		connectionPoint->Advise(this, &m_dwAdvise);
	}

	BOOL IsEmpty()
	{
		return m_request.p == NULL;
	}

	HRESULT Open(LPCTSTR method, const CString& uri, BOOL async)
	{
		return m_request->raw_Open(_bstr_t(method), _bstr_t(uri), _variant_t(TRUE));
	}

	HRESULT SetRequestHeader(LPCTSTR header, LPCTSTR value)
	{
		return m_request->raw_SetRequestHeader(_bstr_t(header), _bstr_t(value));
	}

	HRESULT Send(const CString& data)
	{
		return m_request->raw_Send(_variant_t(data));
	}

	BOOL IsCompleted()
	{
		return m_completed;
	}

	HRESULT Abort()
	{
		return m_request->raw_Abort();
	}

	HRESULT GetAllResponseHeaders(CString& _headers)
	{
		_bstr_t headers;
		HRESULT res = m_request->raw_GetAllResponseHeaders(headers.GetAddress());
		_headers = (LPCTSTR)headers;
		return res;
	}

	HRESULT GetStatus(long* status)
	{
		return m_request->get_Status(status);
	}

	HRESULT GetResponseLocation(CString& location)
	{
		_bstr_t _location;
		HRESULT res = m_request->raw_GetResponseHeader(_bstr_t(_T("Location")), _location.GetAddress());
		location = (LPCTSTR)_location;
		return res;
	}

	HRESULT GetResponseText(CString& text)
	{
		_bstr_t _text;
		HRESULT res = m_request->get_ResponseText(_text.GetAddress());
		text = (LPCTSTR)_text;
		return res;
	}

	HRESULT GetResponseBody(unique_ptr<BYTE[]>* buffer, DWORD* size)
	{
		if (buffer != NULL)
			*buffer = NULL;
		if (size != NULL)
			*size = 0;

		HRESULT res = E_FAIL;
		if (buffer != NULL && size != NULL)
		{
			_variant_t body;
			res = m_request->get_ResponseBody(body.GetAddress());
			BYTE* p;
			if (SUCCEEDED(SafeArrayAccessData(body.parray, (void**)&p)))
			{
				*size = body.parray->rgsabound[0].cElements;
				buffer->reset(new BYTE[*size]);
				memcpy(buffer->get(), p, *size);
				SafeArrayUnaccessData(body.parray);
			}
		}

		return res;
	}
};

unique_ptr<CWinHttpBase> CWinHttpBase::Create()
{
	unique_ptr<CWinHttpBase> res;

	res.reset(new CServerXMLHTTPRequest<ServerXMLHTTP60>(_T("CoCreateInstance(ServerXMLHTTP60)")));
	if (!res->IsEmpty())
		return res;

	res.reset(new CServerXMLHTTPRequest<ServerXMLHTTP40>(_T("CoCreateInstance(ServerXMLHTTP40)")));
	if (!res->IsEmpty())
		return res;

	res.reset(new CServerXMLHTTPRequest<ServerXMLHTTP30>(_T("CoCreateInstance(ServerXMLHTTP30)")));
	if (!res->IsEmpty())
		return res;

	res.reset(new CServerXMLHTTPRequest<ServerXMLHTTP>(_T("CoCreateInstance(ServerXMLHTTP)")));
	if (!res->IsEmpty())
		return res;

	res.reset(new CWinHttpRequest());
	return res;
}


// 从HTTP头提取Cookie并修改cookie
static void ReceiveCookie(const CString& headers, CString& cookie)
{
	static const wregex cookieExp(_T("Set-Cookie: (.*?)=(.*?);"));
	for (regex_iterator<LPCTSTR> it(headers, (LPCTSTR)headers + _tcslen(headers), cookieExp), end; it != end; ++it)
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
static HTTPRequestResult HTTPRequestBase(BOOL postMethod, CWinHttpBase& xml,
	const CString& URL, const CString& data, CString* cookie)
{
	if (xml.IsEmpty())
		return NET_FAILED_TO_CREATE_INSTANCE;

	if (postMethod)
	{
		xml.Open(_T("POST"), URL, true);
		xml.SetRequestHeader(_T("Content-Type"), _T("application/x-www-form-urlencoded"));
	}
	else
		xml.Open(_T("GET"), URL, true);
	if (cookie != NULL)
		xml.SetRequestHeader(_T("Cookie"), *cookie);
	xml.Send(data);

	// 等待
#pragma warning(suppress: 28159)
	DWORD startTime = GetTickCount();
	while (!xml.IsCompleted())
	{
		Delay(1);
#pragma warning(suppress: 28159)
		if (GetTickCount() - startTime > 10000)
		{
			xml.Abort();
			return NET_TIMEOUT;
		}
	}

	// 接收Cookie
	if (cookie != NULL)
	{
		CString headers;
		xml.GetAllResponseHeaders(headers);
		ReceiveCookie(headers, *cookie);
	}

	// 重定向
	long status;
	xml.GetStatus(&status);
	if (status == 302)
	{
		CString location;
		xml.GetResponseLocation(location);
		return HTTPRequestBase(postMethod, xml, URL, data, cookie);
	}

	return NET_SUCCESS;
}

// HTTP GET请求
HELPER_API CString HTTPGet(const CString& URL, CString* cookie)
{
	unique_ptr<CWinHttpBase> xml(CWinHttpBase::Create());
	HTTPRequestResult ret = HTTPRequestBase(FALSE, *xml, URL, NULL, cookie);
	if (ret != NET_SUCCESS)
	{
		CString result;
		switch (ret)
		{
		case NET_FAILED_TO_CREATE_INSTANCE:
			result = NET_FAILED_TO_CREATE_INSTANCE_TEXT;
			break;
		case NET_TIMEOUT:
			result = NET_TIMEOUT_TEXT;
			break;
		}
		return result;
	}

	CString result;
	xml->GetResponseText(result);
	return result;
}

// HTTP POST请求
HELPER_API CString HTTPPost(const CString& URL, const CString& data, CString* cookie)
{
	unique_ptr<CWinHttpBase> xml(CWinHttpBase::Create());
	HTTPRequestResult ret = HTTPRequestBase(TRUE, *xml, URL, data, cookie);
	if (ret != NET_SUCCESS)
	{
		CString result;
		switch (ret)
		{
		case NET_FAILED_TO_CREATE_INSTANCE:
			result = NET_FAILED_TO_CREATE_INSTANCE_TEXT;
			break;
		case NET_TIMEOUT:
			result = NET_TIMEOUT_TEXT;
			break;
		}
		return result;
	}

	CString result;
	xml->GetResponseText(result);
	return result;
}

// HTTP GET请求，取得原始数据
HELPER_API HTTPRequestResult HTTPGetRaw(const CString& URL, unique_ptr<BYTE[]>* buffer, ULONG* size, CString* cookie)
{
	unique_ptr<CWinHttpBase> xml(CWinHttpBase::Create());
	HTTPRequestResult ret = HTTPRequestBase(FALSE, *xml, URL, NULL, cookie);
	if (ret != NET_SUCCESS)
		return ret;

	// 返回
	xml->GetResponseBody(buffer, size);
	return NET_SUCCESS;
}
