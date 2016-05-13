#pragma once
#include "HelperCommon.h"
#include <regex>


struct RegexText
{
	BOOL isRegex;
	CString text; // 为了避免忘记设置regexp要用Set方法赋值！！
	std::wregex regexp;

	void Set(BOOL _isRegex, const CString& _text)
	{
		isRegex = _isRegex;
		text = _text;
		regexp = isRegex ? text : _T("");
	}
};


// 分割字符串
HELPER_API void SplitString(CStringArray& dst, const CString& src, const CString& slipt);

// 字符串包含
HELPER_API BOOL StringIncludes(const CString& str, const CString& content);
// 字符串包含
HELPER_API BOOL StringIncludes(const CString& str, const CString& content, BOOL isRegex);
// 字符串包含
HELPER_API BOOL StringIncludes(const CString& str, const RegexText& content, int* _pos = NULL, int* length = NULL);
// 字符串匹配
HELPER_API BOOL StringMatchs(const CString& str, const CString& content, BOOL isRegex);
// 字符串匹配
HELPER_API BOOL StringMatchs(const CString& str, const RegexText& content);

// 取字符串之间的字符串
HELPER_API CString GetStringBetween(const CString& src, const CString& left, const CString& right, int startPos = 0);
// 取字符串之间的字符串，包括左右的字符串
HELPER_API CString GetStringBetween2(const CString& src, const CString& left, const CString& right, int startPos = 0);
// 取字符串之前的字符串
HELPER_API CString GetStringBefore(const CString& src, const CString& right, int startPos = 0);

// 写字符串到文件
HELPER_API BOOL WriteString(const CString& src, const CString& path);

// Unicode转GBK
HELPER_API CStringA W2GBK(const CStringW& src);
// GBK转Unicode
HELPER_API CStringW GBK2W(const CStringA& src);
// URL编码
HELPER_API CString EncodeURI(const CString& src);
// URL编码 GBK版
HELPER_API CString EncodeURI_GBK(const CString& src);
// HTML转义
HELPER_API CString HTMLEscape(const CString& src);
// HTML反转义
HELPER_API CString HTMLUnescape(const CString& src);
// JS反转义，调用者应自行转义src里的双引号
HELPER_API CString JSUnescape(const CString& src);
