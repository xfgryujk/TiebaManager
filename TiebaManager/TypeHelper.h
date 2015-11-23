#pragma once
#include <opencv2\core\mat.hpp>


struct RegexText
{
	BOOL isRegex;
	CString text;
	wregex regexp;
};

struct NameImage
{
	CString name;
	cv::Mat img;
};
