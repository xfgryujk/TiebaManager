#include "stdafx.h"
#include "ScanImage.h"
#include "Setting.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\imgcodecs.hpp>
#include <regex>
using std::wregex;


// 1是图片地址
const wregex THREAD_IMG_REG(_T("<img.*b?pic=\"(.*?)\".*?/>"));
// 1是图片地址
const wregex POST_IMG_REG(_T("<img class=\"BDE_Image\".*?src=\"(.*?)\".*?>"));


void ReadFeature(const gzFile& f, ImageFeature& feature)
{
	ReadText(f, feature.name);
	feature.feature.create(HUE_BIN, SATURATION_BIN, CV_32F);
	for (int i = 0; i < feature.feature.rows; i++)
		gzread(f, feature.feature.ptr<float>(i), feature.feature.cols * sizeof(float));
}

void ReadFeatures(const TCHAR* path, vector<ImageFeature>& features)
{
	gzFile f = gzopen_w(path, "rb");
	if (f == NULL)
	{
		features.clear();
		return;
	}
	int intBuf;
	gzread(f, &intBuf, sizeof(int)); // 长度
	features.resize(intBuf);
	for (ImageFeature& i : features)
		ReadFeature(f, i);
	gzclose(f);
}

void WriteFeature(const gzFile& f, const ImageFeature& feature)
{
	WriteText(f, feature.name);
	for (int i = 0; i < feature.feature.rows; i++)
		gzwrite(f, feature.feature.ptr<float>(i), feature.feature.cols * sizeof(float));
}

void WriteFeatures(const TCHAR* path, const vector<ImageFeature>& features)
{
	gzFile f = gzopen_w(path, "wb");
	if (f == NULL)
		return;
	int intBuf;
	gzwrite(f, &(intBuf = features.size()), sizeof(int));
	for (const ImageFeature& i : features)
		WriteFeature(f, i);
	gzclose(f);
}


// 从图片缓冲计算特征值
void CalculateFeature(const BYTE* imgBuffer, ULONG imgSize, Mat& feature)
{
	vector<BYTE> _imgBuffer(imgBuffer, imgBuffer + imgSize);
	Mat img = cv::imdecode(Mat(_imgBuffer), cv::IMREAD_COLOR); // 不支持gif格式
	if (img.data == NULL)
		return;
	CalculateFeature(img, feature);
}

// 从图片路径计算特征值
void CalculateFeature(const CString& imgPath, Mat& feature)
{
	Mat img = cv::imread((LPCSTR)(CStringA)imgPath);
	if (img.data == NULL)
		return;
	CalculateFeature(img, feature);
}

// 从BGR Mat计算特征值
void CalculateFeature(const Mat& img, Mat& feature)
{
	Mat hsvImg;
	cvtColor(img, hsvImg, CV_BGR2HSV);

	static const int channels[] = { 0, 1 };
	static const int histSize[] = { HUE_BIN, SATURATION_BIN };
	static const float range[][2] = { { 0, 180 }, { 0, 256 } };
	static const float* pRange[] = { range[0], range[1] };

	calcHist(&hsvImg, 1, channels, Mat(), feature, 2, histSize, pRange);
	normalize(feature, feature, 0, 1, cv::NORM_MINMAX);
}

// 比较特征值
double CompareFeature(const Mat& feature1, const Mat& feature2)
{
	return compareHist(feature1, feature2, cv::HISTCMP_BHATTACHARYYA);
}


// 从主题预览取图片地址
void GetImage(const ThreadInfo& thread, vector<CString>& img)
{
	for (std::regex_iterator<LPCTSTR> it((LPCTSTR)thread.preview, (LPCTSTR)thread.preview
		+ thread.preview.GetLength(), THREAD_IMG_REG), end; it != end; it++)
		img.push_back((*it)[1].str().c_str());
}

// 从帖子取图片地址
void GetImage(const PostInfo& post, vector<CString>& img)
{
	for (std::regex_iterator<LPCTSTR> it((LPCTSTR)post.content, (LPCTSTR)post.content
		+ post.content.GetLength(), POST_IMG_REG), end; it != end; it++)
		img.push_back((*it)[1].str().c_str());
}
