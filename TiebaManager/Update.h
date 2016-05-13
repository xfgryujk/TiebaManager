#pragma once


#define UPDATE_URL _T("http://sinacloud.net/xfgryujk/TiebaManager/贴吧管理器.zip")
#define UPDATE_INFO_URL UPDATE_URL _T("?meta&formatter=json")

extern const char UPDATE_CURRENT_VERSION_A[];
extern const CString UPDATE_CURRENT_VERSION;
extern const TCHAR UPDATE_LOG[];


enum CheckUpdateResult { UPDATE_NO_UPDATE, UPDATE_HAS_UPDATE, UPDATE_FAILED_TO_GET_FILE_INFO };
CheckUpdateResult CheckUpdate();
UINT AFX_CDECL AutoUpdateThread(LPVOID _dlg);
