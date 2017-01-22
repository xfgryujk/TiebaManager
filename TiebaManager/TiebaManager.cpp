/*
Copyright (C) 2011-2017  xfgryujk
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


// TiebaManager.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include <vld.h> // Visual Leak Detector，检测内存泄漏的，可以不要

#include "TiebaManager.h"
#include "TiebaManagerDlg.h"
#include <Dbghelp.h>
#include "TBMConfigPath.h"

#include "PluginManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的一个 CTiebaManagerApp 对象
CTiebaManagerApp theApp;


// CTiebaManagerApp

BEGIN_MESSAGE_MAP(CTiebaManagerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTiebaManagerApp 构造
CTiebaManagerApp::CTiebaManagerApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// CTiebaManagerApp 初始化
BOOL CTiebaManagerApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();


	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO:  应适当修改该字符串，
	// 例如修改为公司或组织名
	//SetRegistryKey(_T("应用程序向导生成的本地应用程序"));


	CTiebaManagerDlg dlg;
	m_pMainWnd = &dlg;
	m_log = &dlg.m_log;

	// 初始化
	Init();

	// 载入主窗口
	dlg.DoModal();
	m_pMainWnd = NULL;
	m_log = NULL;


	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

// 异常处理
LONG WINAPI CTiebaManagerApp::ExceptionHandler(_EXCEPTION_POINTERS* ExceptionInfo)
{
	CFile file;
	if (file.Open(_T("exception.dmp"), CFile::modeCreate | CFile::modeWrite))
	{
		MINIDUMP_EXCEPTION_INFORMATION einfo;
		einfo.ThreadId = GetCurrentThreadId();
		einfo.ExceptionPointers = ExceptionInfo;
		einfo.ClientPointers = FALSE;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpWithIndirectlyReferencedMemory,
			&einfo, NULL, NULL);
	}
	AfxMessageBox(_T("程序崩溃了，请把exception.dmp文件发到xfgryujk@126.com帮助调试"), MB_ICONERROR);
	return EXCEPTION_EXECUTE_HANDLER;
}

// 初始化
void CTiebaManagerApp::Init()
{
	// 添加异常处理
	SetUnhandledExceptionFilter(ExceptionHandler);

	// 本地化输出，用于CStdioFile
	_tsetlocale(LC_ALL, _T(".936"));

	// 初始化配置文件路径
	CString cd;
	cd.ReleaseBuffer(GetCurrentDirectory(MAX_PATH, cd.GetBuffer(MAX_PATH)));
	GLOBAL_CONFIG_PATH = cd + GLOBAL_CONFIG_PATH;
	USERS_DIR_PATH = cd + USERS_DIR_PATH;

	// 判断一下有没有解压
	CString tmpDir;
	tmpDir.ReleaseBuffer(GetEnvironmentVariable(_T("TMP"), tmpDir.GetBuffer(MAX_PATH), MAX_PATH));
	if (cd.Find(tmpDir) != -1)
		AfxMessageBox(_T("请先解压再运行，否则无法保存设置"), MB_ICONINFORMATION);

	// 初始化各模块 ////////////////////////////////////////////////////////////////

	// 各种设置
	m_globalConfig = std::make_unique<CGlobalConfig>();
	m_userConfig = std::make_unique<CUserConfig>();
	m_cookieConfig = std::make_unique<CCookieConfig>();
	m_plan = std::make_unique<CPlan>();
	m_userCache = std::make_unique<CUserCache>();

	// 外部模块
	m_tiebaOperate = std::make_unique<CTiebaOperate>(m_cookieConfig->m_cookie, m_plan->m_banDuration, m_plan->m_banReason);
	m_operate = std::make_unique<CTBMOperate>(m_plan.get(), m_userCache.get(), m_tiebaOperate.get(), m_log);
	m_scan = std::make_unique<CTBMScan>(m_plan.get(), m_userCache.get(), m_operate.get(), m_log);

	// 内部模块
	m_configHelper = std::make_unique<CConfigHelper>();
	m_scanImage = std::make_unique<CScanImage>();

	// 内部Listeners
	CTBMListeners::GetInstance();

	// 插件
	CPluginManager::GetInstance();
}

// 释放
int CTiebaManagerApp::ExitInstance()
{
	TRACE(_T("释放m_pluginManager\n"));
	CPluginManager::GetInstance().Uninit();
	TRACE(_T("释放m_scan\n"));
	m_scan = nullptr;
	TRACE(_T("释放m_operate\n"));
	m_operate = nullptr;
	TRACE(_T("释放m_plan\n"));
	m_plan = nullptr;
	TRACE(_T("退出程序\n"));

	return CWinApp::ExitInstance();
}
