#pragma once
#include "Setting.h"


// 字符串操作 ///////////////////////////////////////////////////////////////////////////
void SplitString(CStringArray& dst, const CString& src, LPCTSTR slipt);
// 字符串包含
inline BOOL StringIncludes(const CString& str, LPCTSTR content){ return str.Find(content) != -1; }
inline BOOL StringIncludes(const CString& str, LPCTSTR content, BOOL isRegex)
{
	if (isRegex)
	{
		try
		{
			wregex reg(content);
			return std::regex_search((LPCTSTR)str, reg);
		}
		catch (...)
		{
			MessageBox(NULL, _T("正则表达式错误！"), _T("贴吧管理器"), MB_ICONERROR);
			return FALSE;
		}
	}
	else
		return StringIncludes(str, content);
}
inline BOOL StringIncludes(const CString& str, const RegexText& content, int* _pos = NULL, int* length = NULL)
{
	BOOL result;
	if (content.isRegex)
	{
		std::wcmatch res;
		result = std::regex_search((LPCTSTR)str, res, content.regexp);
		if (result && _pos != NULL && length != NULL)
		{
			*_pos = res.position();
			*length = res.length();
		}
	}
	else
	{
		int pos = str.Find(content.text);
		result = pos != -1;
		if (result && _pos != NULL && length != NULL)
		{
			*_pos = pos;
			*length = content.text.GetLength();
		}
	}
	return result;
}
// 字符串匹配
inline BOOL StringMatchs(const CString& str, LPCTSTR content, BOOL isRegex)
{
	if (isRegex)
	{
		try
		{
			wregex reg(content);
			return std::regex_match((LPCTSTR)str, reg);
		}
		catch (...)
		{
			MessageBox(NULL, _T("正则表达式错误！"), _T("贴吧管理器"), MB_ICONERROR);
			return FALSE;
		}
	}
	else
		return str == content;
}
inline BOOL StringMatchs(const CString& str, const RegexText& content)
{
	if (content.isRegex)
		return std::regex_match((LPCTSTR)str, content.regexp);
	else
		return str == content.text;
}
CString GetStringBetween(const CString& src, const CString& left, LPCTSTR right, int startPos = 0);
CString GetStringBetween2(const CString& src, const CString& left, const CString& right, int startPos = 0);
CString GetStringBefore(const CString& src, LPCTSTR right, int startPos = 0);
BOOL WriteString(const CString& src, LPCTSTR path);
CString EncodeURI(const CString& src);
//CString EncodeURI_GBK(const CString& src);
CString HTMLEscape(const CString& src);
CString HTMLUnescape(const CString& src);
CString JSUnescape(const CString& src);

// 网络操作 /////////////////////////////////////////////////////////////////////////////
// 请求
enum HTTPRequestResult { NET_SUCCESS, NET_FAILED_TO_CREATE_INSTANCE, NET_STOP, NET_TIMEOUT, };
const TCHAR NET_FAILED_TO_CREATE_INSTANCE_TEXT[] = _T("failed to create instance");
const TCHAR NET_STOP_TEXT[] = _T("stopped");
const TCHAR NET_TIMEOUT_TEXT[] = _T("timeout");
CString HTTPGet(LPCTSTR URL, BOOL useCookie = TRUE, volatile BOOL* stopFlag = NULL, CString* cookie = NULL);
CString HTTPPost(LPCTSTR URL, LPCTSTR data, BOOL useCookie = TRUE, volatile BOOL* stopFlag = NULL, CString* cookie = NULL);
HTTPRequestResult HTTPGetRaw(LPCTSTR URL, BYTE** buffer = NULL, ULONG* size = NULL, BOOL useCookie = TRUE, volatile BOOL* stopFlag = NULL, CString* cookie = NULL);

// 杂项 /////////////////////////////////////////////////////////////////////////////////
void Delay(DWORD time);
void DoEvents();
LONG WINAPI ExceptionHandler(_EXCEPTION_POINTERS* ExceptionInfo);
