#pragma once
#include "HelperCommon.h"


// 不阻塞消息的延迟
HELPER_API void Delay(DWORD time);
// 处理消息
HELPER_API void DoEvents();
// 创建目录
HELPER_API BOOL CreateDir(const CString& path);
// 初始化COM库
HELPER_API BOOL CoInitializeHelper();
