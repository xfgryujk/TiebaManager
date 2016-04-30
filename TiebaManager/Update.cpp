#include "stdafx.h"
#include "Update.h"
#import <msscript.ocx> no_namespace
#include "StringHelper.h"
#include "NetworkHelper.h"
#include "TiebaManagerDlg.h"


// 当前版本日期，每次更新后修改
const char UPDATE_CURRENT_VERSION_A[] = "16-04-27";
const CString UPDATE_CURRENT_VERSION(UPDATE_CURRENT_VERSION_A);
// 更新日志
const TCHAR UPDATE_LOG[] = _T("1. 更换软件更新源（不用垃圾百度云了");

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
	CString src = HTTPGet(UPDATE_INFO_URL, FALSE);
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
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
		AfxMessageBox(_T("CoInitializeEx失败！"), MB_ICONERROR);
		return 0;
	}
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

	return 0;
}
