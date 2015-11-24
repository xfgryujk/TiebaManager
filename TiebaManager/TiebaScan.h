#pragma once
class CTiebaManagerDlg;
struct IHTMLDocument2;


BOOL CheckIllegal(LPCTSTR content, LPCTSTR author, CString& msg, int& pos, int& length);
UINT AFX_CDECL ScanThread(LPVOID mainDlg);
UINT AFX_CDECL ScanPostThread(LPVOID threadID);
BOOL ScanPostPage(const CString& tid, int page, const CString& title, BOOL hasHistoryReply,
	int ScanedCount, const CString& src, int threadID, CTiebaManagerDlg* dlg, CComPtr<IHTMLDocument2>* pDocument);
