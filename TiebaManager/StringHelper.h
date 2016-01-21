#pragma once
#include "TypeHelper.h"


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
			AfxMessageBox(_T("正则表达式错误！"), MB_ICONERROR);
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
			AfxMessageBox(_T("正则表达式错误！"), MB_ICONERROR);
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
