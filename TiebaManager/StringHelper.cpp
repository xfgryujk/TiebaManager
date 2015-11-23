#include "stdafx.h"
#include "StringHelper.h"
#import "msscript.ocx" no_namespace


// 分割字符串
void SplitString(CStringArray& dst, const CString& src, LPCTSTR slipt)
{
	dst.RemoveAll();
	const int len = _tcslen(slipt);

	int start = 0, end = 0;
	while ((end = src.Find(slipt, end)) != -1)
	{
		dst.Add(src.Mid(start, end - start));
		start = end += len;
	}
	dst.Add(src.Right(src.GetLength() - start));
}

// 取字符串之间的字符串
CString GetStringBetween(const CString& src, const CString& left, LPCTSTR right, int startPos)
{
	int leftPos = src.Find(left, startPos);
	if (leftPos == -1)
		return _T("");
	leftPos += left.GetLength();
	int rightPos = src.Find(right, leftPos);
	if (rightPos == -1)
		return _T("");
	return src.Mid(leftPos, rightPos - leftPos);
}

// 取字符串之间的字符串，包括左右的字符串
CString GetStringBetween2(const CString& src, const CString& left, const CString& right, int startPos)
{
	int leftPos = src.Find(left, startPos);
	if (leftPos == -1)
		return _T("");
	int rightPos = src.Find(right, leftPos + left.GetLength());
	if (rightPos == -1)
		return _T("");
	rightPos += right.GetLength();
	return src.Mid(leftPos, rightPos - leftPos);
}

// 取字符串之前的字符串
CString GetStringBefore(const CString& src, LPCTSTR right, int startPos)
{
	int rightPos = src.Find(right, startPos);
	if (rightPos == -1)
		return _T("");
	return src.Left(rightPos);
}

// 写字符串到文件
BOOL WriteString(const CString& src, LPCTSTR path)
{
	CFile file;
	if (!file.Open(path, CFile::modeCreate | CFile::modeWrite))
		return FALSE;
#ifdef _UNICODE
	static const BYTE UNICODE_HEADER[] = { 0xFF, 0xFE };
	file.Write(UNICODE_HEADER, sizeof(UNICODE_HEADER));
#endif
	file.Write((LPCTSTR)src, src.GetLength() * sizeof(TCHAR));
	return TRUE;
}

// URL编码
CString EncodeURI(const CString& src)
{
	CComPtr<IScriptControl> script;
	if (FAILED(script.CoCreateInstance(__uuidof(ScriptControl))))
		return _T("");
	script->PutLanguage("JScript");
	_variant_t param = src;
	SAFEARRAY* params = SafeArrayCreateVector(VT_VARIANT, 0, 1);
	LONG index = 0;
	SafeArrayPutElement(params, &index, &param);
	_variant_t result = script->Run(_bstr_t(_T("encodeURIComponent")), &params);
	SafeArrayDestroy(params);
	return (LPCTSTR)(_bstr_t)result;
}

// URL编码 GBK版
/*CString EncodeURI_GBK(const CString& _src)
{
CString result, tmp;
CStringA src(_src); // 有些电脑会转码失败？
const int len = src.GetLength();
for (int i = 0; i < len; i++)
{
tmp.Format(_T("%%%02X"), src[i] & 0xFF);
result += tmp;
}
return result;
}*/

// HTML转义
CString HTMLEscape(const CString& src)
{
	CString result = src;
	result.Replace(_T("&"), _T("&amp;"));
	result.Replace(_T(" "), _T("&nbsp;"));
	result.Replace(_T("<"), _T("&lt;"));
	result.Replace(_T(">"), _T("&gt;"));
	return result;
}

// HTML反转义
CString HTMLUnescape(const CString& src)
{
	CString result = src;
	//result.Replace(_T("<br>"), _T("\r\n")); // 不转换行符
	result.Replace(_T("&nbsp;"), _T(" "));
	result.Replace(_T("&quot;"), _T("\""));
	result.Replace(_T("&&#039;"), _T("'"));
	result.Replace(_T("&lt;"), _T("<"));
	result.Replace(_T("&gt;"), _T(">"));
	result.Replace(_T("&amp;"), _T("&"));
	return result;
}

// JS反转义，自行转义src里的双引号
CString JSUnescape(const CString& src)
{
	try
	{
		CComPtr<IScriptControl> script;
		if (FAILED(script.CoCreateInstance(__uuidof(ScriptControl))))
			return _T("");
		script->PutLanguage(_bstr_t(_T("JScript")));
		_variant_t result;
		result = script->Eval((LPCTSTR)(_T("\"") + src + _T("\"")));
		return (LPCTSTR)(_bstr_t)result;
	}
	catch (_com_error& e)
	{
		CString info = _T("Description: ");
		info += (LPCTSTR)e.Description();
		info += _T("\r\nSource: ");
		info += (LPCTSTR)e.Source();
		info += _T("\r\nSource: ");
		info += e.ErrorMessage();
		WriteString(info, _T("error.txt"));
		return _T("");
	}
}
