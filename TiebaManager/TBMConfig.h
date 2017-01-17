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
#include <TBMCoreConfig.h>
#include <StringHelper.h>
#pragma warning(disable:4819) // OpenCV头文件包含Unicode字符
#include <opencv2\core\mat.hpp>


// 全局配置
class CGlobalConfig : public CConfigBase
{
public:
	COption<BOOL> m_firstRun;				// 第一次运行
	COption<CString> m_currentUser;			// 当前账号
	COption<BOOL> m_autoUpdate;				// 自动更新

	CGlobalConfig();
};

// 用户配置
class CUserConfig : public CConfigBase
{
public:
	COption<CString> m_plan;		// 当前方案
	COption<CString> m_forumName;	// 贴吧名

	CUserConfig();
};

// Cookie文件
class CCookieConfig : public CConfigBase
{
public:
	COption<CString> m_cookie;

	CCookieConfig();
};

// 用不相关的两个内容做测试
const TCHAR MATCH_TOO_MUCH_CONTENT_TEST1[] = _T("【吧务导航】删帖查询，帖子申精，吧规，刷豆，刷粉");
const TCHAR MATCH_TOO_MUCH_CONTENT_TEST2[] = _T("贺岁·番外14·叫兽教你烤地瓜");
const TCHAR MATCH_TOO_MUCH_USERNAME_TEST1[] = _T("盗我原号的没J8");
const TCHAR MATCH_TOO_MUCH_USERNAME_TEST2[] = _T("从容人生");
// 方案
class CPlan : public CTBMCoreConfig
{
public:
	struct Keyword : RegexText
	{
		BOOL forceToConfirm;	// 强制确认
		int trigCount;			// 触发次数
	};

	struct NameImage
	{
		CString name;
		cv::Mat img;
	};
	
public:
	CCriticalSection m_optionsLock; // 方案临界区
	BOOL m_updateImage; // 读取后更新违规图片

	COption<BOOL>				m_autoSaveLog;		// 自动保存日志
	COption<int>				m_illegalLevel;		// 违规等级
	COption<std::vector<Keyword> >	m_keywords;			// 违规内容
	std::vector<NameImage>			m_images;			// 违规图片
	COption<CString>			m_imageDir;			// 违规图片目录
	COption<double>				m_SSIMThreshold;	// 阈值
	COption<std::vector<RegexText> >	m_blackList;		// 屏蔽用户
	COption<std::set<CString> >		m_whiteList;		// 信任用户
	COption<std::vector<RegexText> >	m_whiteContent;		// 信任内容
	COption<std::set<CString> >		m_trustedThread;	// 信任主题

	CPlan();
	void OnChange();
	void PostChange();
};
