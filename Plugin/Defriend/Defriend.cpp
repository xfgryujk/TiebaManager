// Defriend.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "Defriend.h"
#include "resource.h"
#include "DefriendDlg.h"

#include <TBMAPI.h>
#include <TiebaOperate.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CDefriend g_defriend;


extern "C" __declspec(dllexport) bool __cdecl Init()
{
	return g_defriend.Init();
}

extern "C" __declspec(dllexport) bool __cdecl Uninit()
{
	return g_defriend.Uninit();
}

extern "C" __declspec(dllexport) CString __cdecl GetDescription()
{
	return _T("批量拉黑插件\r\n")
		_T("\r\n")
		_T("作者：盗我原号的没J8");
}

extern "C" __declspec(dllexport) void __cdecl OnConfig()
{
	g_defriend.OnConfig();
}


bool CDefriend::Init()
{
	return true;
}

bool CDefriend::Uninit()
{
	// 关闭窗口
	if (m_defriendDlg != NULL)
		m_defriendDlg->DestroyWindow();

	return true;
}

void CDefriend::OnConfig()
{
	if (!CTBMAPI::GetInstance().GetTiebaOperate()->HasSetTieba())
	{
		AfxMessageBox(_T("请先确认贴吧！"), MB_ICONERROR);
		return;
	}

	if (m_defriendDlg == NULL)
	{
		m_defriendDlg = new CDefriendDlg(m_defriendDlg);
		m_defriendDlg->Create(m_defriendDlg->IDD, CWnd::GetDesktopWindow());
	}
}
