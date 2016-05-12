#pragma once
#include "TBMCommon.h"
#include <ConfigFile.h>
#include <StringHelper.h>
#include <opencv2\core\mat.hpp>


// 配置文件路径
extern TIEBA_MANAGER_API CString GLOBAL_CONFIG_PATH;	// 程序运行时初始化
extern TIEBA_MANAGER_API CString USER_PROFILE_PATH;		// 确定贴吧时初始化
extern TIEBA_MANAGER_API CString OPTIONS_PATH;
extern TIEBA_MANAGER_API CString USERS_PATH;			// 程序运行时初始化
extern TIEBA_MANAGER_API CString CURRENT_USER_PATH;		// 确定贴吧时初始化
extern TIEBA_MANAGER_API CString COOKIE_PATH;			// 确定贴吧时初始化
extern TIEBA_MANAGER_API CString CACHE_PATH;			// 确定贴吧时初始化


// 全局配置
class TIEBA_MANAGER_API CGlobalConfig : public CConfigBase
{
public:
	COption<BOOL> m_firstRun;				// 第一次运行
	COption<BOOL> m_firstRunAfterUpdate;	// 更新后第一次运行
	COption<CString> m_currentUser;			// 当前账号
	COption<BOOL> m_autoUpdate;				// 自动更新

	CGlobalConfig();
};
extern TIEBA_MANAGER_API CGlobalConfig g_globalConfig;

// 用户配置
class TIEBA_MANAGER_API CUserConfig : public CConfigBase
{
public:
	COption<CString> m_plan;		// 当前方案
	COption<CString> m_forumName;	// 贴吧名

	CUserConfig();
};
extern TIEBA_MANAGER_API CUserConfig g_userConfig;

// 方案
class TIEBA_MANAGER_API CPlan : public CConfigBase
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

	COption<int>		m_scanInterval;			// 扫描间隔
	COption<BOOL>		m_onlyScanTitle;		// 只扫描标题
	COption<int>		m_scanPageCount;		// 扫描最后页数
	COption<BOOL>		m_briefLog;				// 只输出删帖封号
	COption<int>		m_threadCount;			// 线程数
	COption<BOOL>		m_autoSaveLog;			// 自动保存日志
	COption<int>		m_illegalLevel;			// 违规等级
	COption<BOOL>		m_delete;				// 删帖
	COption<BOOL>		m_banID;				// 封ID
	COption<BOOL>		m_defriend;				// 拉黑
	COption<float>		m_deleteInterval;		// 删帖间隔
	COption<int>		m_banDuration;			// 封禁时长
	COption<CString>	m_banReason;			// 封号原因
	COption<int>		m_banTrigCount;			// 封禁违规次数
	COption<int>		m_defriendTrigCount;	// 拉黑违规次数
	COption<BOOL>		m_confirm;				// 操作前提示
	COption<BOOL>		m_wapBanInterface;		// 封禁用WAP接口
	COption<BOOL>		m_autoLoopBan;			// 自动循环封
	COption<vector<Keyword> >	m_keywords;		// 违规内容
	vector<NameImage>			m_images;		// 违规图片
	COption<CString>	m_imageDir;				// 违规图片目录
	COption<double>		m_SSIMThreshold;		// 阈值
	COption<vector<RegexText> >	m_blackList;	// 屏蔽用户
	COption<set<CString> >		m_whiteList;	// 信任用户
	COption<vector<RegexText> >	m_whiteContent;	// 信任内容
	COption<set<CString> >		m_trustedThread;// 信任主题

	CPlan();
	void OnChange(){ m_optionsLock.Lock(); }
	void PostChange();
};
extern TIEBA_MANAGER_API CPlan g_plan;
