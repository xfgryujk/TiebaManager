/*
Copyright (C) 2011-2017  xfgryujk
https://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

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
#include "TiebaManagerCoreCommon.h"
#include "TBMCoreRule.h"
#include "Singleton.h"
#include "StringHelper.h"
#pragma warning(disable:4819) // OpenCV头文件包含Unicode字符
#include <opencv2\core\mat.hpp>


// 规则

class TBM_CORE_API CIllegalRule final : public CRule
{
public:
	BOOL m_forceToConfirm = FALSE;	          // 强制确认
	int m_trigCount = 0;    			      // 触发次数
};


// 条件

// 关键词条件

class TBM_CORE_API CKeywordParam final : public CConditionParam
{
public:
	enum KeywordRange
	{
		TITLE,            // 主题标题
		PREVIEW,          // 主题预览
		POST_CONTENT,     // 帖子内容
		LZL_CONTENT,      // 楼中楼内容
		AUTHOR,           // 作者名
		ALL_CONTENT       // 所有内容（包括主题标题、主题预览、帖子内容、楼中楼内容）
	};


	CKeywordParam() : CConditionParam(_T("关键词条件")) { }


	KeywordRange m_range = ALL_CONTENT;      // 搜索范围
	BOOL m_not = FALSE;                      // 结果取FALSE
	BOOL m_include = TRUE;                   // TRUE为包含，FALSE为匹配
	RegexText m_keyword;                     // 关键词
};

class TBM_CORE_API CKeywordCondition final : public CCondition, public Singleton<CKeywordCondition>
{
	DECL_SINGLETON(CKeywordCondition);
private:
	CKeywordCondition() : CCondition(_T("关键词条件")) { };

public:
	virtual CString GetDescription(const CConditionParam& param) override;

	virtual CConditionParam* ReadParam(const tinyxml2::XMLElement* optionNode) override;
	virtual void WriteParam(const CConditionParam& param, tinyxml2::XMLElement* optionNode) override;
	virtual CConditionParam* CloneParam(const CConditionParam& param) override;

	virtual BOOL MatchThread(const CConditionParam& param, const ThreadInfo& thread, int& pos, int& length) override;
	virtual BOOL MatchPost(const CConditionParam& param, const PostInfo& post, int& pos, int& length) override;
	virtual BOOL MatchLzl(const CConditionParam& param, const LzlInfo& lzl, int& pos, int& length) override;

private:
	BOOL Match(const CKeywordParam& param, const CString& content, int startPos, int& pos, int& length);
};


// 等级条件

class TBM_CORE_API CLevelParam final : public CConditionParam
{
public:
	enum Operator
	{
		LESS,             // <=
		GREATER           // >=
	};


	CLevelParam() : CConditionParam(_T("等级条件")) { }


	Operator m_operator = LESS;  // 操作符
	int m_level = 1;             // 等级
};

class TBM_CORE_API CLevelCondition final : public CCondition, public Singleton<CLevelCondition>
{
	DECL_SINGLETON(CLevelCondition);
private:
	CLevelCondition() : CCondition(_T("等级条件")) { };

public:
	virtual CString GetDescription(const CConditionParam& param) override;

	virtual CConditionParam* ReadParam(const tinyxml2::XMLElement* optionNode) override;
	virtual void WriteParam(const CConditionParam& param, tinyxml2::XMLElement* optionNode) override;
	virtual CConditionParam* CloneParam(const CConditionParam& param) override;

	virtual BOOL MatchThread(const CConditionParam& param, const ThreadInfo& thread, int& pos, int& length) override;
	virtual BOOL MatchPost(const CConditionParam& param, const PostInfo& post, int& pos, int& length) override;
	virtual BOOL MatchLzl(const CConditionParam& param, const LzlInfo& lzl, int& pos, int& length) override;
};


// 时间条件

class TBM_CORE_API CTimeParam final : public CConditionParam
{
public:
	enum Operator
	{
		LESS,             // <=
		GREATER           // >=
	};


	CTimeParam() : CConditionParam(_T("时间条件")) { }


	Operator m_operator = GREATER;  // 操作符
	time_t m_time = time(NULL);     // Unix时间戳
};

class TBM_CORE_API CTimeCondition final : public CCondition, public Singleton<CTimeCondition>
{
	DECL_SINGLETON(CTimeCondition);
private:
	CTimeCondition() : CCondition(_T("时间条件")) { };

public:
	virtual CString GetDescription(const CConditionParam& param) override;

	virtual CConditionParam* ReadParam(const tinyxml2::XMLElement* optionNode) override;
	virtual void WriteParam(const CConditionParam& param, tinyxml2::XMLElement* optionNode) override;
	virtual CConditionParam* CloneParam(const CConditionParam& param) override;

	virtual BOOL MatchThread(const CConditionParam& param, const ThreadInfo& thread, int& pos, int& length) override;
	virtual BOOL MatchPost(const CConditionParam& param, const PostInfo& post, int& pos, int& length) override;
	virtual BOOL MatchLzl(const CConditionParam& param, const LzlInfo& lzl, int& pos, int& length) override;

private:
	BOOL Match(const CTimeParam& param, const TBObject& obj);
};


// 图片条件

class TBM_CORE_API CImageParam final : public CConditionParam
{
public:
	enum Algorithm
	{
		EQUAL,            // RGB完全相等
		PSNR,
		SSIM,             // 其实是MSSIM...
		MATCH_TEMPLATE    // 模板匹配，计算使用归一化平方差
	};


	CImageParam() : CConditionParam(_T("图片条件")) { }


	CString m_imagePath;             // 图片路径
	Algorithm m_algorithm = PSNR;    // 比较算法
	BOOL m_ignoreSize = FALSE;       // 比较不同尺寸图片前缩放，用于PSNR、SSIM
	double m_threshold = 35.0;       // 阈值，用于PSNR、SSIM、模板匹配
						     
	cv::Mat m_image;                 // 图片数据，改变m_imagePath或m_algorithm后应立即调用CImageCondition::UpdateImage
};

class TBM_CORE_API CImageCondition final : public CCondition, public Singleton<CImageCondition>
{
	DECL_SINGLETON(CImageCondition);
private:
	CImageCondition() : CCondition(_T("图片条件")) { };

public:
	virtual CString GetDescription(const CConditionParam& param) override;

	virtual CConditionParam* ReadParam(const tinyxml2::XMLElement* optionNode) override;
	virtual void WriteParam(const CConditionParam& param, tinyxml2::XMLElement* optionNode) override;
	virtual CConditionParam* CloneParam(const CConditionParam& param) override;
	// 读取图片数据、预处理
	void UpdateImage(CImageParam& param);

	virtual BOOL MatchThread(const CConditionParam& param, const ThreadInfo& thread, int& pos, int& length) override;
	virtual BOOL MatchPost(const CConditionParam& param, const PostInfo& post, int& pos, int& length) override;
	virtual BOOL MatchLzl(const CConditionParam& param, const LzlInfo& lzl, int& pos, int& length) override;

	// 特殊情况返回负值
	double CompareImage(const CImageParam& param, const cv::Mat& img);

private:
	BOOL Match(const CImageParam& param, const TBObject& obj);
};


// 楼层条件

class TBM_CORE_API CFloorParam final : public CConditionParam
{
public:
	enum Operator
	{
		LESS,             // <=
		EQUAL,            // ==
		GREATER           // >=
	};


	CFloorParam() : CConditionParam(_T("楼层条件")) { }


	Operator m_operator = LESS;  // 操作符
	int m_floor = 2;             // 楼层
};

class TBM_CORE_API CFloorCondition final : public CCondition, public Singleton<CFloorCondition>
{
	DECL_SINGLETON(CFloorCondition);
private:
	CFloorCondition() : CCondition(_T("楼层条件")) { };

public:
	virtual CString GetDescription(const CConditionParam& param) override;

	virtual CConditionParam* ReadParam(const tinyxml2::XMLElement* optionNode) override;
	virtual void WriteParam(const CConditionParam& param, tinyxml2::XMLElement* optionNode) override;
	virtual CConditionParam* CloneParam(const CConditionParam& param) override;

	virtual BOOL MatchThread(const CConditionParam& param, const ThreadInfo& thread, int& pos, int& length) override;
	virtual BOOL MatchPost(const CConditionParam& param, const PostInfo& post, int& pos, int& length) override;
	virtual BOOL MatchLzl(const CConditionParam& param, const LzlInfo& lzl, int& pos, int& length) override;

private:
	BOOL Match(const CConditionParam& param, const CString& floor);
};
