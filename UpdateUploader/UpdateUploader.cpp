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

// UpdateUploader.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "UpdateUploader.h"

#include <NetworkHelper.h>
#include <MiscHelper.h>
#include <Md5.h>

#include <Update.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static const CString UPDATE_INFO_PATH = _T("..\\UpdateInfo.xml");
static const CString REMOTE_ROOT = _T("https://sinacloud.net/xfgryujk/TiebaManager/");

static void Upload();


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// 初始化 MFC 并在失败时显示错误
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO:  更改错误代码以符合您的需要
			_tprintf(_T("错误:  MFC 初始化失败\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO:  在此处为应用程序的行为编写代码。
			if (!CoInitializeHelper())
				return 1;
			Upload();
			CoUninitialize();
		}
	}
	else
	{
		// TODO:  更改错误代码以符合您的需要
		_tprintf(_T("错误:  GetModuleHandle 失败\n"));
		nRetCode = 1;
	}

	return nRetCode;
}


void Upload()
{
	// 必须的文件列表、更新日志，只能手动写
	CUpdateInfo updateInfo;
	if (!updateInfo.Load(UPDATE_INFO_PATH))
	{
		puts("获取本地更新信息失败！");
		return;
	}

	// 取远程更新信息
	unique_ptr<BYTE[]> buffer;
	ULONG size;
	if (HTTPGetRaw(UPDATE_INFO_URL, &buffer, &size) != NET_SUCCESS) // 用HTTPGetRaw防止转码
	{
		puts("取远程更新信息失败！");
		return;
	}
	CUpdateInfo remoteUpdateInfo;
	if (!remoteUpdateInfo.LoadFromString((LPCSTR)buffer.get(), size))
	{
		puts("取远程更新信息失败！");
		return;
	}

	// 需要上传的文件，url是远程路径
	vector<CUpdateInfo::FileInfo> uploadFiles;

	// 设置更新信息
	*updateInfo.m_version = UPDATE_CURRENT_VERSION;
	CFile f;
	for (auto& fileInfo : *updateInfo.m_files)
	{
		// URL
		CString remotePath = fileInfo.dir;
		remotePath.Replace(_T('\\'), _T('/'));
		fileInfo.url = REMOTE_ROOT + remotePath + fileInfo.name;

		// MD5
		if (!f.Open(fileInfo.dir + fileInfo.name, CFile::modeRead))
		{
			puts("取MD5失败！");
			return;
		}
		fileInfo.md5 = GetMD5_File(f);
		f.Close();

		// 和远程信息比较决定是否上传
		const CUpdateInfo::FileInfo* remoteInfo = NULL;
		for (const auto& remoteInfo_ : *remoteUpdateInfo.m_files)
		{
			if (remoteInfo_.dir == fileInfo.dir && remoteInfo_.name == fileInfo.name)
				remoteInfo = &remoteInfo_;
		}
		if (remoteInfo == NULL || remoteInfo->md5 != fileInfo.md5)
		{
			uploadFiles.push_back(fileInfo);
			uploadFiles[uploadFiles.size() - 1].url = remotePath + fileInfo.name;
		}
	}
	if (!updateInfo.Save(_T("UpdateInfo.xml")))
	{
		puts("保存更新信息失败！");
		return;
	}
	uploadFiles.push_back(CUpdateInfo::FileInfo{ _T(""), _T("UpdateInfo.xml"), _T("UpdateInfo.xml"), _T("") });

	// 打包完整程序
	CString cd;
	GetCurrentDirectory(MAX_PATH, cd.GetBuffer(MAX_PATH));
	cd.ReleaseBuffer();
	SetCurrentDirectory(_T(".."));
	system("PackForRelease.bat");
	SetCurrentDirectory(cd);
	uploadFiles.push_back(CUpdateInfo::FileInfo{ _T("..\\"), _T("贴吧管理器.zip"), _T("贴吧管理器.zip"), _T("") });

	// 上传
	for (const auto& fileInfo : uploadFiles)
	{
		// 调用上传脚本上传文件，里面有秘钥所以不公开此文件
		_tsystem(_T("F:\\Anaconda3\\envs\\py2\\python.exe Upload.py \"") + fileInfo.dir + fileInfo.name + _T("\" \"") + fileInfo.url + _T("\""));
	}

	// 删除临时文件
	DeleteFile(_T("UpdateInfo.xml"));
	DeleteFile(_T("..\\贴吧管理器.zip"));

	system("pause");
}
