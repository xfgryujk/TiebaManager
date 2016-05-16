#pragma once
#include <TBMCoreConfig.h>
#include <StringHelper.h>
#include <opencv2\core\mat.hpp>


// 全局配置
class CGlobalConfig : public CConfigBase
{
public:
	COption<BOOL> m_firstRun;				// 第一次运行
	COption<BOOL> m_firstRunAfterUpdate;	// 更新后第一次运行
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
	COption<BOOL>				m_autoLoopBan;		// 自动循环封
	COption<vector<Keyword> >	m_keywords;			// 违规内容
	vector<NameImage>			m_images;			// 违规图片
	COption<CString>			m_imageDir;			// 违规图片目录
	COption<double>				m_SSIMThreshold;	// 阈值
	COption<vector<RegexText> >	m_blackList;		// 屏蔽用户
	COption<set<CString> >		m_whiteList;		// 信任用户
	COption<vector<RegexText> >	m_whiteContent;		// 信任内容
	COption<set<CString> >		m_trustedThread;	// 信任主题

	CPlan();
	void OnChange();
	void PostChange();
};
