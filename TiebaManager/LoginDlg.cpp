// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LoginDlg.h"
#include "StringHelper.h"
#include "NetworkHelper.h"
#include <WinInet.h>
#include <Iepmapi.h>
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
	DDX_Control(pDX, IDC_BUTTON3, m_useIECookieButton);
	DDX_Control(pDX, IDC_BUTTON1, m_loginButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CNormalDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON3, &CLoginDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CLoginDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &CLoginDlg::OnBnClickedCancel)
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

BOOL CLoginDlg::OnInitDialog()
{
	CNormalDlg::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// 设置大图标
	SetIcon(hIcon, FALSE);			// 设置小图标

	m_resize.AddControl(&m_explorer, RT_NULL, NULL, RT_NULL, NULL, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, this);
	m_resize.AddControl(&m_useIECookieButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_explorer);
	m_resize.AddControl(&m_loginButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_explorer);
	m_resize.AddControl(&m_cancelButton, RT_KEEP_DIST_TO_RIGHT, this, RT_KEEP_DIST_TO_BOTTOM, &m_explorer);

	InternetSetCookieEx(_T("http://tieba.baidu.com/"), NULL,
		_T("BDUSS=; expires=Thu, 01-Jan-1900 00:00:01 GMT; path=/; domain=baidu.com;"), INTERNET_COOKIE_HTTPONLY, NULL);
	m_explorer.Navigate(_T("https://passport.baidu.com/v2/?login"), NULL, NULL, NULL, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 浏览器导航完毕
void CLoginDlg::NavigateComplete2Explorer1(LPDISPATCH pDisp, VARIANT* URL)
{
	OnBnClickedButton1();
}

// 使用IE Cookie
void CLoginDlg::OnBnClickedButton3()
{
	DWORD size = 1024 * 1024;
	InternetGetCookieEx(_T("http://tieba.baidu.com/"), _T("BDUSS"), m_cookie.GetBuffer(size),
		&size, INTERNET_COOKIE_HTTPONLY, NULL);
	HRESULT result = HRESULT_FROM_WIN32(GetLastError());
	m_cookie.ReleaseBuffer();

	BOOL jump = TRUE;
CheckResult:
	if (FAILED(result))
	{
		if (jump)
			goto Win10;
		CString tmp;
		tmp.Format(_T("获取Cookie失败！\r\n错误代码0x%08X"), result);
		AfxMessageBox(tmp, MB_ICONERROR);
		if (result == HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS))
			AfxMessageBox(_T("请先在IE浏览器登陆百度账号并选中下次自动登录！"), MB_ICONERROR);
		return;
	}
	if (m_cookie.GetLength() < 100 || !StringIncludes(m_cookie, _T("BDUSS=")))
	{
		if (jump)
			goto Win10;
		AfxMessageBox(_T("请先在IE浏览器登陆百度账号并选中下次自动登录！"), MB_ICONERROR);
		return;
	}
	m_cookie += _T(";");

	GetLoginUserName();
	if (m_userName == _T(""))
	{
		AfxMessageBox(_T("获取用户名失败！"), MB_ICONERROR);
		return;
	}

	AfxMessageBox(_T("登录完毕，不要在IE退出账号以免cookie失效，可以直接清除cookie"), MB_ICONINFORMATION);
	EndDialog(IDOK);
	return;

Win10:
	size = 1024 * 1024;
	result = IEGetProtectedModeCookie(L"http://tieba.baidu.com/", _T("BDUSS"), m_cookie.GetBuffer(size),
		&size, INTERNET_COOKIE_HTTPONLY);	
	jump = FALSE;
	goto CheckResult;
}

// 取用户名
void CLoginDlg::GetLoginUserName()
{
	CString src = HTTPGet(_T("http://tieba.baidu.com/f?ie=utf-8&kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0")
						  _T("xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD"), TRUE, NULL, &m_cookie);
	CString tmp;
	std::wcmatch res;
	if (std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData.user"), _T("}"))), res, USER_NAME_REG))
		m_userName = JSUnescape(res[3].str().c_str());
	if (m_userName == _T(""))
		WriteString(src, _T("login_forum.txt"));
}

// 登录
void CLoginDlg::OnBnClickedButton1()
{
	DWORD size = 1024 * 1024;
	InternetGetCookieEx(_T("http://tieba.baidu.com/"), _T("BDUSS"), m_cookie.GetBuffer(size),
		&size, INTERNET_COOKIE_HTTPONLY, NULL);
	DWORD result = GetLastError();
	m_cookie.ReleaseBuffer();

	TRACE(_T("0x%08X %s\n"), result, (LPCTSTR)m_cookie);
	if (result != ERROR_SUCCESS || m_cookie.GetLength() < 100 || !StringIncludes(m_cookie, _T("BDUSS=")))
		return;
	m_cookie += _T(";");

	GetLoginUserName();
	if (m_userName == _T(""))
	{
		AfxMessageBox(_T("获取用户名失败！"), MB_ICONERROR);
		return;
	}

	EndDialog(IDOK);
}

// 取消
void CLoginDlg::OnBnClickedCancel()
{
	EndDialog(IDCANCEL);
}
