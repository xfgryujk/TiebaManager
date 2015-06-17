#pragma once


// ¸üÐÂ
const TCHAR UPDATE_CURRENT_VERSION[] = _T("15-06-17");
enum CheckUpdateResult { UPDATE_NO_UPDATE, UPDATE_HAS_UPDATE, UPDATE_FAILED_TO_GET_FILE_ID, UPDATE_FAILED_TO_GET_LINK };
CheckUpdateResult CheckUpdate();
