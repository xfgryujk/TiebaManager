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
#include "TiebaManagerCommon.h"
#include "EventHelper.h"
#include "TiebaClawer.h"


extern TBM_API PreEvent<>                         g_mainDialogPostInitEvent;   // 主对话框初始化后触发
extern TBM_API PostEvent<BOOL&>                   g_mainDialogCloseEvent;      // 主对话框关闭，参数：不取消
extern TBM_API PostEvent<>                        g_mainDialogDestroyEvent;    // 主对话框销毁
										          
extern TBM_API PreEvent<const CString&, BOOL&>    g_preSetCurrentUserEvent;    // 准备设置当前用户，参数：用户名, 不取消
extern TBM_API PostEvent<const CString&>          g_postSetCurrentUserEvent;   // 设置当前用户后，参数：用户名
extern TBM_API PreEvent<const CString&, BOOL&>    g_preSetTiebaEvent;          // 准备确认贴吧，参数：贴吧名, 不取消
extern TBM_API PostEvent<const CString&>          g_postSetTiebaEvent;         // 确认贴吧成功后，参数：贴吧名
							              
extern TBM_API PreEvent<const ThreadInfo&, std::vector<CString>&, BOOL&>   g_getThreadImagesEvent;   // 取主题中的图片，参数：主题信息, 结果, 使用默认的取图片
extern TBM_API PreEvent<const PostInfo&, std::vector<CString>&, BOOL&>     g_getPostImagesEvent;     // 取帖子中的图片，参数：帖子信息, 结果, 使用默认的取图片
extern TBM_API PreEvent<const LzlInfo&, std::vector<CString>&, BOOL&>      g_getLzlImagesEvent;      // 取楼中楼中的图片，参数：楼中楼信息, 结果, 使用默认的取图片
							              
extern TBM_API PreEvent<const CString&, BOOL&>    g_openLinkInLogEvent;        // 打开日志中的链接，参数：URL, 不取消


// 外部模块可注册的事件集合，用来自动卸载外部listener
extern TBM_API std::vector<EventBase*> g_tbmEvents;
