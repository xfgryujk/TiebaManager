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
