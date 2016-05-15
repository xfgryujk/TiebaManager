#include "stdafx.h"
#include <TBMConfigPath.h>


// 配置文件路径
TIEBA_MANAGER_API CString GLOBAL_CONFIG_PATH = _T("\\options.xml");	// 程序初始化时初始化
TIEBA_MANAGER_API CString USER_CONFIG_PATH;							// 确定用户时初始化
TIEBA_MANAGER_API CString OPTIONS_DIR_PATH = _T("Option\\");
TIEBA_MANAGER_API CString USERS_DIR_PATH = _T("\\User\\");			// 程序初始化时初始化
TIEBA_MANAGER_API CString CURRENT_USER_DIR_PATH;					// 确定用户时初始化
TIEBA_MANAGER_API CString COOKIE_PATH;								// 确定用户时初始化
TIEBA_MANAGER_API CString CACHE_PATH;								// 确定用户时初始化
TIEBA_MANAGER_API CString IMG_CACHE_PATH = _T("ImageCache\\");
