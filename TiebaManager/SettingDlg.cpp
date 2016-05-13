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
#include "SettingDlg.h"
#include "TiebaManagerDlg.h"
#include <TBMConfig.h>
#include "TiebaVariable.h"
#include "ScanImage.h"
#include <MiscHelper.h>


// CSettingDlg 对话框

IMPLEMENT_DYNAMIC(CSettingDlg, CNormalDlg)

// 构造函数
CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CSettingDlg::IDD, pParent),
	m_pagesResize(&m_tab), 
	m_whiteListPage(_T("用户名：")), 
	m_trustedThreadPage(_T("主题ID："))
{
	// 初始化m_pages
	int i = 0;
	m_pages[i++] = &m_scanPage;
	m_pages[i++] = &m_operatePage;
	m_pages[i++] = &m_keywordsPage;
	m_pages[i++] = &m_imagePage;
	m_pages[i++] = &m_blackListPage;
	m_pages[i++] = &m_whiteListPage;
	m_pages[i++] = &m_whiteContentPage;
	m_pages[i++] = &m_trustedThreadPage;
	m_pages[i++] = &m_optionsPage;
	m_pages[i++] = &m_usersPage;
	m_pages[i++] = &m_aboutPage;
}

#pragma region MFC
CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CNormalDlg)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CSettingDlg::OnTcnSelchangeTab1)
	ON_WM_CLOSE()
END_MESSAGE_MAP()
#pragma endregion

// CSettingDlg 消息处理程序

// 初始化
BOOL CSettingDlg::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// 设置大图标
	SetIcon(hIcon, FALSE);			// 设置小图标

	// 初始化m_tab
	int i = 0;
	m_tab.InsertItem(i++, _T("扫描"));
	m_tab.InsertItem(i++, _T("操作"));
	m_tab.InsertItem(i++, _T("违规内容"));
	m_tab.InsertItem(i++, _T("违规图片"));
	m_tab.InsertItem(i++, _T("屏蔽用户"));
	m_tab.InsertItem(i++, _T("信任用户"));
	m_tab.InsertItem(i++, _T("信任内容"));
	m_tab.InsertItem(i++, _T("信任主题"));
	m_tab.InsertItem(i++, _T("方案"));
	m_tab.InsertItem(i++, _T("账号管理"));
	m_tab.InsertItem(i++, _T("关于&&更新"));

	// 初始化各页
#define CREATE_PAGE(page) page.Create(page.IDD, &m_tab)
	CREATE_PAGE(m_scanPage);
	CREATE_PAGE(m_operatePage);
	CREATE_PAGE(m_keywordsPage);
	CREATE_PAGE(m_imagePage);
	CREATE_PAGE(m_blackListPage);
	CREATE_PAGE(m_whiteListPage);
	m_whiteListPage.m_static.SetWindowText(_T("当被测文本等于文本时匹配(无正则)"));
	CREATE_PAGE(m_whiteContentPage);
	CREATE_PAGE(m_trustedThreadPage);
	m_trustedThreadPage.m_static.SetWindowText(_T("添加的主题不会扫描，主题ID是网址中\"p/\"后面跟的数字"));
	CREATE_PAGE(m_optionsPage);
	CREATE_PAGE(m_usersPage);
	CREATE_PAGE(m_aboutPage);

	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	m_pages[0]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	for (i = 1; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_HIDEWINDOW);

	m_resize.AddControl(&m_tab, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_okButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_tab);
	m_resize.AddControl(&m_cancelButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_tab);
	for (i = 0; i < _countof(m_pages); i++)
		m_pagesResize.AddControl(m_pages[i], RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, &m_tab, RT_KEEP_DIST_TO_BOTTOM, &m_tab);

	// 显示配置
	ShowPlan(g_plan);
	m_clearScanCache = FALSE; // 在m_scanPage.m_scanPageCountEdit.SetWindowText后初始化

	m_optionsPage.m_currentOptionStatic.SetWindowText(_T("当前方案：") + g_userConfig.m_plan); // 当前方案
	// 方案
	CFileFind fileFind;
	BOOL flag = fileFind.FindFile(OPTIONS_DIR_PATH + _T("*.xml"));
	while (flag)
	{
		flag = fileFind.FindNextFile();
		m_optionsPage.m_list.AddString(fileFind.GetFileTitle());
	}

	m_usersPage.m_currentUserStatic.SetWindowText(_T("当前账号：") + g_globalConfig.m_currentUser); // 当前账号
	// 账号
	m_usersPage.m_list.AddString(_T("[NULL]"));
	flag = fileFind.FindFile(USERS_DIR_PATH + _T("*"));
	while (flag)
	{
		flag = fileFind.FindNextFile();
		if (fileFind.IsDirectory() && !fileFind.IsDots() 
			&& (PathFileExists(fileFind.GetFilePath() + _T("\\ck.xml"))))
		{
			CString name = fileFind.GetFileName();
			if (name != _T("[NULL]"))
				m_usersPage.m_list.AddString(name);
		}
	}

	m_aboutPage.m_autoCheckUpdateCheck.SetCheck(g_globalConfig.m_autoUpdate); // 自动更新

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

#pragma region UI
// 窗口 /////////////////////////////////////////////////////////////////////////////////

// 取消
void CSettingDlg::OnCancel()
{
	DestroyWindow();
}

// 提示是否保存
void CSettingDlg::OnClose()
{
	int result = AfxMessageBox(_T("保存设置？"), MB_ICONQUESTION | MB_YESNOCANCEL);
	if (result == IDYES)
	{
		OnOK();
		return;
	}
	else if (result == IDCANCEL)
		return;

	DestroyWindow();
}

// 释放this
void CSettingDlg::PostNcDestroy()
{
	CNormalDlg::PostNcDestroy();

	((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_settingDlg = NULL;
	delete this;
}

// 限制最小尺寸
void CSettingDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	/*lpMMI->ptMinTrackSize.x = 666;
	lpMMI->ptMinTrackSize.y = 576;*/

	CNormalDlg::OnGetMinMaxInfo(lpMMI);
}

// 改变尺寸
void CSettingDlg::OnSize(UINT nType, int cx, int cy)
{
	CNormalDlg::OnSize(nType, cx, cy);
	m_pagesResize.Resize();
}

// 切换标签
void CSettingDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	int index = m_tab.GetCurSel();
	for (int i = 0; i < _countof(m_pages); i++)
		m_pages[i]->ShowWindow(i == index ? SW_SHOW : SW_HIDE);
}
#pragma endregion

// 显示当前设置
void CSettingDlg::ShowPlan(const CPlan& plan)
{
	CString tmp;
	tmp.Format(_T("%d"), *plan.m_scanInterval);
	m_scanPage.m_scanIntervalEdit.SetWindowText(tmp);				// 扫描间隔
	m_operatePage.m_banIDCheck.SetCheck(plan.m_banID);				// 封ID
	m_operatePage.OnBnClickedCheck1();
	m_operatePage.m_banDurationCombo.SetCurSel(plan.m_banDuration == 1 ? 0 : (plan.m_banDuration == 3 ? 1 : 2)); // 封禁时长
	tmp.Format(_T("%d"), *plan.m_banTrigCount);
	m_operatePage.m_banTrigCountEdit.SetWindowText(tmp);			// 封禁违规次数
	m_scanPage.m_onlyScanTitleCheck.SetCheck(plan.m_onlyScanTitle);	// 只扫描标题
	tmp.Format(_T("%g"), *plan.m_deleteInterval);
	m_operatePage.m_deleteIntervalEdit.SetWindowText(tmp);			// 删帖间隔
	m_operatePage.m_confirmCheck.SetCheck(plan.m_confirm);			// 操作前提示
	m_operatePage.m_wapBanInterfaceCheck.SetCheck(plan.m_wapBanInterface);	// 封禁使用WAP接口
	m_operatePage.m_autoLoopBanCheck.SetCheck(plan.m_autoLoopBan);	// 自动循环封
	tmp.Format(_T("%d"), *plan.m_scanPageCount);
	m_scanPage.m_scanPageCountEdit.SetWindowText(tmp);				// 扫描最后页数
	m_scanPage.m_briefLogCheck.SetCheck(plan.m_briefLog);			// 只输出删帖封号
	m_operatePage.m_deleteCheck.SetCheck(plan.m_delete);			// 删帖
	tmp.Format(_T("%d"), *plan.m_threadCount);
	m_scanPage.m_threadCountEdit.SetWindowText(tmp);				// 线程数
	m_operatePage.m_banReasonEdit.SetWindowText(*plan.m_banReason);	// 封禁原因
	m_imagePage.m_dirEdit.SetWindowText(*plan.m_imageDir);			// 违规图片目录
	tmp.Format(_T("%g"), *plan.m_SSIMThreshold);
	m_imagePage.m_thresholdEdit.SetWindowText(tmp);					// 阈值
	m_operatePage.m_defriendCheck.SetCheck(plan.m_defriend);		// 拉黑
	m_operatePage.OnBnClickedCheck3();
	tmp.Format(_T("%d"), *plan.m_defriendTrigCount);
	m_operatePage.m_defriendTrigCountEdit.SetWindowText(tmp);		// 拉黑违规次数
	m_scanPage.m_autoSaveLogCheck.SetCheck(plan.m_autoSaveLog);		// 自动保存日志
	tmp.Format(_T("%d"), *plan.m_illegalLevel);
	m_scanPage.m_illegalLevelEdit.SetWindowText(tmp);				// 违规等级

	// 违规内容
	m_keywordsPage.ShowList(plan.m_keywords);

	// 屏蔽用户
	m_blackListPage.ShowList(plan.m_blackList);

	// 信任用户
	m_whiteListPage.ShowList(plan.m_whiteList);

	// 信任内容
	m_whiteContentPage.ShowList(plan.m_whiteContent);

	// 信任主题
	m_trustedThreadPage.ShowList(plan.m_trustedThread);
}

// 应用对话框中的设置
void CSettingDlg::ApplyPlanInDlg(CPlan& plan)
{
	CString strBuf;
	int intBuf;
	plan.OnChange();

	m_scanPage.m_scanIntervalEdit.GetWindowText(strBuf);
	*plan.m_scanInterval = _ttoi(strBuf);								// 扫描间隔
	*plan.m_banID = m_operatePage.m_banIDCheck.GetCheck();				// 封ID
	intBuf = m_operatePage.m_banDurationCombo.GetCurSel();
	*plan.m_banDuration = intBuf == 0 ? 1 : (intBuf == 1 ? 3 : 10);		// 封禁时长
	m_operatePage.m_banTrigCountEdit.GetWindowText(strBuf);
	*plan.m_banTrigCount = _ttoi(strBuf);								// 封禁违规次数
	*plan.m_onlyScanTitle = m_scanPage.m_onlyScanTitleCheck.GetCheck(); // 只扫描标题
	m_operatePage.m_deleteIntervalEdit.GetWindowText(strBuf);
	*plan.m_deleteInterval = (float)_ttof(strBuf);						// 删帖间隔
	*plan.m_confirm = m_operatePage.m_confirmCheck.GetCheck();			// 操作前提示
	*plan.m_wapBanInterface = m_operatePage.m_wapBanInterfaceCheck.GetCheck();	// 封禁使用WAP接口
	*plan.m_autoLoopBan = m_operatePage.m_autoLoopBanCheck.GetCheck();	// 自动循环封
	m_scanPage.m_scanPageCountEdit.GetWindowText(strBuf);
	*plan.m_scanPageCount = _ttoi(strBuf);								// 扫描最后页数
	*plan.m_briefLog = m_scanPage.m_briefLogCheck.GetCheck();			// 只输出删帖封号
	*plan.m_delete = m_operatePage.m_deleteCheck.GetCheck();			// 删帖
	m_scanPage.m_threadCountEdit.GetWindowText(strBuf);
	*plan.m_threadCount = _ttoi(strBuf);								// 线程数
	m_operatePage.m_banReasonEdit.GetWindowText(strBuf);
	*plan.m_banReason = strBuf;											// 封禁原因
	m_imagePage.m_dirEdit.GetWindowText(plan.m_imageDir);				// 违规图片目录
	m_imagePage.m_thresholdEdit.GetWindowText(strBuf);
	*plan.m_SSIMThreshold = _ttof(strBuf);								// 阈值
	*plan.m_defriend = m_operatePage.m_defriendCheck.GetCheck();		// 拉黑
	m_operatePage.m_defriendTrigCountEdit.GetWindowText(strBuf);
	*plan.m_defriendTrigCount = _ttoi(strBuf);							// 拉黑违规次数
	*plan.m_autoSaveLog = m_scanPage.m_autoSaveLogCheck.GetCheck();		// 自动保存日志
	m_scanPage.m_illegalLevelEdit.GetWindowText(strBuf);
	*plan.m_illegalLevel = _ttoi(strBuf);								// 违规等级

	// 违规内容
	m_keywordsPage.ApplyList(plan.m_keywords);

	// 屏蔽用户
	m_blackListPage.ApplyList(plan.m_blackList);

	// 信任用户
	m_whiteListPage.ApplyList(plan.m_whiteList);

	// 信任内容
	m_whiteContentPage.ApplyList(plan.m_whiteContent);

	// 信任主题
	m_trustedThreadPage.ApplyList(plan.m_trustedThread);

	// 违规图片
	BOOL updateImage = &plan == &g_plan && plan.m_updateImage;
	if (updateImage)
	{
		g_leagalImage.clear();
		g_illegalImage.clear();
	}

	plan.PostChange();

	if (updateImage)
	{
		strBuf.Format(_T("载入了%d张图片"), plan.m_images.size());
		AfxMessageBox(strBuf, MB_ICONINFORMATION);
	}

	if (&plan == &g_plan && m_clearScanCache)
	{
		if (!plan.m_briefLog)
			((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_log.Log(_T("<font color=green>清除历史回复</font>"));
		g_userCache.m_reply->clear();
	}
}

// 显示文件中的设置
void CSettingDlg::ShowPlanInFile(const CString& path)
{
	CPlan tmp;
	tmp.Load(path);
	g_plan.m_updateImage = TRUE;
	ShowPlan(tmp);
}

// 把对话框中的设置写入文件
void CSettingDlg::SavePlanInDlg(const CString& path)
{
	CPlan tmp;
	ApplyPlanInDlg(tmp);
	tmp.Save(path);
}

// 确认
void CSettingDlg::OnOK()
{
	*g_globalConfig.m_autoUpdate = m_aboutPage.m_autoCheckUpdateCheck.GetCheck();
	g_globalConfig.Save(GLOBAL_CONFIG_PATH);

	CString tmp;
	m_optionsPage.m_currentOptionStatic.GetWindowText(tmp);
	*g_userConfig.m_plan = tmp.Right(tmp.GetLength() - 5); // "当前方案："
	g_userConfig.Save(USER_CONFIG_PATH);

	CreateDir(OPTIONS_DIR_PATH);
	SavePlanInDlg(OPTIONS_DIR_PATH + g_userConfig.m_plan + _T(".xml"));
	ApplyPlanInDlg(g_plan);

	DestroyWindow();
}
