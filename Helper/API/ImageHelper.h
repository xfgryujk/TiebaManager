#pragma once
#include "HelperCommon.h"
#include <opencv2\core\mat.hpp>
using cv::Mat;
using std::min;
using std::max;
#include <atlimage.h>


HELPER_API BOOL ReadImage(const CString& path, Mat& img);
HELPER_API BOOL ReadImage(const BYTE* buffer, ULONG size, CImage& img);
HELPER_API BOOL ReadImage(const BYTE* buffer, ULONG size, Mat& img);
HELPER_API CString GetImageName(const CString& imgUrl);
