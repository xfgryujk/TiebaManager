#include "stdafx.h"
#include "Update.h"
#include "StringHelper.h"
#include "NetworkHelper.h"
#include "TiebaManagerDlg.h"


// 当前版本日期，每次更新后修改
const char UPDATE_CURRENT_VERSION_A[] = "16-04-08";
const CString UPDATE_CURRENT_VERSION(UPDATE_CURRENT_VERSION_A);
// 更新日志
const TCHAR UPDATE_LOG[] = _T("1. 支持导入导出txt文件\r\n")
						   _T("2. 修复低分辨率下UI多出一块空白\r\n")
						   _T("3. 确认操作时可用浏览器打开帖子\r\n")
						   _T("4. 支持扫描楼中楼语音帖");

// 检查更新
CheckUpdateResult CheckUpdate()
{
	static const wregex CHECK_UPDATE_REG(_T("\"fs_id\":(\\d+),\"path\":\"\\\\/\\\\u6211\\\\u7684\\\\u5206\\\\u4eab\\\\/\\\\u767e")
										 _T("\\\\u5ea6\\\\u8d34\\\\u5427\\\\u76f8\\\\u5173\\\\/\\\\u8d34\\\\u5427\\\\u7ba1\\\\u7")
										 _T("406\\\\u5668.zip\",\"server_filename\":\"\\\\u8d34\\\\u5427\\\\u7ba1\\\\u7406\\\\u5")
										 _T("668.zip\".*?\"server_ctime\":(\\d+),"));
	CString src = HTTPGet(_T("http://pan.baidu.com/share/list?channel=chunlei&clienttype=0&web=1&num=100&page=1&dir=%2F%E6%88%91")
						  _T("%E7%9A%84%E5%88%86%E4%BA%AB%2F%E7%99%BE%E5%BA%A6%E8%B4%B4%E5%90%A7%E7%9B%B8%E5%85%B3&uk=436464474&")
						  _T("shareid=497149087"), FALSE);
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
	time.Format(_T("%02d-%02d-%02d"), tmServer_ctime.tm_year % 100, tmServer_ctime.tm_mon + 1, tmServer_ctime.tm_mday);
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
	src = HTTPGet(UPDATE_URL, FALSE);
	CString timeStamp = GetStringBetween(src, TIME_STAMP_LEFT, TIME_STAMP_RIGHT);
	CString sign = GetStringBetween(src, SIGN_LEFT, SIGN_RIGHT);
	CString bdstoken = GetStringBetween(src, BDSTOKEN_LEFT, BDSTOKEN_RIGHT);
	CString url;
	url.Format(_T("http://pan.baidu.com/api/sharedownload?sign=%s&timestamp=%s&bdstoken=%s&channel=chunlei")
			   _T("&clienttype=0&web=1&app_id=250528"), sign, timeStamp, bdstoken);
	src = HTTPPost(url, _T("encrypt=0&product=share&uk=436464474&primaryid=497149087&fid_list=%5B") + fs_id + _T("%5D"), FALSE);

	// 文件下载链接
	CString dlink = GetStringBetween(src, DLINK_LEFT, DLINK_RIGHT);
	if (dlink == _T(""))
		return UPDATE_FAILED_TO_GET_LINK;

	dlink.Replace(_T("\\/"), _T("/"));
	ShellExecute(NULL, _T("open"), dlink, NULL, NULL, SW_NORMAL);
	AfxMessageBox(_T("下载后解压并覆盖旧版本文件，可保留当前设置"), MB_ICONINFORMATION);
	return UPDATE_HAS_UPDATE;
}

// 自动更新线程
UINT AFX_CDECL AutoUpdateThread(LPVOID _dlg)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)_dlg;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CheckUpdateResult res = CheckUpdate();
	CoUninitialize();
	switch (res)
	{
	case UPDATE_FAILED_TO_GET_FILE_ID:
		dlg->m_stateStatic.SetWindowText(_T("检查更新失败：获取文件ID失败，在设置里手动检查更新"));
		break;
	case UPDATE_FAILED_TO_GET_LINK:
		if (AfxMessageBox(_T("获取下载地址失败，手动更新？"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			ShellExecute(NULL, _T("open"), UPDATE_FULL_URL, NULL, NULL, SW_NORMAL);
	case UPDATE_NO_UPDATE:
	case UPDATE_HAS_UPDATE:
		dlg->m_stateStatic.SetWindowText(_T("待机中"));
		break;
	}

	return 0;
}
