#include "stdafx.h"
#include "NetworkHelper.h"
using std::regex_iterator;
#include "TiebaVariable.h" // extern CString g_cookie;
#include "StringHelper.h"
#include "MiscHelper.h"
#include "msxml2.h" // C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Include\MsXml2.h
//#import <winhttp.dll> no_namespace
#import <winhttpcom.dll> no_namespace


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
	virtual HRESULT GetResponseBody(BYTE** buffer, DWORD* size) = 0;

	static CWinHttpBase* Create();
};

template<class Class>
class CServerXMLHTTPRequest : public CWinHttpBase
{
protected:
	CComPtr<IServerXMLHTTPRequest> m_xml;

public:
	CServerXMLHTTPRequest(LPCTSTR errorOutput)
	{
		HRESULT hr = m_xml.CoCreateInstance(__uuidof(Class));
		if (FAILED(hr))
			WriteError(errorOutput, hr);
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
		long state;
		m_xml->get_readyState(&state);
		return state == 4;
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

	HRESULT GetResponseBody(BYTE** buffer, DWORD* size)
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
				*buffer = new BYTE[*size];
				memcpy(*buffer, p, *size);
				SafeArrayUnaccessData(body.parray);
			}
		}

		return res;
	}
};

class CWinHttpRequest : public CWinHttpBase
{
protected:
	// 传递给COM组件的回调类，用来监听回应结束事件
	class CWinHttpRequestEvents : public IWinHttpRequestEvents
	{
		// 实现IUnknown
	protected:
		DWORD m_dwRefCount;

	public:
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **ppvObject)
		{
			if (iid == __uuidof(IWinHttpRequestEvents) || iid == IID_IUnknown)
			{
				m_dwRefCount++;
				*ppvObject = this;
				return S_OK;
			}
			return E_NOINTERFACE;
		}

		ULONG STDMETHODCALLTYPE AddRef()
		{
			return m_dwRefCount++;
		}

		ULONG STDMETHODCALLTYPE Release()
		{
			ULONG count = m_dwRefCount--;
			if (m_dwRefCount == 0)
				delete this;
			return count;
		}

		// 实现IWinHttpRequestEvents
	protected:
		CWinHttpRequest* m_request;

	public:
		CWinHttpRequestEvents(CWinHttpRequest* request)
		{
			m_dwRefCount = 0;
			m_request = request;
		}

		void __stdcall OnResponseStart(long Status, BSTR ContentType) {};
		void __stdcall OnResponseDataAvailable(SAFEARRAY** Data) {};
		void __stdcall OnError(long ErrorNumber, BSTR ErrorDescription) {};

		void __stdcall OnResponseFinished()
		{
			m_request->m_completed = TRUE;
		}
	};

protected:
	CComPtr<IWinHttpRequest> m_request;
	DWORD m_dwAdvise;

	BOOL m_completed;

public:
	CWinHttpRequest()
	{
		m_completed = FALSE;

		HRESULT hr = m_request.CoCreateInstance(__uuidof(WinHttpRequest));
		if (FAILED(hr))
			WriteError(_T("CoCreateInstance(WinHttpRequest)"), hr);

		// 注册回调事件
		CComPtr<IConnectionPointContainer> connectionPointContainer;
		hr = m_request.QueryInterface(&connectionPointContainer);
		if (FAILED(hr))
			WriteError(_T("QueryInterface(IConnectionPointContainer)"), hr);

		CComPtr<IConnectionPoint> connectionPoint;
		connectionPointContainer->FindConnectionPoint(__uuidof(IWinHttpRequestEvents), &connectionPoint);
		connectionPoint->Advise(new CWinHttpRequestEvents(this), &m_dwAdvise);
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

	HRESULT GetResponseBody(BYTE** buffer, DWORD* size)
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
				*buffer = new BYTE[*size];
				memcpy(*buffer, p, *size);
				SafeArrayUnaccessData(body.parray);
			}
		}

		return res;
	}
};

CWinHttpBase* CWinHttpBase::Create()
{
	CWinHttpBase* res;

	res = new CServerXMLHTTPRequest<ServerXMLHTTP60>(_T("CoCreateInstance(ServerXMLHTTP60)"));
	if (!res->IsEmpty())
		return res;
	delete res;

	res = new CServerXMLHTTPRequest<ServerXMLHTTP40>(_T("CoCreateInstance(ServerXMLHTTP40)"));
	if (!res->IsEmpty())
		return res;
	delete res;

	res = new CServerXMLHTTPRequest<ServerXMLHTTP30>(_T("CoCreateInstance(ServerXMLHTTP30)"));
	if (!res->IsEmpty())
		return res;
	delete res;

	res = new CServerXMLHTTPRequest<ServerXMLHTTP>(_T("CoCreateInstance(ServerXMLHTTP)"));
	if (!res->IsEmpty())
		return res;
	delete res;

	return new CWinHttpRequest();
}


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
static HTTPRequestResult HTTPRequestBase(BOOL postMethod, CWinHttpBase& xml,
	LPCTSTR URL, LPCTSTR data, BOOL useCookie, volatile BOOL* stopFlag, CString* cookie)
{
	if (xml.IsEmpty())
		return NET_FAILED_TO_CREATE_INSTANCE;
	if (cookie == NULL)
		cookie = &g_cookie;

	if (postMethod)
	{
		xml.Open(_T("POST"), URL, true);
		xml.SetRequestHeader(_T("Content-Type"), _T("application/x-www-form-urlencoded"));
	}
	else
		xml.Open(_T("GET"), URL, true);
	if (useCookie)
		xml.SetRequestHeader(_T("Cookie"), *cookie);
	xml.Send(data);

	// 等待
	DWORD startTime = GetTickCount();
	while (!xml.IsCompleted())
	{
		Delay(1);
		if (stopFlag != NULL && *stopFlag)
		{
			xml.Abort();
			return NET_STOP;
		}
		if (GetTickCount() - startTime > 10000)
		{
			xml.Abort();
			return NET_TIMEOUT;
		}
	}

	// 接收Cookie
	if (useCookie)
	{
		CString headers;
		xml.GetAllResponseHeaders(headers);
		ReceiveCookie((LPCTSTR)headers, *cookie);
	}

	// 重定向
	long status;
	xml.GetStatus(&status);
	if (status == 302)
	{
		CString location;
		xml.GetResponseLocation(location);
		return HTTPRequestBase(postMethod, xml, URL, data, useCookie, stopFlag, cookie);
	}

	return NET_SUCCESS;
}

// HTTP GET请求
CString HTTPGet(LPCTSTR URL, BOOL useCookie, volatile BOOL* stopFlag, CString* cookie)
{
	CWinHttpBase* xml = CWinHttpBase::Create();
	HTTPRequestResult ret = HTTPRequestBase(FALSE, *xml, URL, NULL, useCookie, stopFlag, cookie);
	if (ret != NET_SUCCESS)
	{
		CString result;
		switch (ret)
		{
		case NET_FAILED_TO_CREATE_INSTANCE:
			result = NET_FAILED_TO_CREATE_INSTANCE_TEXT;
			break;
		case NET_STOP:
			result = NET_STOP_TEXT;
			break;
		case NET_TIMEOUT:
			result = NET_TIMEOUT_TEXT;
			break;
		}
		delete xml;
		return result;
	}

	CString result;
	xml->GetResponseText(result);
	delete xml;
	return result;
}

// HTTP POST请求
CString HTTPPost(LPCTSTR URL, LPCTSTR data, BOOL useCookie, volatile BOOL* stopFlag, CString* cookie)
{
	CWinHttpBase* xml = CWinHttpBase::Create();
	HTTPRequestResult ret = HTTPRequestBase(TRUE, *xml, URL, data, useCookie, stopFlag, cookie);
	if (ret != NET_SUCCESS)
	{
		CString result;
		switch (ret)
		{
		case NET_FAILED_TO_CREATE_INSTANCE:
			result = NET_FAILED_TO_CREATE_INSTANCE_TEXT;
			break;
		case NET_STOP:
			result = NET_STOP_TEXT;
			break;
		case NET_TIMEOUT:
			result = NET_TIMEOUT_TEXT;
			break;
		}
		delete xml;
		return result;
	}

	CString result;
	xml->GetResponseText(result);
	delete xml;
	return result;
}

// HTTP GET请求，取得原始数据，注意自行delete buffer!!!
HTTPRequestResult HTTPGetRaw(LPCTSTR URL, BYTE** buffer, ULONG* size, BOOL useCookie, volatile BOOL* stopFlag, CString* cookie)
{
	CWinHttpBase* xml = CWinHttpBase::Create();
	HTTPRequestResult ret = HTTPRequestBase(FALSE, *xml, URL, NULL, useCookie, stopFlag, cookie);
	if (ret != NET_SUCCESS)
	{
		delete xml;
		return ret;
	}

	// 返回
	xml->GetResponseBody(buffer, size);
	delete xml;
	return NET_SUCCESS;
}
