// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LoginDlg.h"
#include <StringHelper.h>
#include "NetworkHelper.h"
#include <WinInet.h>
#include "TiebaCollect.h"


// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CNormalDlg)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CNormalDlg(CLoginDlg::IDD, pParent)
{

}

#pragma region MFC
CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CNormalDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, m_explorer);
	DDX_Control(pDX, IDC_BUTTON1, m_loginButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CNormalDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CLoginDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &CLoginDlg::OnBnClickedCancel)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CLoginDlg, CNormalDlg)
	ON_EVENT(CLoginDlg, IDC_EXPLORER1, 252, CLoginDlg::NavigateComplete2Explorer1, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()
#pragma endregion

// CLoginDlg 消息处理程序

#pragma region UI
// 销毁窗口
void CLoginDlg::OnClose()
{
	EndDialog(IDCANCEL);
}
#pragma endregion

// 初始化
BOOL CLoginDlg::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// 设置大图标
	SetIcon(hIcon, FALSE);			// 设置小图标

	m_resize.AddControl(&m_explorer, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_loginButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_explorer);
	m_resize.AddControl(&m_cancelButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_explorer);

	// 删除Cookie
	InternetSetCookieEx(_T("http://tieba.baidu.com/"), NULL,
		_T("BDUSS=; expires=Thu, 01-Jan-1900 00:00:01 GMT; path=/; domain=baidu.com;"), INTERNET_COOKIE_HTTPONLY, NULL);
	m_explorer.Navigate(_T("https://passport.baidu.com/v2/?login"), NULL, NULL, NULL, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 关闭
void CLoginDlg::OnDestroy()
{
	CNormalDlg::OnDestroy();

	// 删除Cookie
	InternetSetCookieEx(_T("http://tieba.baidu.com/"), NULL,
		_T("BDUSS=; expires=Thu, 01-Jan-1900 00:00:01 GMT; path=/; domain=baidu.com;"), INTERNET_COOKIE_HTTPONLY, NULL);
}

// 浏览器导航完毕
void CLoginDlg::NavigateComplete2Explorer1(LPDISPATCH pDisp, VARIANT* URL)
{
	Login(FALSE);
}

// 登录
void CLoginDlg::OnBnClickedButton1()
{
	Login(TRUE);
}

// 取消
void CLoginDlg::OnBnClickedCancel()
{
	EndDialog(IDCANCEL);
}

// 真登录
void CLoginDlg::Login(BOOL prompt)
{
	HRESULT result = GetCookie(m_cookie);

	TRACE(_T("0x%08X %s\n"), result, (LPCTSTR)m_cookie);
	if (FAILED(result))
	{
		if (prompt)
		{
			CString tmp;
			if (result == ERROR_INVALID_DATA)
				tmp.Format(_T("无效的Cookie：\r\n%s"), (LPCTSTR)m_cookie);
			else
				tmp.Format(_T("获取Cookie失败！\r\n错误代码0x%08X\r\n"), result);
			AfxMessageBox(tmp, MB_ICONERROR);
		}
		return;
	}

	GetLoginUserName();
	if (m_userName == _T(""))
	{
		AfxMessageBox(_T("获取用户名失败！"), MB_ICONERROR);
		return;
	}

	EndDialog(IDOK);
}

HRESULT CLoginDlg::GetCookie(CString& cookie)
{
	DWORD size;
	InternetGetCookieEx(_T("http://tieba.baidu.com/"), _T("BDUSS"), NULL,
		&size, INTERNET_COOKIE_HTTPONLY, NULL); // size为字节数
	BOOL result = InternetGetCookieEx(_T("http://tieba.baidu.com/"), _T("BDUSS"), cookie.GetBuffer(size),
		&size, INTERNET_COOKIE_HTTPONLY, NULL); // BUG：size在XP下单位是字节，在win10是字符数，XP下传字符数会返回缓冲不够
	HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
	cookie.ReleaseBuffer();

	if (result)
	{
		if (cookie.GetLength() < 100 || !StringIncludes(cookie, _T("BDUSS=")))
			return HRESULT_FROM_WIN32(ERROR_INVALID_DATA); // 无效的Cookie
		cookie += _T(";");
		return S_OK; // 成功
	}
	return hr;
}

// 取用户名
void CLoginDlg::GetLoginUserName()
{
	CString src = HTTPGet(_T("http://tieba.baidu.com/f?ie=utf-8&kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0")
						  _T("xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD"), &m_cookie);
	CString tmp;
	std::wcmatch res;
	if (std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData.user"), _T("}"))), res, USER_NAME_REG))
		m_userName = JSUnescape(res[3].str().c_str());
	if (m_userName == _T(""))
		WriteString(src, _T("login_forum.txt"));
}
