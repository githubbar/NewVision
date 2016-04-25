#if !defined( SKINDIALOG_H )
#define SKINDIALOG_H

#pragma once
#include "afxtempl.h"
#include "cv.h" 
#include "highgui.h" 
#include "SkinModel.h"
#include "Resource.h"


// ---------- CSkinControl class -------------------
class CSkinControl : public CButton {
	DECLARE_MESSAGE_MAP()
public:
	static const int DOT = 2;
	CImage image;
	CSkinControl();
	bool Initialize(CNewVisionDoc* doc);
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void afx_msg OnLButtonDown(UINT nFlags, CPoint point);
	void afx_msg OnMouseMove(UINT nFlags, CPoint point);
};

// ---------- CSkinControl class -------------------
class CHistogramControl : public CButton {
public:
	CImage image;
	CHistogramControl();
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};

// ---------- CColorControl class -------------------
class CColorControl : public CButton {
public:
	COLORREF color;
	CColorControl();
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};

// ------------------- SkinDialog class ------------------
class SkinDialog : public CPropertyPage, public SkinModel
{
	DECLARE_MESSAGE_MAP()
	enum { IDD = IDD_SKINDIALOG };

public:
	CSkinControl m_image;
	CHistogramControl m_histogram;
	CColorControl m_color;

	CScrollBar m_vbar;
	CScrollBar m_hbar;
	int offsetx, offsety;

	SkinDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~SkinDialog();
	bool Initialize();
	void DeInitialize();
	virtual BOOL OnSetActive();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonClear();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void Serialize(CArchive& ar);
};
#endif // !defined( SKINDIALOG_H )