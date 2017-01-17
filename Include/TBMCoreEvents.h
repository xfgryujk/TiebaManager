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
#include "TiebaManagerCoreCommon.h"
#include <EventHelper.h>
#include <TBMOperate.h>
#include <vector>


// 操作事件

extern TBM_CORE_API PreEvent<const Operation&, BOOL&>   g_comfirmEvent;        // 确认是否操作，参数：操作, 不取消

extern TBM_CORE_API PreEvent<const Operation&, BOOL&>   g_preOperateEvent;     // 准备操作时触发，参数：操作, 不取消
extern TBM_CORE_API PreEvent<const Operation&, BOOL&>   g_preBanEvent;         // 准备封禁时触发，参数：操作, 不取消
extern TBM_CORE_API PostEvent<const Operation&, BOOL>   g_postBanEvent;        // 已封禁时触发，参数：操作, 封禁成功
extern TBM_CORE_API PreEvent<const Operation&, BOOL&>   g_preDefriendEvent;    // 准备拉黑时触发，参数：操作, 不取消
extern TBM_CORE_API PostEvent<const Operation&, BOOL>   g_postDefriendEvent;   // 已拉黑时触发，参数：操作, 封禁成功
extern TBM_CORE_API PreEvent<const Operation&, BOOL&>   g_preDeleteEvent;      // 准备删除时触发，参数：操作, 不取消
extern TBM_CORE_API PostEvent<const Operation&, BOOL>   g_postDeleteEvent;     // 已删除时触发，参数：操作, 封禁成功

// 扫描事件

extern TBM_CORE_API PreEvent<const ThreadInfo&, BOOL&, CString&, BOOL&, int&, int&>   g_checkThreadIllegalEvent;        // 判断主题是否违规，参数：主题信息, 是否违规, 日志里输出的信息, 强制确认, 确认时高亮位置, 确认时高亮长度
extern TBM_CORE_API PreEvent<const ThreadInfo&, BOOL&, CString&, BOOL&, int&, int&>   g_checkThreadImageIllegalEvent;   // 判断主题图片是否违规，参数：主题信息, 是否违规, 日志里输出的信息, 强制确认, 确认时高亮位置, 确认时高亮长度
extern TBM_CORE_API PreEvent<const PostInfo&, BOOL&, CString&, BOOL&, int&, int&>     g_checkPostIllegalEvent;          // 判断帖子是否违规，参数：帖子信息, 是否违规, 日志里输出的信息, 强制确认, 确认时高亮位置, 确认时高亮长度
extern TBM_CORE_API PreEvent<const PostInfo&, BOOL&, CString&, BOOL&, int&, int&>     g_checkPostImageIllegalEvent;     // 判断帖子图片是否违规，参数：帖子信息, 是否违规, 日志里输出的信息, 强制确认, 确认时高亮位置, 确认时高亮长度
extern TBM_CORE_API PreEvent<const LzlInfo&, BOOL&, CString&, BOOL&, int&, int&>      g_checkLzlIllegalEvent;           // 判断楼中楼是否违规，参数：帖子信息, 是否违规, 日志里输出的信息, 强制确认, 确认时高亮位置, 确认时高亮长度
extern TBM_CORE_API PreEvent<const LzlInfo&, BOOL&, CString&, BOOL&, int&, int&>      g_checkLzlImageIllegalEvent;      // 判断楼中楼图片是否违规，参数：帖子信息, 是否违规, 日志里输出的信息, 强制确认, 确认时高亮位置, 确认时高亮长度

extern TBM_CORE_API PreEvent<BOOL&>                                g_scanThreadStartEvent;      // 总扫描线程开始，参数：不取消
extern TBM_CORE_API PostEvent<>                                    g_scanThreadEndEvent;        // 总扫描线程结束
extern TBM_CORE_API PreEvent<BOOL&>                                g_scanOnceStartEvent;        // 一次扫描开始，参数：不取消
extern TBM_CORE_API PostEvent<>                                    g_scanOnceEndEvent;          // 一次扫描结束
extern TBM_CORE_API PreEvent<BOOL&>                                g_preScanAllThreadsEvent;    // 准备扫描所有主题，参数：不取消
														                     
extern TBM_CORE_API PreEvent<int, BOOL&>                           g_scanPostThreadStartEvent;  // 扫描帖子线程开始，参数：线程ID, 不取消
extern TBM_CORE_API PostEvent<int>                                 g_scanPostThreadEndEvent;    // 扫描帖子线程结束，参数：线程ID
extern TBM_CORE_API PreEvent<int, const ThreadInfo&, BOOL&>        g_preScanThreadEvent;        // 准备扫描某个主题，参数：线程ID, 主题信息, 不取消

extern TBM_CORE_API PreEvent<int, const ThreadInfo&, int, BOOL&>   g_scanPostPageEvent;         // 扫描帖子页开始，参数：线程ID, 主题信息, 帖子页, 不取消


// 外部模块可注册的事件集合，用来自动卸载外部listener
extern TBM_CORE_API std::vector<EventBase*> g_externalEvents;
