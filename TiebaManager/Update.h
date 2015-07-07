#pragma once


// 当前版本日期，每次更新后修改
const TCHAR UPDATE_CURRENT_VERSION[] = _T("15-07-07");
enum CheckUpdateResult { UPDATE_NO_UPDATE, UPDATE_HAS_UPDATE, UPDATE_FAILED_TO_GET_FILE_ID, UPDATE_FAILED_TO_GET_LINK };
CheckUpdateResult CheckUpdate();
