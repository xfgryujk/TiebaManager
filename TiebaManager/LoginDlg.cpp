// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "Global.h"
#include <WinInet.h>
#include "Tieba.h"
#include "ScanImage.h"


// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialog)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
{

}

#pragma region MFC
CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_userNameEdit);
	DDX_Control(pDX, IDC_EDIT3, m_passwordEdit);
	DDX_Control(pDX, IDC_EDIT4, m_verifyCodeEdit);
	DDX_Control(pDX, IDC_STATIC4, m_verifyCodePicture);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	ON_STN_CLICKED(IDC_STATIC4, &CLoginDlg::OnStnClickedStatic4)
	ON_BN_CLICKED(IDC_BUTTON3, &CLoginDlg::OnBnClickedButton3)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()
#pragma endregion

// CLoginDlg 消息处理程序


BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// 设置大图标
	SetIcon(hIcon, FALSE);			// 设置小图标

	SetClassLong(m_verifyCodePicture.m_hWnd, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_HAND));

	// 初始化token
	CString src = HTTPGet(_T("https://passport.baidu.com/v2/api/?getapi&class=login&tpl=mn&tangram=true"), TRUE, NULL, &m_cookie);
	m_token = GetStringBetween(src, _T("token='"), _T("'"));
	if (m_token == _T("the fisrt two args should be string type:0,1!"))
	{
		src = HTTPGet(_T("https://passport.baidu.com/v2/api/?getapi&class=login&tpl=mn&tangram=true"), TRUE, NULL, &m_cookie);
		m_token = GetStringBetween(src, _T("token='"), _T("'"));
	}

	// 获取验证码
	OnStnClickedStatic4();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

// 更换验证码
void CLoginDlg::OnStnClickedStatic4()
{
	CString src = HTTPGet(_T("https://passport.baidu.com/v2/?reggetcodestr&token=") + m_token + _T("&tpl=mn&apiver=v3&tt=1374720914453&callback=bd__cbs__hb302a"), TRUE, NULL, &m_cookie);
	m_verifyStr = GetStringBetween(src, _T("\"verifyStr\" : \""), _T("\""));
	
	// 下载图片
	BYTE* buffer;
	ULONG size;
	HTTPGetRaw(_T("https://passport.baidu.com/cgi-bin/genimage?") + m_verifyStr, &buffer, &size, TRUE, NULL, &m_cookie);
	if (buffer == NULL)
	{
		AfxMessageBox(_T("获取验证码图片失败！"), MB_ICONERROR);
		return;
	}

	ReadImage(buffer, size, m_verifyImage);
	delete buffer;

	// 显示图片
	m_verifyCodePicture.Invalidate();
}

// 画验证码
void CLoginDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == m_verifyCodePicture.GetDlgCtrlID())
	{
		FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH));
		if (!m_verifyImage.IsNull())
			m_verifyImage.Draw(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem);
		FrameRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)GetStockObject(BLACK_BRUSH));
		return;
	}

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

// 登录
void CLoginDlg::OnOK()
{
	CString userName, password, verifyCode;
	m_userNameEdit.GetWindowText(userName);
	if (userName == _T(""))
	{
		m_userNameEdit.SetFocus();
		return;
	}
	m_passwordEdit.GetWindowText(password);
	if (password == _T(""))
	{
		m_passwordEdit.SetFocus();
		return;
	}
	m_verifyCodeEdit.GetWindowText(verifyCode);
	if (verifyCode == _T(""))
	{
		m_verifyCodeEdit.SetFocus();
		return;
	}

	EnableWindow(FALSE);

	time_t timestamp;
	time(&timestamp);
	CString data;
	data.Format(_T("staticpage=http%%3A%%2F%%2Fwww.baidu.com%%2Fcache%%2Fuser%%2Fhtml%%2Fv3Jump.html\
&charset=utf-8&token=%s&tpl=mn&apiver=v3&tt=%I64d&codestring=%s&isPhone=false&safeflg=0\
&u=http%%3A%%2F%%2Fwww.baidu.com%%2F&username=%s&password=%s&verifycode=%s&mem_pass=on&ppui_logintime=35219\
&callback=parent.bd__pcbs__4y6hex"), 
		m_token, timestamp, m_verifyStr, EncodeURI(userName), EncodeURI(password), verifyCode);
	CString result = HTTPPost(_T("https://passport.baidu.com/v2/api/?login"), data, TRUE, NULL, &m_cookie);

	EnableWindow(TRUE);
	if (result == NET_TIMEOUT_TEXT)
	{
		AfxMessageBox(_T("连接超时..."), MB_ICONERROR);
		OnStnClickedStatic4();
		m_verifyCodeEdit.SetWindowText(_T(""));
		return;
	}
	if (!StringIncludes(m_cookie, _T("BDUSS=")))
	{
		WriteString(result, _T("login.txt"));
		AfxMessageBox(_T("登录失败！"), MB_ICONERROR);
		if (userName.Left(2) == _T("13"))
			AfxMessageBox(_T("(不能用手机号哦)"), MB_ICONINFORMATION);
		goto error;
	}

	GetLoginUserName();
	if (m_userName == _T(""))
	{
		AfxMessageBox(_T("获取用户名失败！"), MB_ICONERROR);
		goto error;
	}

	CDialog::OnOK();
	return;

error:
	OnStnClickedStatic4();
	m_verifyCodeEdit.SetWindowText(_T(""));
}

// 使用IE Cookie
void CLoginDlg::OnBnClickedButton3()
{
	DWORD size = 1024 * 1024;
	BOOL result = InternetGetCookieEx(_T("http://tieba.baidu.com/"), NULL, m_cookie.GetBuffer(size),
		&size, INTERNET_COOKIE_HTTPONLY, NULL);
	m_cookie.ReleaseBuffer();
	if (!result)
	{
		AfxMessageBox(_T("获取Cookie失败！"), MB_ICONERROR);
		return;
	}
	if (!StringIncludes(m_cookie, _T("BDUSS=")))
	{
		AfxMessageBox(_T("请先在IE浏览器登陆百度账号并选中下次自动登录！"), MB_ICONERROR);
		return;
	}

	GetLoginUserName();
	if (m_userName == _T(""))
	{
		AfxMessageBox(_T("获取用户名失败！"), MB_ICONERROR);
		return;
	}

	AfxMessageBox(_T("登录完毕，不要在IE退出账号以免cookie失效，可以直接清除cookie"), MB_ICONINFORMATION);
	EndDialog(IDOK);
}

// 取用户名
void CLoginDlg::GetLoginUserName()
{
	CString src = HTTPGet(_T("http://tieba.baidu.com/f?ie=utf-8&kw=\
%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD"), TRUE, NULL, &m_cookie);
	std::wcmatch res;
	if (std::regex_search((LPCTSTR)GetStringBetween(src, _T("PageData.user"), _T("}")), res, USER_NAME_REG))
		m_userName = JSUnescape(res[3].str().c_str());
	if (m_userName == _T(""))
		WriteString(src, _T("login_forum.txt"));
}
