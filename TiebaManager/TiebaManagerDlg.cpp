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

#include "stdafx.h"
#include "TiebaManagerDlg.h"
#include <TBMEvents.h>
#include <TBMAPI.h>
#include "TBMGlobal.h"

#include "SettingDlg.h"
#include "ExplorerDlg.h"
#include "PluginDlg.h"

#include <MiscHelper.h>
#include <Update.h>

#include "TBMConfigPath.h"
#include "ConfigHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 常量
static const UINT WM_TASKBARCREATED = RegisterWindowMessage(_T("TaskbarCreated"));
static const UINT WM_TRAY = WM_APP + 1;


// 构造函数
CTiebaManagerDlg::CTiebaManagerDlg(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CTiebaManagerDlg::IDD, pParent), 
	m_log(m_logExplorer)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_clearLogStatic.m_normalColor = m_saveLogStatic.m_normalColor = RGB(128, 128, 128);
	m_clearLogStatic.m_hoverColor = m_saveLogStatic.m_hoverColor = RGB(192, 192, 192);

	// 初始化托盘图标数据
	m_nfData.cbSize = sizeof(NOTIFYICONDATA);
	m_nfData.hIcon = AfxGetApp()->LoadIcon(IDI_TRAY);	
	_tcscpy_s(m_nfData.szTip, _T("贴吧管理器"));
	m_nfData.uCallbackMessage = WM_TRAY;
	m_nfData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
}

#pragma region MFC
void CTiebaManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, m_logExplorer);
	DDX_Control(pDX, IDC_EDIT1, m_forumNameEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_confirmButton);
	DDX_Control(pDX, IDC_BUTTON2, m_startButton);
	DDX_Control(pDX, IDC_BUTTON3, m_stopButton);
	DDX_Control(pDX, IDC_EDIT2, m_pageEdit);
	DDX_Control(pDX, IDC_BUTTON4, m_pluginButton);
	DDX_Control(pDX, IDC_BUTTON5, m_settingButton);
	DDX_Control(pDX, IDC_STATIC4, m_logStatic);
	DDX_Control(pDX, IDC_STATIC5, m_stateStatic);
	DDX_Control(pDX, IDC_STATIC6, m_clearLogStatic);
	DDX_Control(pDX, IDC_STATIC7, m_saveLogStatic);
	DDX_Control(pDX, IDC_BUTTON7, m_explorerButton);
	DDX_Control(pDX, IDC_LIST2, m_stateList);
}

BEGIN_MESSAGE_MAP(CTiebaManagerDlg, CNormalDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_TRAY, OnTray)
	ON_REGISTERED_MESSAGE(WM_TASKBARCREATED, OnTaskBarCreated)
	ON_BN_CLICKED(IDC_BUTTON1, &CTiebaManagerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTiebaManagerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTiebaManagerDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CTiebaManagerDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CTiebaManagerDlg::OnBnClickedButton5)
	ON_STN_CLICKED(IDC_STATIC6, &CTiebaManagerDlg::OnStnClickedStatic6)
	ON_STN_CLICKED(IDC_STATIC7, &CTiebaManagerDlg::OnStnClickedStatic7)
	ON_BN_CLICKED(IDC_BUTTON7, &CTiebaManagerDlg::OnBnClickedButton7)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CTiebaManagerDlg, CNormalDlg)
	ON_EVENT(CTiebaManagerDlg, IDC_EXPLORER1, 250, CTiebaManagerDlg::BeforeNavigate2Explorer1, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()


// CTiebaManagerDlg 消息处理程序


// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTiebaManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CNormalDlg::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTiebaManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
#pragma endregion

// 初始化
BOOL CTiebaManagerDlg::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_resize.AddControl(&m_explorerButton, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_pluginButton, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_settingButton, RT_KEEP_DIST_TO_RIGHT, this);
	m_resize.AddControl(&m_logStatic, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_logExplorer, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, &m_logStatic, RT_KEEP_DIST_TO_BOTTOM, &m_logStatic);
	m_resize.AddControl(&m_stateStatic, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_logStatic, RT_KEEP_DIST_TO_RIGHT, this, RT_NULL, NULL);
	m_resize.AddControl(&m_saveLogStatic, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_logStatic, RT_NULL, this, RT_NULL, NULL);
	m_resize.AddControl(&m_clearLogStatic, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_logStatic, RT_NULL, this, RT_NULL, NULL);
	m_resize.AddControl(&m_stateList, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, this, RT_KEEP_DIST_TO_RIGHT, this);

	m_pageEdit.SetWindowText(_T("1"));

	// 初始化托盘窗口句柄
	m_nfData.hWnd = m_hWnd;

	// 初始化日志
	m_log.Init();

	// 读取设置
	g_postSetCurrentUserEvent.AddListener([this](const CString&) {
		m_forumNameEdit.SetWindowText(*g_userConfig.m_forumName); // 显示贴吧名
	});
	g_globalConfig.Load(GLOBAL_CONFIG_PATH);
	SetCurrentUser(g_globalConfig.m_currentUser, FALSE);
	
	// 自动更新
	if (g_globalConfig.m_autoUpdate)
		std::thread(&CTiebaManagerDlg::AutoUpdateThread, this).detach();

	// 初次运行先看关于
	if (g_globalConfig.m_firstRun)
	{
		*g_globalConfig.m_firstRun = FALSE;
		g_globalConfig.Save(GLOBAL_CONFIG_PATH);
		OnBnClickedButton5();
		m_settingDlg->ShowAbout();
	}


	// 每24小时清除已封名单
	g_userCache.m_bannedUser->clear(); // 临时解决方案，相当于不保存已封名单
	SetTimer(0, 24 * 60 * 60 * 1000, [](HWND, UINT, UINT_PTR, DWORD) {
		g_userCache.m_bannedUser->clear();
	});

	// 每30分钟清除图片缓存
	SetTimer(1, 30 * 60 * 1000, [](HWND, UINT, UINT_PTR, DWORD) {
		CFileFind fileFind;
		BOOL found = fileFind.FindFile(IMG_CACHE_PATH + _T("*"));
		while (found)
		{
			found = fileFind.FindNextFile();
			DeleteFile(fileFind.GetFilePath());
		}
	});


	g_mainDialogPostInitEvent();


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 保存储存在窗口的数据
void CTiebaManagerDlg::OnClose()
{
	if (m_isClosing)
		return;
	BOOL pass = TRUE;
	g_mainDialogCloseEvent(pass);
	if (!pass)
		return;
	m_isClosing = TRUE;

	auto& scan = CTBMScan::GetInstance();
	scan.StopScan();
	while (scan.IsScanning())
		Delay(100);

	//theApp.m_tbmApi->m_log = NULL;
	if (g_plan.m_autoSaveLog)
		m_log.Save(_T("Log"));
	m_log.Release();

	CNormalDlg::OnClose();
}

// 保存其他数据、释放
void CTiebaManagerDlg::OnDestroy()
{
	CNormalDlg::OnDestroy();

	SaveCurrentUserConfig();
	g_globalConfig.Save(GLOBAL_CONFIG_PATH);
	g_plan.Save(OPTIONS_DIR_PATH + g_userConfig.m_plan + _T(".xml"));

	g_mainDialogDestroyEvent();

	// 还是有内存泄漏，但我找不出了...
}

#pragma region UI
// 托盘 /////////////////////////////////////////////////////////////////////////////////

// 最小化
void CTiebaManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_MINIMIZE) // 最小化时隐藏窗口
	{
		ShowWindow(SW_HIDE);
		Shell_NotifyIcon(NIM_ADD, &m_nfData);
		return;
	}

	CNormalDlg::OnSysCommand(nID, lParam);
}

// 托盘消息
LRESULT CTiebaManagerDlg::OnTray(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_LBUTTONDOWN) // 单击显示窗口
	{
		Shell_NotifyIcon(NIM_DELETE, &m_nfData);
		ShowWindow(SW_SHOW);
	}

	return 0;
}

// 任务栏重建
LRESULT CTiebaManagerDlg::OnTaskBarCreated(WPARAM wParam, LPARAM lParam)
{
	if (!IsWindowVisible())
		Shell_NotifyIcon(NIM_ADD, &m_nfData);
	return 0;
}
#pragma endregion

#pragma region 日志
// 日志浏览器将导航
void CTiebaManagerDlg::BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	CString url = _bstr_t(URL->bstrVal);
	if (url == _T("about:blank"))
		return;
	*Cancel = TRUE;

	BOOL pass = TRUE;
	g_openLinkInLogEvent(url, pass);
	if (pass)
		ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOWNORMAL);
}

// 清空日志
void CTiebaManagerDlg::OnStnClickedStatic6()
{
	m_log.Clear();
}

// 保存日志
void CTiebaManagerDlg::OnStnClickedStatic7()
{
	m_saveLogStatic.EnableWindow(FALSE);
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.lpszTitle = _T("要保存的目录：");
	bi.ulFlags = BIF_STATUSTEXT;
	LPITEMIDLIST pidlSel = SHBrowseForFolder(&bi);

	if (pidlSel != NULL)
	{
		CString folder;
		SHGetPathFromIDList(pidlSel, folder.GetBuffer(MAX_PATH));
		folder.ReleaseBuffer();

		m_log.Save(folder);
	}

	m_saveLogStatic.EnableWindow(TRUE);
}
#pragma endregion

#pragma region 打开其他对话框
// 查看帖子
void CTiebaManagerDlg::OnBnClickedButton7()
{
	if (m_explorerDlg == NULL)
	{
		m_explorerDlg = new CExplorerDlg(m_explorerDlg);
		m_explorerDlg->Create(m_explorerDlg->IDD, GetDesktopWindow());
	}
}

// 插件
void CTiebaManagerDlg::OnBnClickedButton4()
{
	if (m_pluginDlg == NULL)
	{
		m_pluginDlg = new CPluginDlg(m_pluginDlg);
		m_pluginDlg->Create(m_pluginDlg->IDD, GetDesktopWindow());
	}
}

// 设置
void CTiebaManagerDlg::OnBnClickedButton5()
{
	if (m_settingDlg == NULL)
	{
		m_settingDlg = new CSettingDlg(m_settingDlg);
		m_settingDlg->Create(m_settingDlg->IDD, this); // 父窗口为this，防止设置条件时操作条件列表
	}
}
#pragma endregion

// 更新 /////////////////////////////////////////////////////////////////////////////////

// 自动更新线程
void CTiebaManagerDlg::AutoUpdateThread()
{
	if (!CoInitializeHelper())
		return;

	CheckUpdateResult res = CheckUpdate();
	switch (res)
	{
	case UPDATE_FAILED_TO_GET_INFO:
		m_stateStatic.SetWindowText(_T("检查更新失败：获取文件信息失败，在设置里手动检查更新"));
		break;
	case UPDATE_NO_UPDATE:
	case UPDATE_HAS_UPDATE:
		m_stateStatic.SetWindowText(_T("待机中"));
		break;
	}

	CoUninitialize();
}

// 扫描 /////////////////////////////////////////////////////////////////////////////////

// 确定
void CTiebaManagerDlg::OnBnClickedButton1()
{
	CString forumName;
	m_forumNameEdit.GetWindowText(forumName);
	if (forumName == _T(""))
	{
		AfxMessageBox(_T("请输入贴吧名！"), MB_ICONERROR);
		return;
	}

	BOOL pass = TRUE;
	g_preSetTiebaEvent(forumName, pass);
	if (!pass)
		return;

	m_forumNameEdit.EnableWindow(FALSE);
	m_confirmButton.EnableWindow(FALSE);
	m_stateStatic.SetWindowText(_T("验证贴吧中"));


	switch (g_tiebaOperate.SetTieba(forumName))
	{
	case CTiebaOperate::SET_TIEBA_TIMEOUT:
		AfxMessageBox(_T("连接超时..."), MB_ICONERROR);
		goto Error;
	case CTiebaOperate::SET_TIEBA_NOT_FOUND:
		AfxMessageBox(_T("贴吧不存在！(也可能是度娘抽了...)"), MB_ICONERROR);
		goto Error;
	case CTiebaOperate::SET_TIEBA_NOT_LOGIN:
		AfxMessageBox(_T("请在设置-账号管理登录百度账号"), MB_ICONERROR);
		goto Error;
	case CTiebaOperate::SET_TIEBA_NO_POWER:
		AfxMessageBox(_T("您不是吧主或小吧主或语音小编，无法删帖封号！"), MB_ICONWARNING);
		break;
	case CTiebaOperate::SET_TIEBA_NO_TBS:
		AfxMessageBox(_T("获取口令号失败！"), MB_ICONERROR);
		goto Error;
	}

	SetWindowText(_T("贴吧管理器 - ") + g_tiebaOperate.GetUserName_());


	m_stateStatic.SetWindowText(_T("待机中"));
	m_startButton.EnableWindow(TRUE);
	m_pageEdit.EnableWindow(TRUE);
	m_explorerButton.EnableWindow(TRUE);

	*g_userConfig.m_forumName = g_tiebaOperate.GetForumName();
	g_userConfig.Save(USER_CONFIG_PATH);
	
	m_log.Log(_T("<font color=green>确认监控贴吧：</font>") + g_tiebaOperate.GetForumName()
		+ _T("<font color=green> 吧，使用账号：</font>" + g_tiebaOperate.GetUserName_()));

	g_postSetTiebaEvent(forumName);
	return;

Error:
	m_stateStatic.SetWindowText(_T("待机中"));
	m_forumNameEdit.EnableWindow(TRUE);
	m_confirmButton.EnableWindow(TRUE);
}

// 开始
void CTiebaManagerDlg::OnBnClickedButton2()
{
	if (g_plan.m_illegalRules->empty())
	{
		AfxMessageBox(_T("至少设置一个违规规则！"), MB_ICONERROR);
		OnBnClickedButton5();
		return;
	}

	CString tmp;
	m_pageEdit.GetWindowText(tmp);
	if (_ttoi(tmp) < 1)
	{
		m_pageEdit.SetWindowText(_T("1"));
		tmp = _T("1");
	}

	CTBMScan::GetInstance().StartScan(tmp);
}

// 停止
void CTiebaManagerDlg::OnBnClickedButton3()
{
	CTBMScan::GetInstance().StopScan();
}
