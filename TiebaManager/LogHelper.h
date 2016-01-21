#pragma once
#include <mshtml.h>
class CExplorer1;
struct IHTMLDocument2;


class CLog
{
protected:
	CExplorer1& m_logExplorer;
	HWND m_explorerHwnd;
	CComPtr<IHTMLDocument2> m_logDocument;

	SYSTEMTIME m_logStartTime;

	static WNDPROC s_oldExplorerWndProc;

public:
	CLog(CExplorer1& explorer) : m_logExplorer(explorer)
	{
		m_explorerHwnd = NULL;
	}

	void Init();
	void Release();
	void Log(LPCTSTR content);
	void Clear();
	void Save(LPCTSTR folder);

protected:
	static BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);
	static LRESULT CALLBACK ExplorerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void WriteDocument(const CString& content);

	void DoLog(const CString* output);
};
