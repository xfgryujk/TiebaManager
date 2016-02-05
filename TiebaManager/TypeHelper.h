#pragma once
#include <opencv2\core\mat.hpp>


struct RegexText
{
	BOOL isRegex;
	CString text; // 为了避免忘记设置regexp要用Set方法赋值！！
	wregex regexp;

	void Set(BOOL _isRegex, const CString& _text)
	{
		isRegex = _isRegex;
		text = _text;
		regexp = isRegex ? text : _T("");
	}
};

struct NameImage
{
	CString name;
	cv::Mat img;
};
