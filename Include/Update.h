/*
Copyright (C) 2011-2017  xfgryujk
https://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

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

#pragma once
#include "UpdateCommon.h"
#include <ConfigFile.h>


extern UPDATE_API const CString UPDATE_CURRENT_VERSION;
extern UPDATE_API const CString UPDATE_INFO_URL;


class UPDATE_API CUpdateInfo : public CConfigBase
{
public:
	class FileInfo
	{
	public:
		CString dir;		// 所在文件夹
		CString name;		// 文件名
		CString url;		// 下载地址
		CString md5;		// 比较MD5确定是否下载这个文件
	};

	COption<CString> m_version;				// 最新版本
	COption<CString> m_updateLog;			// 更新日志
	COption<std::vector<FileInfo> > m_files;		// 只储存必须的文件，如正则表达式手册不储存

	CUpdateInfo();
};


enum CheckUpdateResult { UPDATE_NO_UPDATE, UPDATE_HAS_UPDATE, UPDATE_FAILED_TO_GET_INFO };
// 检查更新，如果需要更新则开一个线程自动更新
UPDATE_API CheckUpdateResult CheckUpdate();
// 手动更新，打开一个URL
UPDATE_API void ManuallyUpdate();
