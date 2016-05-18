#include "stdafx.h"
#include "TBMConfigPath.h"


// 配置文件路径
CString OPTIONS_DIR_PATH = _T("Option\\");
CString IMG_CACHE_PATH = _T("ImageCache\\");
CString PLUGIN_PATH = _T("Plugin\\");

CString GLOBAL_CONFIG_PATH = _T("\\options.xml");	// 程序初始化时初始化

CString USERS_DIR_PATH = _T("\\User\\");			// 程序初始化时初始化
CString USER_CONFIG_PATH;							// 确定用户时初始化
CString CURRENT_USER_DIR_PATH;						// 确定用户时初始化
CString COOKIE_PATH;								// 确定用户时初始化
CString CACHE_PATH;									// 确定用户时初始化
