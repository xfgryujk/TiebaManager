
// TiebaManager.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "TiebaManagerDlg.h"
#include "MiscHelper.h"
#include "Setting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// 唯一的一个 CTiebaManagerApp 对象

CTiebaManagerApp theApp;


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


	// 添加异常处理
	SetUnhandledExceptionFilter(ExceptionHandler);

	// 本地化输出
	setlocale(LC_CTYPE, "chs");

	// 初始化配置文件路径
	TCHAR cd[MAX_PATH];
	GetCurrentDirectory(_countof(cd), cd);
	GLOBAL_CONFIG_PATH = cd + GLOBAL_CONFIG_PATH;
	USERS_PATH = cd + USERS_PATH;

	// 载入主窗口
	CTiebaManagerDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();


	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

