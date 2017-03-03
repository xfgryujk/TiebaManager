/*
Copyright (C) 2011-2017  xfgryujk
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

#include "stdafx.h"
#include <TBMCoreRules.h>
#include <ImageHelper.h>
#include <opencv2\imgproc.hpp>


// 图片条件

CString CImageCondition::GetDescription(const CConditionParam& _param)
{
	const auto& param = (CImageParam&)_param;

	CString res = _T("含有图片\"") + param.m_imagePath + _T("\"，");
	CString tmp;
	switch (param.m_algorithm)
	{
	case CImageParam::EQUAL:           tmp = _T("完全相等"); break;
	case CImageParam::PSNR:            tmp.Format(_T("PSNR > %.3lf%s"), param.m_threshold, param.m_ignoreSize ? _T("，忽略尺寸") : _T("")); break;
	case CImageParam::SSIM:            tmp.Format(_T("SSIM > %.3lf%s"), param.m_threshold, param.m_ignoreSize ? _T("，忽略尺寸") : _T("")); break;
	case CImageParam::MATCH_TEMPLATE:  tmp.Format(_T("模板匹配最小差异 < %.3lf"), param.m_threshold); break;
	}
	res += tmp;
	return res;
}


CConditionParam* CImageCondition::ReadParam(const tinyxml2::XMLElement* optionNode)
{
	auto* param = new CImageParam();

	COption<CString> path("Path");
	COption<int> algorithm("Algorithm", CImageParam::PSNR, InRange<int, CImageParam::EQUAL, CImageParam::MATCH_TEMPLATE>);
	COption<BOOL> ignoreSize("IgnoreSize", FALSE);
	COption<double> threshold("Threshold", 35.0);
	path.Read(*optionNode);
	algorithm.Read(*optionNode);
	ignoreSize.Read(*optionNode);
	threshold.Read(*optionNode);

	param->m_imagePath = path;
	param->m_algorithm = CImageParam::Algorithm(*algorithm);
	param->m_ignoreSize = ignoreSize;
	param->m_threshold = threshold;

	UpdateImage(*param);

	return param;
}

void CImageCondition::WriteParam(const CConditionParam& _param, tinyxml2::XMLElement* optionNode)
{
	const auto& param = (CImageParam&)_param;

	COption<CString> path("Path");
	*path = param.m_imagePath;
	path.Write(*optionNode);
	COption<int> algorithm("Algorithm");
	*algorithm = param.m_algorithm;
	algorithm.Write(*optionNode);
	COption<BOOL> ignoreSize("IgnoreSize");
	*ignoreSize = param.m_ignoreSize;
	ignoreSize.Write(*optionNode);
	COption<double> threshold("Threshold");
	*threshold = param.m_threshold;
	threshold.Write(*optionNode);
}

CConditionParam* CImageCondition::CloneParam(const CConditionParam& _param)
{
	const auto& param = (CImageParam&)_param;
	return new CImageParam(param);
}

void CImageCondition::UpdateImage(CImageParam& param)
{
	if (!ReadImage(param.m_imagePath, param.m_image))
	{
		AfxMessageBox(_T("加载图片失败：") + param.m_imagePath);
		return;
	}

	if (param.m_algorithm == CImageParam::EQUAL)
		return;

	// 对其他算法只比较灰度图，提高速度
	cv::Mat gray;
	cv::cvtColor(param.m_image, gray, CV_BGR2GRAY);
	param.m_image = gray;
}


BOOL CImageCondition::Match(const CImageParam& param, const TBObject& obj)
{
	// 未完成
	return FALSE;
}

BOOL CImageCondition::MatchThread(const CConditionParam& _param, const ThreadInfo& thread, int& pos, int& length)
{
	return Match((CImageParam&)_param, thread);
}

BOOL CImageCondition::MatchPost(const CConditionParam& _param, const PostInfo& post, int& pos, int& length)
{
	return Match((CImageParam&)_param, post);
}

BOOL CImageCondition::MatchLzl(const CConditionParam& _param, const LzlInfo& lzl, int& pos, int& length)
{
	return Match((CImageParam&)_param, lzl);
}

