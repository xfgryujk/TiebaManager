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

#include "stdafx.h"
#include "Update.h"

#import <msscript.ocx> no_namespace
#include <StringHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>

#include "TiebaManagerDlg.h"


// 当前版本日期，每次更新后修改
const char UPDATE_CURRENT_VERSION_A[] = "16-05-20";
const CString UPDATE_CURRENT_VERSION(UPDATE_CURRENT_VERSION_A);
// 更新日志
const TCHAR UPDATE_LOG[] = _T("1. 支持插件开发\r\n")
						   _T("2. 修复一堆BUG");


// 字符串转时间戳
static time_t Str2Time(const CString& str)
{
	CComPtr<IScriptControl> script;
	if (FAILED(script.CoCreateInstance(__uuidof(ScriptControl))))
		return 0;
	script->put_Language(_bstr_t(_T("JScript")));
	_variant_t result;
	if (FAILED(script->raw_Eval(_bstr_t(_T("Date.parse(\"") + str + _T("\")")), result.GetAddress())))
		return 0;
	return (__int64)(result.dblVal / 1000);
}

// 检查更新
CheckUpdateResult CheckUpdate()
{
	// 取上传时间
	CString src = HTTPGet(UPDATE_INFO_URL);
	// 字符串
	CString time = GetStringBetween(src, _T(R"("Last-Modified":")"), _T("\""));
	if (time == _T(""))
		return UPDATE_FAILED_TO_GET_FILE_INFO;
	// 时间戳
	time_t tTime = Str2Time(time);
	if (tTime == 0)
		return UPDATE_FAILED_TO_GET_FILE_INFO;
	tm tmTime;
	localtime_s(&tmTime, &tTime);
	// 字符串
	time.Format(_T("%02d-%02d-%02d"), tmTime.tm_year % 100, tmTime.tm_mon + 1, tmTime.tm_mday);
	if (time == UPDATE_CURRENT_VERSION)
		return UPDATE_NO_UPDATE;

	if (AfxMessageBox(_T("最新版本") + time + _T("，是否更新？"), MB_ICONQUESTION | MB_YESNO) == IDNO)
		return UPDATE_HAS_UPDATE;

	ShellExecute(NULL, _T("open"), UPDATE_URL, NULL, NULL, SW_NORMAL);
	AfxMessageBox(_T("下载后解压并覆盖旧版本文件，可保留当前设置"), MB_ICONINFORMATION);
	return UPDATE_HAS_UPDATE;
}

// 自动更新线程
UINT AFX_CDECL AutoUpdateThread(LPVOID _dlg)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)_dlg;
	if (!CoInitializeHelper())
		return 0;

	CheckUpdateResult res = CheckUpdate();
	CoUninitialize();
	switch (res)
	{
	case UPDATE_FAILED_TO_GET_FILE_INFO:
		dlg->m_stateStatic.SetWindowText(_T("检查更新失败：获取文件信息失败，在设置里手动检查更新"));
		break;
	case UPDATE_NO_UPDATE:
	case UPDATE_HAS_UPDATE:
		dlg->m_stateStatic.SetWindowText(_T("待机中"));
		break;
	}

	CoUninitialize();
	return 0;
}
