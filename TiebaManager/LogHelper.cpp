#include "stdafx.h"
#include "LogHelper.h"

#include "explorer1.h"
#include <MiscHelper.h>
#include <StringHelper.h>


static const TCHAR LOG_FRAME[] = _T("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=gbk\" /><title>日志</title>\r\n")
								 _T("<style type=\"text/css\">\r\n")
								 _T("body {border:1px solid black; overflow:auto; margin:0px; padding:3px;")
								 _T(" font-family:\"宋体\",Verdana; font-size:9pt; line-height:9pt}\r\n")
								 _T("a:link {text-decoration:none}\r\na:hover {text-decoration:underline}\r\na:visited {text-decoration:none}\r\n")
								 _T("</style>\r\n</head><body>\r\n");

static const UINT WM_LOG = WM_APP + 2;


WNDPROC CLog::s_oldExplorerWndProc = NULL;


// 初始化
void CLog::Init()
{
	// 取document
	m_logExplorer.Navigate(_T("about:blank"), NULL, NULL, NULL, NULL);
	while (TRUE)
	{
		Delay(1);
		CComDispatchDriver disp(m_logExplorer.get_Document());
		if (disp.p == NULL)
			continue;
		if (SUCCEEDED(disp->QueryInterface(&m_logDocument)))
			break;
	}

	// 写框架
	WriteDocument(LOG_FRAME);

	// 屏蔽右键菜单、监听Log消息
	m_explorerHwnd = m_logExplorer.m_hWnd;
	EnumChildWindows(m_explorerHwnd, EnumChildProc, (LPARAM)&m_explorerHwnd);
	s_oldExplorerWndProc = (WNDPROC)SetWindowLong(m_explorerHwnd, GWL_WNDPROC, (LONG)ExplorerWndProc);

	// 日志开始时间
	GetLocalTime(&m_logStartTime);
}

// 释放
void CLog::Release()
{
	m_explorerHwnd = NULL;
	m_logDocument.Release();
}

// 输出日志1，把内容格式化发送到消息队列
void CLog::Log(LPCTSTR content)
{
	if (m_explorerHwnd == NULL)
		return;

	SYSTEMTIME time;
	GetLocalTime(&time);
	CString* output = new CString();
	output->Format(_T("%02d:%02d:%02d %s<br />\r\n"), time.wHour, time.wMinute, time.wSecond, content);
	PostMessage(m_explorerHwnd, WM_LOG, (WPARAM)this, (LPARAM)output);
}

// 输出日志2，在m_logExplorer写日志
void CLog::DoLog(const CString* output)
{
	if (m_logDocument.p == NULL)
	{
		delete output;
		return;
	}

	WriteDocument(*output);
	delete output;

	// 滚动到底端
	CComPtr<IHTMLElement> body;
	m_logDocument->get_body(&body);
	// 取ID
	static LPOLESTR scrollHeightName = OLESTR("scrollHeight"), scrollTopName = OLESTR("scrollTop");
	static DISPID scrollHeightID = -1, scrollTopID = -1;
	if (scrollHeightID == -1)
		body->GetIDsOfNames(IID_NULL, &scrollHeightName, 1, LOCALE_SYSTEM_DEFAULT, &scrollHeightID);
	if (scrollTopID == -1)
		body->GetIDsOfNames(IID_NULL, &scrollTopName, 1, LOCALE_SYSTEM_DEFAULT, &scrollTopID);
	// body.scrollTop = body.scrollHeight
	DISPPARAMS params = {};
	_variant_t scrollHeight;
#pragma warning(suppress: 6102)
	if (scrollHeightID != -1)
		body->Invoke(scrollHeightID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &params,
			scrollHeight.GetAddress(), NULL, NULL);
	params.cArgs = 1;
	params.rgvarg = &scrollHeight;
#pragma warning(suppress: 6102)
	if (scrollTopID != -1)
		body->Invoke(scrollTopID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUT, &params, NULL, NULL, NULL);
}

// 清空日志
void CLog::Clear()
{
	if (m_logDocument.p == NULL)
		return;

	IDispatch* tmp = NULL;
	m_logDocument->open(_T("about:blank"), variant_t(), variant_t(), variant_t(), &tmp);
	if (tmp != NULL)
		tmp->Release();
	WriteDocument(LOG_FRAME);
	GetSystemTime(&m_logStartTime);
}

// 保存日志
void CLog::Save(LPCTSTR folder)
{
	if (m_logDocument.p == NULL)
		return;

	// 取日志HTML
	CComDispatchDriver documentDisp(m_logDocument);

	// document.documentElement.outerHTML
	_variant_t res;
	if (FAILED(documentDisp.GetPropertyByName(OLESTR("documentElement"), res.GetAddress())))
		return;
	CComDispatchDriver documentElementDisp((IDispatch*)res);
	if (FAILED(documentElementDisp.GetPropertyByName(OLESTR("outerHTML"), res.GetAddress())))
		return;
	CString strHtml = (LPCTSTR)(_bstr_t)res;

	// 另一种取网页HTML方法，末尾有四个乱码？
	/*CComPtr<IPersistStreamInit> psi;
	document->QueryInterface(&psi);
	HGLOBAL html = GlobalAlloc(GMEM_MOVEABLE, 5 * 1024 * 1024);
	IStream *stream;
	CreateStreamOnHGlobal(html, TRUE, &stream);
	psi->Save(stream, FALSE);
	CString strHtml = (LPCTSTR)GlobalLock(html);
	strHtml += _T("</body></html>");
	GlobalUnlock(html);
	stream->Release();*/

	// 保存
	CreateDir(folder);
	CString path;
	path.Format(_T("%s\\%d-%02d-%02d %02d：%02d：%02d.html"), folder, m_logStartTime.wYear, m_logStartTime.wMonth,
		m_logStartTime.wDay, m_logStartTime.wHour, m_logStartTime.wMinute, m_logStartTime.wSecond);
	WriteString(strHtml, path);
}

// 枚举寻找Internet Explorer_Server窗口
BOOL CALLBACK CLog::EnumChildProc(HWND hwnd, LPARAM lParam)
{
	TCHAR buf[30];
	GetClassName(hwnd, buf, _countof(buf));
	if (_tcscmp(buf, _T("Internet Explorer_Server")) == 0)
	{
		*(HWND*)lParam = hwnd;
		return FALSE;
	}
	else
		return TRUE;
}

// 屏蔽日志右键菜单、监听Log消息
LRESULT CALLBACK CLog::ExplorerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_LOG)
	{
		((CLog*)wParam)->DoLog((CString*)lParam);
		return 0;
	}
	if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP)
		return 0;
	return CallWindowProc(s_oldExplorerWndProc, hwnd, uMsg, wParam, lParam);
}

// 写HTML到document
void CLog::WriteDocument(const CString& content)
{
	SAFEARRAY *arr = SafeArrayCreateVector(VT_VARIANT, 0, 1);
	VARIANT *str = NULL;
	if (FAILED(SafeArrayAccessData(arr, (LPVOID*)&str)))
		return;
	str->vt = VT_BSTR;
	str->bstrVal = content.AllocSysString();
	SafeArrayUnaccessData(arr);
	m_logDocument->write(arr);
	SafeArrayDestroy(arr);
}
