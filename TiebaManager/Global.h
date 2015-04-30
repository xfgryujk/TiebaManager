#pragma once
#include <regex>
using std::wregex;
#include "Setting.h"


// ×Ö·û´®²Ù×÷ ///////////////////////////////////////////////////////////////////////////
void SplitString(CStringArray& dst, const CString& src, LPCTSTR slipt);
// ×Ö·û´®°üº¬
inline BOOL StringIncludes(const CString& str, LPCTSTR content){ return str.Find(content) != -1; }
inline BOOL StringIncludes(const CString& str, LPCTSTR content, BOOL isRegex)
{
	if (isRegex)
	{
		wregex reg(content);
		return std::regex_search((LPCTSTR)str, reg);
	}
	else
		return StringIncludes(str, content);
}
inline BOOL StringIncludes(const CString& str, const RegexText& content)
{
	if (content.isRegex)
		return std::regex_search((LPCTSTR)str, content.regexp);
	else
		return StringIncludes(str, content.text);
}
// ×Ö·û´®Æ¥Åä
inline BOOL StringMatchs(const CString& str, LPCTSTR content, BOOL isRegex)
{
	if (isRegex)
	{
		wregex reg(content);
		return std::regex_match((LPCTSTR)str, reg);
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
CString GetStringBefore(const CString& src, LPCTSTR right, int startPos = 0);
BOOL WriteString(const CString& src, LPCTSTR path);
CString EncodeURI(const CString& src);
//CString EncodeURI_GBK(const CString& src);
CString HTMLEscape(const CString& src);
CString HTMLUnescape(const CString& src);
CString JSUnescape(const CString& src);

// ÍøÂç²Ù×÷ /////////////////////////////////////////////////////////////////////////////
// ÇëÇó
enum HTTPRequestResult { NET_SUCCESS, NET_FAILED_TO_CREATE_INSTANCE, NET_STOP, NET_TIMEOUT, };
const TCHAR NET_FAILED_TO_CREATE_INSTANCE_TEXT[] = _T("failed to create instance");
const TCHAR NET_STOP_TEXT[] = _T("stopped");
const TCHAR NET_TIMEOUT_TEXT[] = _T("timeout");
CString HTTPGet(LPCTSTR URL, BOOL useCookie = TRUE, volatile BOOL* stopFlag = NULL);
CString HTTPPost(LPCTSTR URL, LPCTSTR data, BOOL useCookie = TRUE, volatile BOOL* stopFlag = NULL);
HTTPRequestResult HTTPGetRaw(LPCTSTR URL, BYTE** buffer = NULL, ULONG* size = NULL, BOOL useCookie = TRUE, volatile BOOL* stopFlag = NULL);
// ¸üÐÂ
const TCHAR UPDATE_CURRENT_VERSION[] = _T("15-05-01");
enum CheckUpdateResult { UPDATE_NO_UPDATE, UPDATE_HAS_UPDATE, UPDATE_FAILED_TO_GET_FILE_ID, UPDATE_FAILED_TO_GET_LINK };
CheckUpdateResult CheckUpdate();

// ÔÓÏî /////////////////////////////////////////////////////////////////////////////////
void Delay(DWORD time);
void DoEvents();
LONG WINAPI ExceptionHandler(_EXCEPTION_POINTERS* ExceptionInfo);
