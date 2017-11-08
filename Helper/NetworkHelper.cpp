/*
Copyright (C) 2011-2017  xfgryujk
https://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

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
#include <StringHelper.h>
#include <MiscHelper.h>
#include <curl\curl.h>


// 从HTTP头提取Cookie并修改cookie
static void ReceiveCookie(const CString& headers, CString& cookie)
{
	static const std::wregex cookieExp(_T("Set-Cookie: (.*?)=(.*?);"));
	for (std::regex_iterator<LPCTSTR> it(headers, (LPCTSTR)headers + _tcslen(headers), cookieExp), end; it != end; ++it)
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

static size_t WriteData(void *buffer, size_t size, size_t nmemb, void *userp)
{
	size_t len = size * nmemb;
	((CMemFile*)userp)->Write(buffer, len);
	return len;
}

// HTTP请求
static HTTPRequestResult HTTPRequestBase(std::unique_ptr<BYTE[]>* buffer, ULONG* size, BOOL postMethod,
	const CString& URL, const CString& data, CString* cookie, CString* charset = NULL)
{
	std::unique_ptr<CURL, void(*)(CURL*)> easyHandle(curl_easy_init(), [](CURL* p){ curl_easy_cleanup(p); });
	std::unique_ptr<CURLM, std::function<void(CURLM*)> > multiHandle(curl_multi_init(), [&easyHandle](CURLM* p){
		curl_multi_remove_handle(p, easyHandle.get()); curl_multi_cleanup(p); });
	if (easyHandle == nullptr || multiHandle == nullptr)
		return NET_FAILED_TO_CREATE_INSTANCE;

	// 设置
	curl_easy_setopt(easyHandle.get(), CURLOPT_URL, (LPCSTR)CStringA(EncodeFullURI(URL)));
	curl_easy_setopt(easyHandle.get(), CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(easyHandle.get(), CURLOPT_SSL_VERIFYHOST, 0L);
	CMemFile headerBuf, bodyBuf;
	curl_easy_setopt(easyHandle.get(), CURLOPT_HEADERFUNCTION, WriteData);
	curl_easy_setopt(easyHandle.get(), CURLOPT_HEADERDATA, &headerBuf);
	curl_easy_setopt(easyHandle.get(), CURLOPT_WRITEFUNCTION, WriteData);
	curl_easy_setopt(easyHandle.get(), CURLOPT_WRITEDATA, &bodyBuf);
	curl_slist* chunk = NULL;
	chunk = curl_slist_append(chunk, "Connection: Keep-Alive");
	chunk = curl_slist_append(chunk, "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.99 Safari/537.36");
	chunk = curl_slist_append(chunk, "Accept: */*");
	curl_easy_setopt(easyHandle.get(), CURLOPT_ACCEPT_ENCODING, "gzip, deflate");
	chunk = curl_slist_append(chunk, "Accept-Language: zh-CN,zh;q=0.8");
	if (postMethod)
	{
		curl_easy_setopt(easyHandle.get(), CURLOPT_POST, 1L);
		CStringA dataA(EncodeFullURI(data));
		curl_easy_setopt(easyHandle.get(), CURLOPT_POSTFIELDSIZE, (long)dataA.GetLength());
		curl_easy_setopt(easyHandle.get(), CURLOPT_COPYPOSTFIELDS, (LPCSTR)dataA);
	}
	if (cookie != NULL)
		curl_easy_setopt(easyHandle.get(), CURLOPT_COOKIE, (LPCSTR)CStringA(*cookie));
	curl_easy_setopt(easyHandle.get(), CURLOPT_HTTPHEADER, chunk);
	std::unique_ptr<curl_slist, void(*)(curl_slist*)> pChunk(chunk, [](curl_slist* p){ curl_slist_free_all(p); });

	// 发送
	curl_multi_add_handle(multiHandle.get(), easyHandle.get());
#pragma warning(suppress: 28159)
	DWORD startTime = GetTickCount();
	while (true)
	{
		int runningCount;
		curl_multi_perform(multiHandle.get(), &runningCount);
		if (runningCount <= 0)
			break;

#pragma warning(suppress: 28159)
		if (GetTickCount() - startTime > 10000)
			return NET_TIMEOUT;
		Delay(1);
	}

	// 接收Cookie
	if (cookie != NULL)
	{
		CStringA headers;
		ULONG headersSize = (ULONG)headerBuf.GetLength();
		headerBuf.SeekToBegin();
		headerBuf.Read(headers.GetBuffer(headersSize), headersSize);
		headers.ReleaseBuffer(headersSize);
		ReceiveCookie(CString(headers), *cookie);
		//TRACE(_T("%s\n%s\n"), (LPCTSTR)URL, (LPCTSTR)*cookie);
	}

	// 重定向
	long status = 200;
	curl_easy_getinfo(easyHandle.get(), CURLINFO_RESPONSE_CODE, &status);
	if (status == 302)
	{
		char* location = NULL;
		curl_easy_getinfo(easyHandle.get(), CURLINFO_REDIRECT_URL, &location);
		return HTTPRequestBase(buffer, size, postMethod, CString(location), data, cookie, charset);
	}

	// 返回数据
	if (buffer != NULL && size != NULL)
	{
		*size = (ULONG)bodyBuf.GetLength();
		buffer->reset(new BYTE[*size]);
		bodyBuf.SeekToBegin();
		bodyBuf.Read(buffer->get(), *size);
	}

	// 返回编码
	if (charset != NULL)
	{
		char* pContentType = NULL;
		curl_easy_getinfo(easyHandle.get(), CURLINFO_CONTENT_TYPE, &pContentType);
		if (pContentType != NULL)
		{
			char* pLeft = strstr(pContentType, "charset=");
			if (pLeft == NULL)
				*charset = _T("");
			else
			{
				pLeft += 8;
				char* pRight = pLeft;
				while (*pRight != '\0' && *pRight != ';')
					pRight++;
				*charset = CString(pLeft, pRight - pLeft).MakeLower();
			}
		}
	}

	return NET_SUCCESS;
}

// HTTP请求，把响应转码
static CString HTTPRequestBase_Convert(BOOL postMethod, const CString& URL, const CString& data, CString* cookie)
{
	std::unique_ptr<BYTE[]> buffer;
	ULONG size = 0;
	CString charset;
	HTTPRequestResult ret = HTTPRequestBase(&buffer, &size, postMethod, URL, data, cookie, &charset);
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

	// 转码
	CStringA result((LPCSTR)buffer.get(), size);
	if (charset == _T("utf-8") || charset == _T("utf8"))
		return UTF82W(result);
	else if (charset == _T("gbk") || charset == _T("gb2312"))
		return GBK2W(result);
	else
		return ANSI2W(result, 1252); // 默认ISO-8859-1
}

// HTTP GET请求
HELPER_API CString HTTPGet(const CString& URL, CString* cookie)
{
	return HTTPRequestBase_Convert(FALSE, URL, NULL, cookie);
}

// HTTP POST请求
HELPER_API CString HTTPPost(const CString& URL, const CString& data, CString* cookie)
{
	return HTTPRequestBase_Convert(TRUE, URL, data, cookie);
}

// HTTP GET请求，取得原始数据
HELPER_API HTTPRequestResult HTTPGetRaw(const CString& URL, std::unique_ptr<BYTE[]>* buffer, ULONG* size, CString* cookie)
{
	return HTTPRequestBase(buffer, size, FALSE, URL, NULL, cookie);
}
