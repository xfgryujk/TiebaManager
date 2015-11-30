#include "stdafx.h"
#include "NetworkHelper.h"
#include <regex>
using std::wregex;
using std::regex_iterator;
#include "TiebaVariable.h" // extern CString g_cookie;
#include "StringHelper.h"
#include "MiscHelper.h"
#include <msxml2.h>
#import <winhttp.dll> no_namespace
//#include <winhttp.h>


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

class CServerXMLHTTPRequest : public CWinHttpBase
{
protected:
	CComPtr<IServerXMLHTTPRequest> m_xml;

public:
	CServerXMLHTTPRequest()
	{
		HRESULT hr = m_xml.CoCreateInstance(__uuidof(ServerXMLHTTP));
		if (FAILED(hr))
			WriteError(_T("CoCreateInstance(ServerXMLHTTP)"), hr);
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
			WriteError(_T("CoCreateInstance(IWinHttpRequest)"), hr);

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

// 接收数据没实现，心好累
//class CWinHttpRequestAPI : public CWinHttpBase
//{
//protected:
//	CCriticalSection m_lock;
//
//	HINTERNET m_session;
//	HINTERNET m_connection;
//	HINTERNET m_request;
//
//	long m_totalSize;
//	BYTE* m_buffer;
//	BYTE* m_pCurPos;
//
//	BOOL m_completed;
//
//	static VOID CALLBACK Callback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
//	{
//		CWinHttpRequestAPI* thiz = (CWinHttpRequestAPI*)dwContext;
//		TRACE(_T("0x%08X\n"), dwInternetStatus);
//		switch (dwInternetStatus)
//		{
//		case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
//			if (!WinHttpReceiveResponse(hInternet, NULL))
//				thiz->WriteError(_T("WinHttpReceiveResponse"));
//			break;
//
//		case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
//		{
//			CString sSize;
//			thiz->GetAllResponseHeaders(sSize);
//			AfxMessageBox(sSize); // chunked
//			long size = 0;
//			thiz->GetLongResponseHeader(WINHTTP_QUERY_CONTENT_LENGTH, &size);
//			thiz->m_totalSize = size;
//			if (size <= 0)
//				break;
//			thiz->m_lock.Lock();
//			thiz->m_buffer = thiz->m_pCurPos = new BYTE[size];
//			thiz->m_lock.Unlock();
//
//			DWORD size2 = 0;
//			if (!WinHttpQueryDataAvailable(hInternet, &size2))
//			{
//				thiz->WriteError(_T("WinHttpQueryDataAvailable"));
//				break;
//			}
//		}
//			break;
//
//		case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
//		{
//			DWORD size = *(DWORD*)lpvStatusInformation;
//			/*DWORD size = 0;
//			if (!WinHttpQueryDataAvailable(hInternet, &size))
//			{
//				thiz->WriteError(_T("WinHttpQueryDataAvailable"));
//				break;
//			}*/
//			TRACE(_T("%u\n"), size);
//
//			thiz->m_lock.Lock();
//			if (!WinHttpReadData(hInternet, thiz->m_pCurPos, size, NULL))
//			{
//				thiz->m_lock.Unlock();
//				thiz->WriteError(_T("WinHttpReadData"));
//				break;
//			}
//			thiz->m_pCurPos += size;
//			thiz->m_lock.Unlock();
//
//			// 判断传输完成
//			if (thiz->m_pCurPos >= thiz->m_buffer + thiz->m_totalSize)
//			{
//				thiz->m_completed = TRUE;
//				WinHttpSetStatusCallback(hInternet, NULL, WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS, NULL);
//			}
//		}
//			break;
//		}
//	}
//
//public:
//	CWinHttpRequestAPI()
//	{
//		m_session = WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC);
//		if (m_session == NULL)
//			WriteError(_T("WinHttpOpen"));
//		m_connection = NULL;
//		m_request = NULL;
//		m_totalSize = 0;
//		m_pCurPos = m_buffer = NULL;
//		m_completed = FALSE;
//	}
//
//	~CWinHttpRequestAPI()
//	{
//		m_lock.Lock();
//		if (m_request != NULL)
//			WinHttpCloseHandle(m_request);
//		if (m_connection != NULL)
//			WinHttpCloseHandle(m_connection);
//		if (m_session != NULL)
//			WinHttpCloseHandle(m_session);
//		m_lock.Unlock();
//	}
//
//	BOOL IsEmpty()
//	{
//		return m_session == NULL;
//	}
//
//	HRESULT Open(LPCTSTR method, const CString& uri, BOOL async)
//	{
//		// 解析域名
//		int left = uri.Find(_T("//"));
//		if (left == -1)
//			left = 0;
//		else
//			left += 2;
//		int right = uri.Find(_T("/"), left);
//		if (right == -1)
//			right = uri.GetLength();
//		CString host = uri.Mid(left, right - left);
//		// 连接
//		m_connection = WinHttpConnect(m_session, host, INTERNET_DEFAULT_PORT, 0);
//		if (m_connection == NULL)
//		{
//			WriteError(_T("WinHttpConnect"));
//			return HRESULT_FROM_WIN32(m_lastError);
//		}
//
//		// 打开请求
//		CString objectName = uri.Mid(right);
//		if (objectName == _T(""))
//			objectName = _T("/");
//		m_request = WinHttpOpenRequest(m_connection, method, objectName, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
//		if (m_request == NULL)
//		{
//			WriteError(_T("WinHttpOpenRequest"));
//			return HRESULT_FROM_WIN32(m_lastError);
//		}
//
//		// 设置头部
//		static const TCHAR HEADERS[] = _T("Accept: */*\r\n")
//									   _T("Accept-Language: en-US,en;q=0.8,zh-Hans-CN;q=0.5,zh-Hans;q=0.3\r\n")
//									   _T("User-Agent: Mozilla/4.0 (compatible; Win32; WinHttp.WinHttpRequest.5)");
//		if (!WinHttpAddRequestHeaders(m_request, HEADERS, -1, WINHTTP_ADDREQ_FLAG_REPLACE | WINHTTP_ADDREQ_FLAG_ADD))
//		{
//			WriteError(_T("WinHttpAddRequestHeaders"));
//			return HRESULT_FROM_WIN32(m_lastError);
//		}
//
//		return S_OK;
//	}
//
//	HRESULT SetRequestHeader(LPCTSTR header, LPCTSTR value)
//	{
//		CString headers = header;
//		headers += _T(": ");
//		headers += value;
//		BOOL res = WinHttpAddRequestHeaders(m_request, headers, -1, WINHTTP_ADDREQ_FLAG_REPLACE | WINHTTP_ADDREQ_FLAG_ADD);
//		if (!res)
//		{
//			WriteError(_T("WinHttpAddRequestHeaders"));
//			return HRESULT_FROM_WIN32(m_lastError);
//		}
//		return S_OK;
//	}
//
//	HRESULT Send(const CString& data)
//	{
//		BOOL res;
//		// 设置回调
//		res = WinHttpSetStatusCallback(m_request, Callback, WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS, NULL) != WINHTTP_INVALID_STATUS_CALLBACK;
//		if (!res)
//		{
//			WriteError(_T("WinHttpSetStatusCallback"));
//			return HRESULT_FROM_WIN32(m_lastError);
//		}
//
//		// 发送
//		if (data != _T(""))
//			res = WinHttpSendRequest(m_request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, (LPVOID)(LPCTSTR)data, 
//				data.GetLength() * sizeof(TCHAR), data.GetLength() * sizeof(TCHAR), (DWORD_PTR)this);
//		else
//			res = WinHttpSendRequest(m_request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, (DWORD_PTR)this);
//		if (!res)
//		{
//			WriteError(_T("WinHttpSendRequest"));
//			return HRESULT_FROM_WIN32(m_lastError);
//		}
//
//		return S_OK;
//	}
//
//	BOOL IsCompleted()
//	{
//		return m_completed;
//	}
//
//	HRESULT Abort()
//	{
//		return S_OK;
//	}
//
//	HRESULT GetStringResponseHeader(DWORD index, CString& header)
//	{
//		DWORD size = 0;
//		WinHttpQueryHeaders(m_request, index, WINHTTP_HEADER_NAME_BY_INDEX, WINHTTP_NO_OUTPUT_BUFFER, &size, WINHTTP_NO_HEADER_INDEX);
//		if (size == 0)
//		{
//			header = _T("");
//			return E_FAIL;
//		}
//
//		DWORD nChar = size / sizeof(TCHAR)-1;
//		BOOL res = WinHttpQueryHeaders(m_request, index, WINHTTP_HEADER_NAME_BY_INDEX, header.GetBuffer(nChar), &size, WINHTTP_NO_HEADER_INDEX);
//		if (!res)
//		{
//			header.ReleaseBuffer(0);
//			WriteError(_T("WinHttpQueryHeaders"));
//			return HRESULT_FROM_WIN32(m_lastError);
//		}
//		header.ReleaseBuffer(nChar);
//		return S_OK;
//	}
//	
//	HRESULT GetLongResponseHeader(DWORD index, long* header)
//	{
//		DWORD size = sizeof(long);
//		BOOL res = WinHttpQueryHeaders(m_request, index | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX,
//			header, &size, WINHTTP_NO_HEADER_INDEX);
//		if (!res)
//		{
//			WriteError(_T("WinHttpQueryHeaders"));
//			return HRESULT_FROM_WIN32(m_lastError);
//		}
//		return S_OK;
//	}
//
//	HRESULT GetAllResponseHeaders(CString& headers)
//	{
//		return GetStringResponseHeader(WINHTTP_QUERY_RAW_HEADERS_CRLF, headers);
//	}
//
//	HRESULT GetStatus(long* status)
//	{
//		return GetLongResponseHeader(WINHTTP_QUERY_STATUS_CODE, status);
//	}
//
//	HRESULT GetResponseLocation(CString& location)
//	{
//		return GetStringResponseHeader(WINHTTP_QUERY_LOCATION, location);
//	}
//
//	HRESULT GetResponseText(CString& body)
//	{
//		BYTE* buffer;
//		DWORD size;
//		HRESULT res = GetResponseBody(&buffer, &size);
//		if (FAILED(res))
//			return res;
//
//		if (buffer == NULL || size == 0)
//			body = _T("");
//		else
//		{
//			body = (LPCTSTR)buffer;
//			delete buffer;
//		}
//		AfxMessageBox(body);
//		return S_OK;
//	}
//
//	HRESULT GetResponseBody(BYTE** buffer, DWORD* size)
//	{
//		if (buffer == NULL || size == NULL)
//			return E_FAIL;
//
//		*buffer = m_buffer;
//		*size = m_totalSize;
//		return S_OK;
//	}
//};

CWinHttpBase* CWinHttpBase::Create()
{
	CWinHttpBase* res;

	res = new CServerXMLHTTPRequest();
	if (!res->IsEmpty())
		return res;
	delete res;

	res = new CWinHttpRequest();
	//if (!res->IsEmpty())
		return res;
	/*delete res;

	res = new CWinHttpRequestAPI();
	return res;*/
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
		switch (ret)
	{
		case NET_FAILED_TO_CREATE_INSTANCE:
			return NET_FAILED_TO_CREATE_INSTANCE_TEXT;
		case NET_STOP:
			return NET_STOP_TEXT;
		case NET_TIMEOUT:
			return NET_TIMEOUT_TEXT;
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
		switch (ret)
	{
		case NET_FAILED_TO_CREATE_INSTANCE:
			return NET_FAILED_TO_CREATE_INSTANCE_TEXT;
		case NET_STOP:
			return NET_STOP_TEXT;
		case NET_TIMEOUT:
			return NET_TIMEOUT_TEXT;
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
		return ret;

	// 返回
	xml->GetResponseBody(buffer, size);
	delete xml;
	return NET_SUCCESS;
}
