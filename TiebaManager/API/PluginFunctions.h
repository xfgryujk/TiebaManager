#pragma once
// 插件应至少实现Init和Uninit
//extern "C" __declspec(dllexport) bool __stdcall Init()

typedef bool(__stdcall *InitType)();
typedef bool(__stdcall *UninitType)();
typedef CString(__stdcall *GetDescriptionType)();
typedef bool(__stdcall *OnConfigType)();
