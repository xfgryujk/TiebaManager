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
