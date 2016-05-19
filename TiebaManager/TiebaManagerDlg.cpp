/*
Copyright (C) 2015  xfgryujk
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
#include <TBMEvent.h>
#include <TBMAPI.h>
#include "TiebaManager.h"

#include "SettingDlg.h"
#include "ExplorerDlg.h"
#include "PluginDlg.h"

#include "TBMConfigPath.h"
#include "TBMConfig.h"
#include "ConfigHelper.h"
#include <StringHelper.h>
#include <NetworkHelper.h>
#include <MiscHelper.h>
#include "Update.h"

#include <TiebaClawer.h>
#include <TiebaOperate.h>
#include <TBMScan.h>
#include <TBMOperate.h>

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
	ON_WM_GETMINMAXINFO()
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
	theApp.m_scan->m_log = &m_log;
	theApp.m_operate->m_log = &m_log;
	theApp.m_tbmApi->m_log = &m_log;

	// 读取设置
	theApp.m_globalConfig->Load(GLOBAL_CONFIG_PATH);
	SetCurrentUser(theApp.m_globalConfig->m_currentUser, FALSE);
	
	// 自动更新
	if (theApp.m_globalConfig->m_autoUpdate)
		AfxBeginThread(AutoUpdateThread, this);

	// 初次运行先看关于
	if (theApp.m_globalConfig->m_firstRun)
	{
		*theApp.m_globalConfig->m_firstRun = FALSE;
		*theApp.m_globalConfig->m_firstRunAfterUpdate = FALSE;
		theApp.m_globalConfig->Save(GLOBAL_CONFIG_PATH);
		OnBnClickedButton5();
		m_settingDlg->m_tab.SetCurSel(SETTING_DLG_PAGE_COUNT - 1);
		LRESULT tmp;
		m_settingDlg->OnTcnSelchangeTab1(NULL, &tmp);
	}
	else if (theApp.m_globalConfig->m_firstRunAfterUpdate) // 弹出更新日志
	{
		*theApp.m_globalConfig->m_firstRunAfterUpdate = FALSE;
		theApp.m_globalConfig->Save(GLOBAL_CONFIG_PATH);
		AfxMessageBox(UPDATE_LOG, MB_ICONINFORMATION);
	}


	// 每24小时清除已封名单
	SetTimer(0, 24 * 60 * 60 * 1000, [](HWND, UINT, UINT_PTR, DWORD) {
		theApp.m_userCache->m_bannedUser->clear();
	});

	// 每30分钟清除图片缓存
	SetTimer(1, 30 * 60 * 1000, [](HWND, UINT, UINT_PTR, DWORD) {
		CFileFind fileFind;
		BOOL flag = fileFind.FindFile(IMG_CACHE_PATH + _T("*"));
		while (flag)
		{
			flag = fileFind.FindNextFile();
			DeleteFile(fileFind.GetFilePath());
		}
	});


	theApp.m_tbmEventBus->Post(MainDialogPostInitEvent);


	// 测试
	/*vector<ThreadInfo> threads;
	GetThreads(_T("一个极其隐秘只有xfgryujk知道的地方"), _T("0"), threads);
	g_userTiebaInfo.m_forumID = _T("309740");
	vector<PostInfo> posts, lzls;
	GetPosts(_T("3033489261"), _T(""), _T("1"), posts, lzls);*/


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 保存储存在窗口的数据
void CTiebaManagerDlg::OnClose()
{
	if (m_isClosing)
		return;
	if (!theApp.m_tbmEventBus->Post(MainDialogCloseEvent))
		return;
	m_isClosing = TRUE;

	theApp.m_scan->StopScan();
	while (theApp.m_scan->IsScanning())
		Delay(100);

	theApp.m_tbmApi->m_log = NULL;
	if (theApp.m_plan->m_autoSaveLog)
		m_log.Save(_T("Log"));
	m_log.Release();

	CNormalDlg::OnClose();
}

// 保存其他数据、释放
void CTiebaManagerDlg::OnDestroy()
{
	CNormalDlg::OnDestroy();

	SaveCurrentUserConfig();
	theApp.m_globalConfig->Save(GLOBAL_CONFIG_PATH);
	theApp.m_plan->Save(OPTIONS_DIR_PATH + theApp.m_userConfig->m_plan + _T(".xml"));

	theApp.m_tbmEventBus->Post(MainDialogDestroyEvent);

	// 还是有内存泄漏，但我找不出了...
}

#pragma region UI
// 窗口 /////////////////////////////////////////////////////////////////////////////////

// 限制最小尺寸
void CTiebaManagerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	/*lpMMI->ptMinTrackSize.x = 705;
	lpMMI->ptMinTrackSize.y = 293;*/

	CNormalDlg::OnGetMinMaxInfo(lpMMI);
}

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

	CString prefix = url.Left(3);
	CTiebaOperate& tiebaOperate = *theApp.m_tiebaOperate;
	if (prefix == _T("dt:")) // 删主题
	{
		CString code = tiebaOperate.DeleteThread(url.Right(url.GetLength() - 3));
		if (code == _T("0"))
			m_log.Log(_T("<font color=green>删除成功！</font>"));
		else
			m_log.Log(_T("<font color=red>删除失败！</font>"));
	}
	else if (prefix == _T("dp:")) // 删帖子
	{
		CStringArray args;
		SplitString(args, url.Right(url.GetLength() - 3), _T(","));
		CString code = tiebaOperate.DeletePost(args[0], args[1]);
		if (code == _T("0"))
			m_log.Log(_T("<font color=green>删除成功！</font>"));
		else
			m_log.Log(_T("<font color=red>删除失败！</font>"));
	}
	else if (prefix == _T("dl:")) // 删楼中楼
	{
		CStringArray args;
		SplitString(args, url.Right(url.GetLength() - 3), _T(","));
		CString code = tiebaOperate.DeleteLZL(args[0], args[1]);
		if (code == _T("0"))
			m_log.Log(_T("<font color=green>删除成功！</font>"));
		else
			m_log.Log(_T("<font color=red>删除失败！</font>"));
	}
	else if (prefix == _T("bd:")) // 封ID
	{
		CStringArray args;
		SplitString(args, url.Right(url.GetLength() - 3), _T(","));
		CString code = (theApp.m_plan->m_wapBanInterface /*|| theApp.m_plan->m_banDuration == 1*/ || args[1] == _T("")) ? 
			tiebaOperate.BanIDClient(args[0]) : tiebaOperate.BanID(args[0], args[1]);
		if (code == _T("0"))
			m_log.Log(_T("<font color=green>封禁成功！</font>"));
		else
			m_log.Log(_T("<font color=red>封禁失败！</font>"));
	}
	else if (prefix == _T("df:")) // 拉黑
	{
		CString code = tiebaOperate.Defriend(url.Right(url.GetLength() - 3));
		if (code == _T("0"))
			m_log.Log(_T("<font color=green>拉黑成功！</font>"));
		else
			m_log.Log(_T("<font color=red>拉黑失败！</font>"));
	}
	else
		ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_NORMAL);
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
		m_pluginDlg = new CPluginDlg(m_pluginDlg, *theApp.m_pluginManager);
		m_pluginDlg->Create(m_pluginDlg->IDD, this);
	}
}

// 设置
void CTiebaManagerDlg::OnBnClickedButton5()
{
	if (m_settingDlg == NULL)
	{
		m_settingDlg = new CSettingDlg(m_settingDlg, m_log);
		m_settingDlg->Create(m_settingDlg->IDD, this);
	}
}
#pragma endregion

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

	if (!theApp.m_tbmEventBus->Post(PreSetTiebaEvent, CSetTiebaEvent(forumName)))
		return;

	m_forumNameEdit.EnableWindow(FALSE);
	m_confirmButton.EnableWindow(FALSE);
	m_stateStatic.SetWindowText(_T("验证贴吧中"));


	CTiebaOperate& tiebaOperate = *theApp.m_tiebaOperate;
	switch (tiebaOperate.SetTieba(forumName))
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

	SetWindowText(_T("贴吧管理器 - ") + tiebaOperate.GetUserName_());

	// 加入信任用户
	theApp.m_plan->m_whiteList->insert(tiebaOperate.GetUserName_());


	m_stateStatic.SetWindowText(_T("待机中"));
	m_startButton.EnableWindow(TRUE);
	m_pageEdit.EnableWindow(TRUE);
	m_explorerButton.EnableWindow(TRUE);

	*theApp.m_userConfig->m_forumName = tiebaOperate.GetForumName();
	theApp.m_userConfig->Save(USER_CONFIG_PATH);
	
	m_log.Log(_T("<font color=green>确认监控贴吧：</font>") + tiebaOperate.GetForumName()
		+ _T("<font color=green> 吧，使用账号：</font>" + tiebaOperate.GetUserName_()));

	theApp.m_tbmEventBus->Post(PostSetTiebaEvent, CSetTiebaEvent(forumName));
	return;

Error:
	m_stateStatic.SetWindowText(_T("待机中"));
	m_forumNameEdit.EnableWindow(TRUE);
	m_confirmButton.EnableWindow(TRUE);
}

// 开始
void CTiebaManagerDlg::OnBnClickedButton2()
{
	if (theApp.m_plan->m_keywords->empty() && theApp.m_plan->m_images.empty() && theApp.m_plan->m_blackList->empty() && theApp.m_plan->m_illegalLevel <= 0)
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

	theApp.m_scan->StartScan(tmp);
}

// 停止
void CTiebaManagerDlg::OnBnClickedButton3()
{
	theApp.m_scan->StopScan();
}
