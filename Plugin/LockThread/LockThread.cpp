// LockThread.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "LockThread.h"
#include "resource.h"
#include "LockThreadDlg.h"

#include <MiscHelper.h>

#include <TBMAPI.h>
#include <TBMCoreConfig.h>
#include <TiebaClawer.h>
#include <TiebaOperate.h>
#include <TBMOperate.h>

#include <Mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CLockThread g_lockThread;


extern "C" __declspec(dllexport) bool __cdecl Init()
{
	return g_lockThread.Init();
}

extern "C" __declspec(dllexport) bool __cdecl Uninit()
{
	return g_lockThread.Uninit();
}

extern "C" __declspec(dllexport) LPCWSTR __cdecl GetDescription()
{
	return _T("锁帖插件\r\n")
		_T("\r\n")
		_T("作者：盗我原号的没J8");
}

extern "C" __declspec(dllexport) void __cdecl OnConfig()
{
	g_lockThread.OnConfig();
}


bool CLockThread::Init()
{
	return true;
}

bool CLockThread::Uninit()
{
	// 关闭窗口
	if (m_lockThreadDlg != NULL)
		m_lockThreadDlg->DestroyWindow();

	// 停止线程
	StopLockThread();
	if (m_lockThreadThread != nullptr && m_lockThreadThread->joinable())
		m_lockThreadThread->join();

	return true;
}

void CLockThread::OnConfig()
{
	if (!CTBMAPI::GetInstance().GetTiebaOperate()->HasSetTieba())
	{
		AfxMessageBox(_T("请先确认贴吧！"), MB_ICONERROR);
		return;
	}

	if (m_lockThreadDlg == NULL)
	{
		m_lockThreadDlg = new CLockThreadDlg(m_lockThreadDlg);
		m_lockThreadDlg->Create(m_lockThreadDlg->IDD, CWnd::GetDesktopWindow());
	}
}


void CLockThread::StartLockThread()
{
	m_stopFlag = FALSE;
	m_lockThreadThread.reset(new thread(&CLockThread::LockThreadThread, this));
}

void CLockThread::StopLockThread()
{
	m_stopFlag = TRUE;
}

void CLockThread::LockThreadThread()
{
	// 初始化
	if (!CoInitializeHelper())
		return;

	ILog& log = *CTBMAPI::GetInstance().GetLog();
	CTiebaOperate& tiebaOperate = *CTBMAPI::GetInstance().GetTiebaOperate();
	CTBMOperate& operate = *CTBMAPI::GetInstance().GetOperate();


	CString tid, page, floor;
	if (m_lockThreadDlg != NULL)
	{
		m_lockThreadDlg->m_tidEdit.GetWindowText(tid);
		m_lockThreadDlg->m_pageEdit.GetWindowText(page);
		m_lockThreadDlg->m_floorEdit.GetWindowText(floor);
		if (_ttoi(page) < 1)
		{
			page = _T("1");
			m_lockThreadDlg->m_pageEdit.SetWindowText(page);
		}
		if (_ttoi(floor) < 1)
		{
			floor = _T("1");
			m_lockThreadDlg->m_floorEdit.SetWindowText(page);
		}
	}
	int iFloor = _ttoi(floor);

	// 锁帖
	while (!m_stopFlag)
	{
		vector<PostInfo> posts;
		GetPosts(tid, _T(""), page, posts);

		for (const PostInfo& post : posts)
		if (_ttoi(post.floor) > iFloor)
		{
			CString code = tiebaOperate.DeletePost(tid, post.pid);
			if (code != _T("0"))
			{
				CString content;
				content.Format(_T("锁帖： %s楼<font color=red> 删除失败！错误代码：%s(%s)</font><a href=\"dl:%s,%s\">重试</a>"),
					(LPCTSTR)post.floor, (LPCTSTR)code, (LPCTSTR)GetTiebaErrorText(code), (LPCTSTR)tid, (LPCTSTR)post.pid);
				log.Log(content);
			}
			else
			{
				sndPlaySound(_T("删贴.wav"), SND_ASYNC | SND_NODEFAULT);
				log.Log(_T("<font color=red>锁帖：删除 </font>") + post.floor + _T("楼"));
				for (int i = 0; i < (int)(operate.m_config->m_deleteInterval * 10); i++)
				{
					if (m_stopFlag)
						break;
					Sleep(100);
				}
			}
		}

		// 扫描间隔3s
		for (int i = 0; i < 30; i++)
		{
			if (m_stopFlag)
				break;
			Sleep(100);
		}
	}

	// 结束
	m_stopFlag = TRUE;
	CoUninitialize();
	if (m_lockThreadDlg != NULL)
	{
		m_lockThreadDlg->m_tidEdit.EnableWindow(TRUE);
		m_lockThreadDlg->m_pageEdit.EnableWindow(TRUE);
		m_lockThreadDlg->m_floorEdit.EnableWindow(TRUE);
		m_lockThreadDlg->m_startButton.EnableWindow(TRUE);
		m_lockThreadDlg->m_stopButton.EnableWindow(FALSE);
	}
}
