
// TiebaManager.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "TiebaManagerDlg.h"
#include <Dbghelp.h>
#include <TBMConfigPath.h>

#include <TBMConfig.h>
#include <TBMCoreConfig.h>
#include <TBMScan.h>
#include <TBMOperate.h>
#include "TBMScanListeners.h"
#include "TBMOperateListeners.h"

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


	// 初始化
	init();


	// 载入主窗口
	CTiebaManagerDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	m_pMainWnd = NULL;


	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

// 异常处理
static LONG WINAPI ExceptionHandler(_EXCEPTION_POINTERS* ExceptionInfo)
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
void CTiebaManagerApp::init()
{
	// 添加异常处理
	SetUnhandledExceptionFilter(ExceptionHandler);

	// 本地化输出
	_tsetlocale(LC_ALL, _T(".936"));

	// 初始化配置文件路径
	TCHAR cd[MAX_PATH] = { _T('\0') };
	GetCurrentDirectory(_countof(cd), cd);
#pragma warning(suppress: 6102)
	GLOBAL_CONFIG_PATH = cd + GLOBAL_CONFIG_PATH;
	USERS_DIR_PATH = cd + USERS_DIR_PATH;

	// 判断一下有没有解压
	TCHAR tmpDir[MAX_PATH] = { _T('\0') };
	if (GetEnvironmentVariable(_T("TMP"), tmpDir, _countof(tmpDir)) != 0
#pragma warning(suppress: 6102)
		&& StrStrI(cd, tmpDir) != NULL)
		AfxMessageBox(_T("请先解压再运行，否则无法保存设置"), MB_ICONINFORMATION);

	// 初始化各模块
	m_globalConfig.reset(new CGlobalConfig());
	m_userConfig.reset(new CUserConfig());
	m_cookieConfig.reset(new CCookieConfig());
	m_plan.reset(new CPlan());
	m_userCache.reset(new CUserCache());
	m_operate.reset(new CTBMOperate(m_cookieConfig->m_cookie, m_plan->m_banDuration, m_plan->m_banReason));
	m_operate->m_config = m_plan.get();
	m_operate->m_userCache = m_userCache.get();
	m_scan.reset(new CTBMScan());
	m_scan->m_config = m_plan.get();
	m_scan->m_userCache = m_userCache.get();
	m_scan->m_operate = m_operate.get();
	m_scanListeners.reset(new CTBMScanListeners(*m_scan));
	m_operateListeners.reset(new CTBMOperateListeners(*m_operate));
}

// 释放
int CTiebaManagerApp::ExitInstance()
{
	TRACE(_T("释放m_scan\n"));
	m_scan = nullptr;
	TRACE(_T("释放m_operate\n"));
	m_operate = nullptr;
	TRACE(_T("释放m_plan\n"));
	m_plan = nullptr;

	return CWinApp::ExitInstance();
}
