#pragma once


// CTextButton

class CTextButton : public CStatic
{
	DECLARE_DYNAMIC(CTextButton)

public:
	CTextButton();
	virtual ~CTextButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	virtual void PreSubclassWindow();

public:
	COLORREF m_normalColor, m_hoverColor;
protected:
	BOOL m_mouseHover;
};


