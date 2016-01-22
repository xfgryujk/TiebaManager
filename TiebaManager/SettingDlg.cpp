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
#include "Setting.h"
#include "TiebaVariable.h"
#include "ScanImage.h"
#include "MiscHelper.h"


// CSettingDlg 对话框

IMPLEMENT_DYNAMIC(CSettingDlg, CNormalDlg)

// 构造函数
CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CSettingDlg::IDD, pParent),
	m_pagesResize(&m_tab)
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
	m_whiteListPage.m_contentStatic.SetWindowText(_T("用户名："));
	m_whiteListPage.m_static.SetWindowText(_T("当被测文本等于文本时匹配(无正则)"));
	CREATE_PAGE(m_whiteContentPage);
	CREATE_PAGE(m_trustedThreadPage);
	m_trustedThreadPage.m_contentStatic.SetWindowText(_T("主题ID："));
	m_trustedThreadPage.m_edit.ModifyStyle(NULL, ES_NUMBER);
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
	ShowCurrentOptions();
	m_clearScanCache = FALSE; // 在m_scanPage.m_scanPageCountEdit.SetWindowText后初始化

	m_optionsPage.m_currentOptionStatic.SetWindowText(_T("当前方案：") + g_currentOption); // 当前方案
	// 方案
	CFileFind fileFind;
	BOOL flag = fileFind.FindFile(OPTIONS_PATH + _T("*.tb"));
	while (flag)
	{
		flag = fileFind.FindNextFile();
		m_optionsPage.m_list.AddString(fileFind.GetFileTitle());
	}

	m_usersPage.m_currentUserStatic.SetWindowText(_T("当前账号：") + g_currentUser); // 当前账号
	// 账号
	m_usersPage.m_list.AddString(_T("[NULL]"));
	flag = fileFind.FindFile(USERS_PATH + _T("*"));
	while (flag)
	{
		flag = fileFind.FindNextFile();
		if (fileFind.IsDirectory() && !fileFind.IsDots() && PathFileExists(fileFind.GetFilePath() + _T("\\ck.tb")))
		{
			CString name = fileFind.GetFileName();
			if (name != _T("[NULL]"))
				m_usersPage.m_list.AddString(name);
		}
	}

	m_aboutPage.m_autoCheckUpdateCheck.SetCheck(g_autoUpdate); // 自动更新

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
	lpMMI->ptMinTrackSize.x = 666;
	lpMMI->ptMinTrackSize.y = 576;

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
void CSettingDlg::ShowCurrentOptions()
{
	CString tmp;
	tmp.Format(_T("%d"), g_scanInterval);
	m_scanPage.m_scanIntervalEdit.SetWindowText(tmp);			// 扫描间隔
	m_operatePage.m_banIDCheck.SetCheck(g_banID);				// 封ID
	m_operatePage.OnBnClickedCheck1();
	m_operatePage.m_banDurationCombo.SetCurSel(g_banDuration == 1 ? 0 : (g_banDuration == 3 ? 1 : 2)); // 封禁时长
	tmp.Format(_T("%d"), g_banTrigCount);
	m_operatePage.m_banTrigCountEdit.SetWindowText(tmp);		// 封禁违规次数
	m_scanPage.m_onlyScanTitleCheck.SetCheck(g_onlyScanTitle);	// 只扫描标题
	tmp.Format(_T("%g"), g_deleteInterval);
	m_operatePage.m_deleteIntervalEdit.SetWindowText(tmp);		// 删帖间隔
	m_operatePage.m_confirmCheck.SetCheck(g_confirm);			// 操作前提示
	tmp.Format(_T("%d"), g_scanPageCount);
	m_scanPage.m_scanPageCountEdit.SetWindowText(tmp);			// 扫描最后页数
	m_scanPage.m_briefLogCheck.SetCheck(g_briefLog);			// 只输出删帖封号
	m_operatePage.m_deleteCheck.SetCheck(g_delete);				// 删帖
	tmp.Format(_T("%d"), g_threadCount);
	m_scanPage.m_threadCountEdit.SetWindowText(tmp);			// 线程数
	m_operatePage.m_banReasonEdit.SetWindowText(g_banReason);	// 封禁原因
	m_imagePage.m_dirEdit.SetWindowText(g_imageDir);			// 违规图片目录
	tmp.Format(_T("%lf"), g_SSIMThreshold);
	m_imagePage.m_thresholdEdit.SetWindowText(tmp);				// 阈值
	m_operatePage.m_defriendCheck.SetCheck(g_defriend);			// 拉黑
	m_operatePage.OnBnClickedCheck3();
	tmp.Format(_T("%d"), g_defriendTrigCount);
	m_operatePage.m_defriendTrigCountEdit.SetWindowText(tmp);	// 拉黑违规次数
	m_scanPage.m_autoSaveLogCheck.SetCheck(g_autoSaveLog);		// 自动保存日志

	// 违规内容
	m_keywordsPage.m_list.ResetContent();
	for (const RegexText& i : g_keywords)
		m_keywordsPage.m_list.AddString((i.isRegex ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + i.text);

	// 屏蔽用户
	m_blackListPage.m_list.ResetContent();
	for (const RegexText& i : g_blackList)
		m_blackListPage.m_list.AddString((i.isRegex ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + i.text);

	// 信任用户
	m_whiteListPage.m_list.ResetContent();
	for (const CString& i : g_whiteList)
		m_whiteListPage.m_list.AddString(i);

	// 信任内容
	m_whiteContentPage.m_list.ResetContent();
	for (const RegexText& i : g_whiteContent)
		m_whiteContentPage.m_list.AddString((i.isRegex ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + i.text);

	// 信任主题
	m_trustedThreadPage.m_list.ResetContent();
	for (const CString& i : g_trustedThread)
		m_trustedThreadPage.m_list.AddString(i);
}

static inline void ApplyRegexTexts(vector<RegexText>& vec, CListBox& list)
{
	int size = list.GetCount();
	vec.resize(size);
	CString tmp;
	for (int i = 0; i < size; i++)
	{
		list.GetText(i, tmp);
		vec[i].isRegex = tmp.Left(REGEX_PREFIX_LENGTH) == IS_REGEX_PREFIX;
		vec[i].text = tmp.Right(tmp.GetLength() - REGEX_PREFIX_LENGTH);
		vec[i].regexp = vec[i].isRegex ? vec[i].text : _T("");
	}
}

// 应用对话框中的设置
void CSettingDlg::ApplyOptionsInDlg()
{
	CString strBuf;
	int intBuf;
	g_optionsLock.Lock();

	m_scanPage.m_scanIntervalEdit.GetWindowText(strBuf);
	g_scanInterval = _ttoi(strBuf);								// 扫描间隔
	g_banID = m_operatePage.m_banIDCheck.GetCheck();			// 封ID
	intBuf = m_operatePage.m_banDurationCombo.GetCurSel();
	g_banDuration = intBuf == 0 ? 1 : (intBuf == 1 ? 3 : 10);	// 封禁时长
	m_operatePage.m_banTrigCountEdit.GetWindowText(strBuf);
	g_banTrigCount = _ttoi(strBuf);								// 封禁违规次数
	g_onlyScanTitle = m_scanPage.m_onlyScanTitleCheck.GetCheck(); // 只扫描标题
	m_operatePage.m_deleteIntervalEdit.GetWindowText(strBuf);
	g_deleteInterval = (float)_ttof(strBuf);					// 删帖间隔
	g_confirm = m_operatePage.m_confirmCheck.GetCheck();		// 操作前提示
	m_scanPage.m_scanPageCountEdit.GetWindowText(strBuf);
	g_scanPageCount = _ttoi(strBuf);							// 扫描最后页数
	g_briefLog = m_scanPage.m_briefLogCheck.GetCheck();			// 只输出删帖封号
	g_delete = m_operatePage.m_deleteCheck.GetCheck();			// 删帖
	m_scanPage.m_threadCountEdit.GetWindowText(strBuf);
	g_threadCount = _ttoi(strBuf);								// 线程数
	m_operatePage.m_banReasonEdit.GetWindowText(strBuf);
	g_banReason = strBuf;										// 封禁原因
	m_imagePage.m_dirEdit.GetWindowText(g_imageDir);			// 违规图片目录
	m_imagePage.m_thresholdEdit.GetWindowText(strBuf);
	g_SSIMThreshold = _ttof(strBuf);							// 阈值
	g_defriend = m_operatePage.m_defriendCheck.GetCheck();		// 拉黑
	m_operatePage.m_defriendTrigCountEdit.GetWindowText(strBuf);
	g_defriendTrigCount = _ttoi(strBuf);						// 拉黑违规次数
	g_autoSaveLog = m_scanPage.m_autoSaveLogCheck.GetCheck();	// 自动保存日志

	// 违规内容
	ApplyRegexTexts(g_keywords, m_keywordsPage.m_list);

	// 屏蔽用户
	ApplyRegexTexts(g_blackList, m_blackListPage.m_list);

	// 信任用户
	int size = m_whiteListPage.m_list.GetCount();
	g_whiteList.clear();
	for (int i = 0; i < size; i++)
	{
		m_whiteListPage.m_list.GetText(i, strBuf);
		g_whiteList.insert(strBuf);
	}

	// 信任内容
	ApplyRegexTexts(g_whiteContent, m_whiteContentPage.m_list);

	// 信任主题
	size = m_trustedThreadPage.m_list.GetCount();
	g_trustedThread.clear();
	for (int i = 0; i < size; i++)
	{
		m_trustedThreadPage.m_list.GetText(i, strBuf);
		g_trustedThread.insert(strBuf);
	}

	// 违规图片
	if (m_imagePage.m_updateImage)
	{
		g_leagalImage.clear();
		g_illegalImage.clear();
		ReadImages(g_imageDir);
	}

	g_optionsLock.Unlock();

	if (m_clearScanCache)
	{
		if (!g_briefLog)
			((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_log.Log(_T("<font color=green>清除历史回复</font>"));
		g_userCache.m_reply->clear();
	}
}

static inline void ReadRegexTexts(const gzFile& f, CListBox& list)
{
	int size;
	gzread(f, &size, sizeof(int)); // 长度
	list.ResetContent();
	CString strBuf;
	for (int i = 0; i < size; i++)
	{
		int isRegex;
		gzread(f, &isRegex, sizeof(int)); // 是正则
		ReadText(f, strBuf);
		list.AddString((isRegex != 0 ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + strBuf);
	}
}

static inline void WriteRegexTexts(const gzFile& f, CListBox& list)
{
	int size;
	gzwrite(f, &(size = list.GetCount()), sizeof(int)); // 长度
	CString strBuf;
	for (int i = 0; i < size; i++)
	{
		list.GetText(i, strBuf);
		BOOL isRegex = strBuf.Left(REGEX_PREFIX_LENGTH) == IS_REGEX_PREFIX;
		int intBuf;
		gzwrite(f, &(intBuf = isRegex ? 1 : 0), sizeof(int)); // 是正则
		strBuf = strBuf.Right(strBuf.GetLength() - REGEX_PREFIX_LENGTH);
		WriteText(f, strBuf);
	}
}

// 显示文件中的设置
void CSettingDlg::ShowOptionsInFile(LPCTSTR path)
{
	CString strBuf;

	gzFile f = gzopen_w(path, "rb");
	if (f == NULL)
		goto UseDefaultOptions;

	// 头部
	char header[2];
	gzread(f, header, sizeof(header));
	if (header[0] != 'T' || header[1] != 'B')
	{
		gzclose(f);
		goto UseDefaultOptions;
	}

	// 违规内容
	ReadRegexTexts(f, m_keywordsPage.m_list);

	// 屏蔽用户
	ReadRegexTexts(f, m_blackListPage.m_list);

	// 信任用户
	int size;
	gzread(f, &size, sizeof(int)); // 长度
	m_whiteListPage.m_list.ResetContent();
	int intBuf;
	for (int i = 0; i < size; i++)
	{
		ReadText(f, strBuf);
		m_whiteListPage.m_list.AddString(strBuf);
	}

	// 信任内容
	ReadRegexTexts(f, m_whiteContentPage.m_list);

	// 违规图片
	m_imagePage.m_updateImage = TRUE;

	BOOL boolBuf;
	float floatBuf;
	double doubleBuf;
	gzread(f, &intBuf, sizeof(int));						// 扫描间隔
	strBuf.Format(_T("%d"), intBuf);
	m_scanPage.m_scanIntervalEdit.SetWindowText(strBuf);
	gzread(f, &boolBuf, sizeof(BOOL));						// 封ID
	m_operatePage.m_banIDCheck.SetCheck(boolBuf);
	m_operatePage.OnBnClickedCheck1();
	gzread(f, &intBuf, sizeof(int));						// 封禁时长
	m_operatePage.m_banDurationCombo.SetCurSel(intBuf == 1 ? 0 : (intBuf == 3 ? 1 : 2));
	gzread(f, &boolBuf, sizeof(BOOL));						// 封IP
	gzread(f, &intBuf, sizeof(int));						// 封禁违规次数
	strBuf.Format(_T("%d"), intBuf);
	m_operatePage.m_banTrigCountEdit.SetWindowText(strBuf);
	gzread(f, &boolBuf, sizeof(BOOL));						// 只扫描标题
	m_scanPage.m_onlyScanTitleCheck.SetCheck(boolBuf);
	gzread(f, &floatBuf, sizeof(float));					// 删帖间隔
	strBuf.Format(_T("%g"), floatBuf);
	m_operatePage.m_deleteIntervalEdit.SetWindowText(strBuf);
	gzread(f, &boolBuf, sizeof(BOOL));						// 操作前提示
	m_operatePage.m_confirmCheck.SetCheck(boolBuf);
	gzread(f, &intBuf, sizeof(int));						// 扫描最后页数
	strBuf.Format(_T("%d"), intBuf);
	m_scanPage.m_scanPageCountEdit.SetWindowText(strBuf);
	gzread(f, &boolBuf, sizeof(BOOL));						// 只输出删帖封号
	m_scanPage.m_briefLogCheck.SetCheck(boolBuf);
	if (gzread(f, &boolBuf, sizeof(BOOL)) == sizeof(BOOL))	// 删帖
		m_operatePage.m_deleteCheck.SetCheck(boolBuf);
	else
		m_operatePage.m_deleteCheck.SetCheck(TRUE);
	if (gzread(f, &intBuf, sizeof(int)) == sizeof(int))		// 线程数
	{
		strBuf.Format(_T("%d"), intBuf);
		m_scanPage.m_threadCountEdit.SetWindowText(strBuf);
	}
	else
		m_scanPage.m_threadCountEdit.SetWindowText(_T("2"));
	ReadText(f, strBuf);									// 封禁原因
	m_operatePage.m_banReasonEdit.SetWindowText(strBuf);
	ReadText(f, strBuf);									// 违规图片目录
	m_imagePage.m_dirEdit.SetWindowText(strBuf);
	if (gzread(f, &doubleBuf, sizeof(double)) == sizeof(double))	// 阈值
	{
		strBuf.Format(_T("%lf"), doubleBuf);
		m_imagePage.m_thresholdEdit.SetWindowText(strBuf);
	}
	else
		m_imagePage.m_thresholdEdit.SetWindowText(_T("2.43"));

	// 信任主题
	m_trustedThreadPage.m_list.ResetContent();
	if (gzread(f, &size, sizeof(int)) == sizeof(int)) // 长度
		for (int i = 0; i < size; i++)
		{
			ReadText(f, strBuf);
			m_trustedThreadPage.m_list.AddString(strBuf);
		}

	gzread(f, &boolBuf, sizeof(BOOL));						// 拉黑
	m_operatePage.m_defriendCheck.SetCheck(boolBuf);
	m_operatePage.OnBnClickedCheck3();
	if (gzread(f, &intBuf, sizeof(int)) == sizeof(int))		// 拉黑违规次数
	{
		strBuf.Format(_T("%d"), intBuf);
		m_scanPage.m_threadCountEdit.SetWindowText(strBuf);
	}
	else
		m_scanPage.m_threadCountEdit.SetWindowText(_T("5"));
	if (gzread(f, &boolBuf, sizeof(BOOL)) == sizeof(BOOL))	// 自动保存日志
		m_scanPage.m_autoSaveLogCheck.SetCheck(boolBuf);
	else
		m_scanPage.m_autoSaveLogCheck.SetCheck(FALSE);

	gzclose(f);
	return;

UseDefaultOptions:
	m_keywordsPage.m_list.ResetContent();					// 违规内容
	m_blackListPage.m_list.ResetContent();					// 屏蔽用户
	m_whiteListPage.m_list.ResetContent();					// 信任用户
	m_whiteContentPage.m_list.ResetContent();				// 信任内容
	m_imagePage.m_updateImage = TRUE;						// 违规图片
	m_scanPage.m_scanIntervalEdit.SetWindowText(_T("5"));	// 扫描间隔
	m_operatePage.m_banIDCheck.SetCheck(FALSE);				// 封ID
	m_operatePage.OnBnClickedCheck1();
	m_operatePage.m_banDurationCombo.SetCurSel(0);			// 封禁时长
	m_operatePage.m_banTrigCountEdit.SetWindowText(_T("1"));	// 封禁违规次数
	m_scanPage.m_onlyScanTitleCheck.SetCheck(FALSE);		// 只扫描标题
	m_operatePage.m_deleteIntervalEdit.SetWindowText(_T("2"));	// 删帖间隔
	m_operatePage.m_confirmCheck.SetCheck(TRUE);			// 操作前提示
	m_scanPage.m_scanPageCountEdit.SetWindowText(_T("1"));	// 扫描最后页数
	m_scanPage.m_briefLogCheck.SetCheck(FALSE);				// 只输出删帖封号
	m_operatePage.m_deleteCheck.SetCheck(TRUE);				// 删帖
	m_scanPage.m_threadCountEdit.SetWindowText(_T("2"));	// 线程数
	m_operatePage.m_banReasonEdit.SetWindowText(_T(""));	// 封禁原因
	m_imagePage.m_dirEdit.SetWindowText(_T(""));			// 违规图片目录
	m_imagePage.m_thresholdEdit.SetWindowText(_T("2.43"));	// 阈值
	m_trustedThreadPage.m_list.ResetContent();				// 信任主题
	m_operatePage.m_defriendCheck.SetCheck(FALSE);			// 拉黑
	m_operatePage.OnBnClickedCheck3();
	m_scanPage.m_threadCountEdit.SetWindowText(_T("5"));	// 拉黑违规次数
	m_scanPage.m_autoSaveLogCheck.SetCheck(FALSE);			// 自动保存日志
}

// 把对话框中的设置写入文件
void CSettingDlg::SaveOptionsInDlg(LPCTSTR path)
{
	gzFile f = gzopen_w(path, "wb");
	if (f == NULL)
		return;

	// 头部
	gzwrite(f, "TB", 2);

	// 违规内容
	WriteRegexTexts(f, m_keywordsPage.m_list);

	// 屏蔽用户
	WriteRegexTexts(f, m_blackListPage.m_list);

	// 信任用户
	int size;
	gzwrite(f, &(size = m_whiteListPage.m_list.GetCount()), sizeof(int)); // 长度
	CString strBuf;
	for (int i = 0; i < size; i++)
	{
		m_whiteListPage.m_list.GetText(i, strBuf);
		WriteText(f, strBuf);
	}

	// 信任内容
	WriteRegexTexts(f, m_whiteContentPage.m_list);

	int intBuf;
	BOOL boolBuf;
	float floatBuf;
	double doubleBuf;
	m_scanPage.m_scanIntervalEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// 扫描间隔
	gzwrite(f, &(boolBuf = m_operatePage.m_banIDCheck.GetCheck()), sizeof(BOOL));	// 封ID
	intBuf = m_operatePage.m_banDurationCombo.GetCurSel();
	intBuf = intBuf == 0 ? 1 : (intBuf == 1 ? 3 : 10);
	gzwrite(f, &intBuf, sizeof(int));												// 封禁时长
	gzwrite(f, &(boolBuf = FALSE), sizeof(BOOL));									// 封IP
	m_operatePage.m_banTrigCountEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// 封禁违规次数
	gzwrite(f, &(boolBuf = m_scanPage.m_onlyScanTitleCheck.GetCheck()), sizeof(BOOL)); // 只扫描标题
	m_operatePage.m_deleteIntervalEdit.GetWindowText(strBuf);
	gzwrite(f, &(floatBuf = (float)_ttof(strBuf)), sizeof(float));					// 删帖间隔
	gzwrite(f, &(boolBuf = m_operatePage.m_confirmCheck.GetCheck()), sizeof(BOOL));	// 操作前提示
	m_scanPage.m_scanPageCountEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// 扫描最后页数
	gzwrite(f, &(boolBuf = m_scanPage.m_briefLogCheck.GetCheck()), sizeof(BOOL));	// 只输出删帖封号
	gzwrite(f, &(boolBuf = m_operatePage.m_deleteCheck.GetCheck()), sizeof(BOOL));	// 删帖
	m_scanPage.m_threadCountEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// 线程数
	m_operatePage.m_banReasonEdit.GetWindowText(strBuf);
	WriteText(f, strBuf);															// 封禁原因
	m_imagePage.m_dirEdit.GetWindowText(strBuf);
	WriteText(f, strBuf);															// 违规图片目录
	m_imagePage.m_thresholdEdit.GetWindowText(strBuf);
	gzwrite(f, &(doubleBuf = _ttof(strBuf)), sizeof(double));						// 阈值

	// 信任主题
	gzwrite(f, &(size = m_trustedThreadPage.m_list.GetCount()), sizeof(int)); // 长度
	for (int i = 0; i < size; i++)
	{
		m_trustedThreadPage.m_list.GetText(i, strBuf);
		WriteText(f, strBuf);
	}

	gzwrite(f, &(boolBuf = m_operatePage.m_defriendCheck.GetCheck()), sizeof(BOOL)); // 拉黑
	m_operatePage.m_defriendTrigCountEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// 拉黑违规次数
	gzwrite(f, &(boolBuf = m_scanPage.m_autoSaveLogCheck.GetCheck()), sizeof(BOOL)); // 自动保存日志
	gzclose(f);
}

// 确认
void CSettingDlg::OnOK()
{
	CString tmp;
	m_optionsPage.m_currentOptionStatic.GetWindowText(tmp);
	g_currentOption = tmp.Right(tmp.GetLength() - 5);
	CreateDir(OPTIONS_PATH);
	SaveOptionsInDlg(OPTIONS_PATH + g_currentOption + _T(".tb"));
	ApplyOptionsInDlg();
	WritePrivateProfileString(_T("Setting"), _T("Option"), g_currentOption, USER_PROFILE_PATH);
	WritePrivateProfileString(_T("Setting"), _T("AutoUpdate"), 
		m_aboutPage.m_autoCheckUpdateCheck.GetCheck() ? _T("1") : _T("0"), ALL_PROFILE_PATH);

	DestroyWindow();
}
