// LoopBanPage.cpp : 实现文件
//

#include "stdafx.h"
#include "LoopBanPage.h"

#include "StringHelper.h"
#include "NetworkHelper.h"
#include "Setting.h"

#include "TiebaCollect.h"
#include "TiebaOperate.h"

#include "SuperFunctionDlg.h"
#include "TiebaManagerDlg.h"


// CLoopBanPage 对话框

IMPLEMENT_DYNAMIC(CLoopBanPage, CListPage)

CLoopBanPage::CLoopBanPage(CWnd* pParent /*=NULL*/)
	: CListPage(CLoopBanPage::IDD, pParent)
{

}

#pragma region MFC
CLoopBanPage::~CLoopBanPage()
{
}

void CLoopBanPage::DoDataExchange(CDataExchange* pDX)
{
	CListPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK6, m_logCheck);
	DDX_Control(pDX, IDC_CHECK7, m_enableCheck);
}


BEGIN_MESSAGE_MAP(CLoopBanPage, CListPage)
END_MESSAGE_MAP()
#pragma endregion

// CLoopBanPage 消息处理程序

// 初始化
BOOL CLoopBanPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_resize.AddControl(&m_enableCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);
	m_resize.AddControl(&m_logCheck, RT_NULL, NULL, RT_KEEP_DIST_TO_BOTTOM, &m_list);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 添加
void CLoopBanPage::OnClickedButton1()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("内容不能为空！"), MB_ICONERROR);
		return;
	}

	CString pid = GetPIDFromUser(text);
	if (pid == NET_TIMEOUT_TEXT)
		pid = _T("");

	int index = m_list.GetCurSel();
	index = m_list.InsertString(index + 1, text);
	m_pid.insert(m_pid.begin() + index, pid);
	m_list.SetCurSel(index);

	((CSuperFunctionDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

// 删除
void CLoopBanPage::OnClickedButton2()
{
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;
	m_list.DeleteString(index);
	m_pid.erase(m_pid.begin() + index);
	m_list.SetCurSel(index == 0 ? 0 : index - 1);
}

// 修改
void CLoopBanPage::OnClickedButton3()
{
	CString text;
	m_edit.GetWindowText(text);
	if (text == _T(""))
	{
		AfxMessageBox(_T("内容不能为空！"), MB_ICONERROR);
		return;
	}
	int index = m_list.GetCurSel();
	if (index == LB_ERR)
		return;

	CString pid = GetPIDFromUser(text);
	if (pid == NET_TIMEOUT_TEXT)
		pid = _T("");

	m_list.DeleteString(index);
	index = m_list.InsertString(index, text);
	m_pid.insert(m_pid.begin() + index, pid);
	m_list.SetCurSel(index);

	((CSuperFunctionDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

// 循环封线程
UINT AFX_CDECL LoopBanThread(LPVOID _dlg)
{
	CTiebaManagerDlg* dlg = (CTiebaManagerDlg*)_dlg;

	// 一天内循环封过不再封
	SYSTEMTIME time;
	GetLocalTime(&time);
	CFile file;
	if (file.Open(CURRENT_USER_PATH + _T("\\LoopBanDate.tb"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite))
	{
		SYSTEMTIME lastTime;
		file.Read(&lastTime, sizeof(lastTime));
		if (time.wDay == lastTime.wDay && time.wMonth == lastTime.wMonth && time.wYear == lastTime.wYear)
			return 0;
	}

	gzFile f = gzopen_w(CURRENT_USER_PATH + _T("\\options2.tb"), "rb");
	if (f == NULL)
		return 0;

	// 头部
	char header[2];
	gzread(f, header, sizeof(header));
	if (header[0] != 'T' || header[1] != 'B')
	{
		gzclose(f);
		return 0;
	}

	// 读取名单、设置
	int size;
	gzread(f, &size, sizeof(int)); // 长度
	vector<CString> name(size), pid(size);
	for (int i = 0; i < size; i++)
	{
		ReadText(f, name[i]);
		ReadText(f, pid[i]);
	}
	BOOL log = FALSE, enable;
	gzread(f, &log, sizeof(BOOL)); // 输出日志
	if (gzread(f, &enable, sizeof(BOOL)) != sizeof(BOOL)) // 开启
		enable = TRUE;
	gzclose(f);
	if (!enable)
		return 0;

	// 更新时间
	if (file.m_hFile != NULL)
	{
		file.SeekToBegin();
		file.Write(&time, sizeof(time));
		file.Close();
	}

	BOOL updatePID = FALSE;
	// 循环封
	dlg->m_stateStatic.SetWindowText(_T("循环封禁中"));
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	for (int i = 0; i < size; i++)
	{
		CString code;
		if (pid[i] != _T("")) // 尝试用PID封禁
			code = BanID(name[i], pid[i]);
		if (pid[i] == _T("") || code != _T("0")) // 尝试不用PID封禁（用户必须为本吧会员）
		{
			code = BanID(name[i]);
			if (code != _T("0")) // 尝试获取新的PID并用PID封禁
			{
				pid[i] = GetPIDFromUser(name[i]);
				updatePID = TRUE;
				code = BanID(name[i], pid[i]);
			}
		}

		if (log)
		{
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("<font color=red>封禁 </font>%s<font color=red> 失败！错误代码：%s(%s)</font><a href=")
					_T("\"bd:%s,%s\">重试</a>"), name[i], code, GetTiebaErrorText(code), pid[i], name[i]);
				dlg->m_log.Log(content);
			}
			else
				dlg->m_log.Log(_T("<font color=red>封禁 </font>") + name[i]);
		}

		if (code == _T("0") && i < size - 1)
			Sleep(3000);
	}
	CoUninitialize();

	// 更新PID
	if (updatePID)
	{
		f = gzopen_w(CURRENT_USER_PATH + _T("\\options2.tb"), "wb");
		if (f == NULL)
		{
			dlg->m_stateStatic.SetWindowText(_T("待机中"));
			return 0;
		}

		// 头部
		gzwrite(f, "TB", 2);

		// 循环封
		int size;
		gzwrite(f, &(size = (int)name.size()), sizeof(int)); // 长度
		for (int i = 0; i < size; i++)
		{
			WriteText(f, name[i]);
			WriteText(f, pid[i]);
		}

		gzwrite(f, &log, sizeof(BOOL));		// 输出日志
		gzwrite(f, &enable, sizeof(BOOL));	// 开启

		gzclose(f);
	}

	dlg->m_stateStatic.SetWindowText(_T("待机中"));
	return 0;
}
