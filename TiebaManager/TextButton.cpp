// TextButton.cpp : 实现文件
//

#include "stdafx.h"
#include "TextButton.h"


// CTextButton

IMPLEMENT_DYNAMIC(CTextButton, CStatic)

CTextButton::CTextButton()
{
	m_normalColor = RGB(0, 0, 255);
	m_hoverColor = RGB(255, 0, 0);
	m_mouseHover = FALSE;
}

CTextButton::~CTextButton()
{
}


BEGIN_MESSAGE_MAP(CTextButton, CStatic)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



// CTextButton 消息处理程序



void CTextButton::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize		= sizeof(tme);
	tme.hwndTrack	= m_hWnd;
	tme.dwFlags		= TME_LEAVE | TME_HOVER;
	tme.dwHoverTime	= 1;
	_TrackMouseEvent(&tme);

	CStatic::OnMouseMove(nFlags, point);
}

void CTextButton::OnMouseHover(UINT nFlags, CPoint point)
{
	if (!m_mouseHover)
	{
		m_mouseHover = TRUE;
		Invalidate();
	}

	CStatic::OnMouseHover(nFlags, point);
}

void CTextButton::OnMouseLeave()
{
	if (m_mouseHover)
	{
		m_mouseHover = FALSE;
		Invalidate();
	}

	CStatic::OnMouseLeave();
}

HBRUSH CTextButton::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_mouseHover ? m_hoverColor : m_normalColor);

	// TODO:  如果不应调用父级的处理程序，则返回非 null 画笔
	return (HBRUSH)(COLOR_BTNFACE + 1);
}

void CTextButton::PreSubclassWindow()
{
	ModifyStyle(0, SS_NOTIFY);
	SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_HAND));

	CStatic::PreSubclassWindow();
}
