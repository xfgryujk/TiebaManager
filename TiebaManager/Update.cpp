#include "stdafx.h"
#include "Update.h"
#include "Global.h"


// 当前版本日期，每次更新后修改
const TCHAR UPDATE_CURRENT_VERSION[] = _T("15-09-14");

// 检查更新
CheckUpdateResult CheckUpdate()
{
	static const wregex CHECK_UPDATE_REG(_T("\"fs_id\":(\\d+),\"path\":\"\\\\/\\\\u6211\\\\u7684\\\\\
u5206\\\\u4eab\\\\/\\\\u767e\\\\u5ea6\\\\u8d34\\\\u5427\\\\u76f8\\\\u5173\\\\/\\\\u8d34\\\\u5427\\\\\
u7ba1\\\\u7406\\\\u5668.zip\",\"server_filename\":\"\\\\u8d34\\\\u5427\\\\u7ba1\\\\u7406\\\\u5668.zip\
\".*?\"server_ctime\":(\\d+),"));
	CString src = HTTPGet(_T("http://pan.baidu.com/share/list?channel=chunlei&clienttype=0&web=1\
&num=100&page=1&dir=%2F%E6%88%91%E7%9A%84%E5%88%86%E4%BA%AB%2F%E7%99%BE%E5%BA%A6%E8%B4%B4%E5%90%A7\
%E7%9B%B8%E5%85%B3&uk=436464474&shareid=497149087"), FALSE);
	std::wcmatch res;
	if (!std::regex_search((LPCTSTR)src, res, CHECK_UPDATE_REG))
		return UPDATE_FAILED_TO_GET_FILE_ID;

	// 文件ID
	CString fs_id = res[1].str().c_str();

	// 上传时间戳
	CString server_ctime = res[2].str().c_str();
	time_t tServer_ctime = _ttoi64(server_ctime);
	tm tmServer_ctime;
	localtime_s(&tmServer_ctime, &tServer_ctime);
	// 上传时间
	CString time;
	time.Format(_T("%d-%02d-%02d"), tmServer_ctime.tm_year % 100, tmServer_ctime.tm_mon + 1, tmServer_ctime.tm_mday);
	if (time == UPDATE_CURRENT_VERSION)
		return UPDATE_NO_UPDATE;

	if (AfxMessageBox(_T("最新版本") + time + _T("，是否更新？"), MB_ICONQUESTION | MB_YESNO) == IDNO)
		return UPDATE_HAS_UPDATE;

	static const TCHAR TIME_STAMP_LEFT[] = _T("yunData.TIMESTAMP = \"");
	static const TCHAR TIME_STAMP_RIGHT[] = _T("\"");
	static const TCHAR SIGN_LEFT[] = _T("yunData.SIGN = \"");
	static const TCHAR SIGN_RIGHT[] = _T("\"");
	static const TCHAR BDSTOKEN_LEFT[] = _T("yunData.MYBDSTOKEN = \"");
	static const TCHAR BDSTOKEN_RIGHT[] = _T("\"");
	static const TCHAR DLINK_LEFT[] = _T("\"dlink\":\"");
	static const TCHAR DLINK_RIGHT[] = _T("\"");
	src = HTTPGet(_T("http://pan.baidu.com/s/1hq86os8"), FALSE);
	CString timeStamp = GetStringBetween(src, TIME_STAMP_LEFT, TIME_STAMP_RIGHT);
	CString sign = GetStringBetween(src, SIGN_LEFT, SIGN_RIGHT);
	CString bdstoken = GetStringBetween(src, BDSTOKEN_LEFT, BDSTOKEN_RIGHT);
	CString url;
	url.Format(_T("http://pan.baidu.com/api/sharedownload?sign=%s&timestamp=%s\
&bdstoken=%s&channel=chunlei&clienttype=0&web=1&app_id=250528"), sign, timeStamp, bdstoken);
	src = HTTPPost(url, _T("encrypt=0&product=share&uk=436464474&primaryid=497149087&fid_list=%5B")
		+ fs_id + _T("%5D"), FALSE);

	// 文件下载链接
	CString dlink = GetStringBetween(src, DLINK_LEFT, DLINK_RIGHT);
	if (dlink == _T(""))
		return UPDATE_FAILED_TO_GET_LINK;

	dlink.Replace(_T("\\/"), _T("/"));
	ShellExecute(NULL, _T("open"), dlink, NULL, NULL, SW_NORMAL);
	AfxMessageBox(_T("下载后解压并覆盖旧版本文件，可保留当前设置"), MB_ICONINFORMATION);
	return UPDATE_HAS_UPDATE;
}
