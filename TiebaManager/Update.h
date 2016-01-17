#pragma once


const TCHAR UPDATE_FULL_URL[] = _T("http://pan.baidu.com/s/1i4vlYIh")
								_T("#path=%252F%25E7%2599%25BE%25E5%25BA%25A6%25E8%25B4")
								_T("%25B4%25E5%2590%25A7%25E7%259B%25B8%25E5%2585%25B3");


enum CheckUpdateResult { UPDATE_NO_UPDATE, UPDATE_HAS_UPDATE, UPDATE_FAILED_TO_GET_FILE_ID, UPDATE_FAILED_TO_GET_LINK };
CheckUpdateResult CheckUpdate();
