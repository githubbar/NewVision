#if !defined( DOORDIALOG_H )
#define DOORDIALOG_H

#pragma once
#include "afxtempl.h"
#include "cv.h" 
#include "highgui.h" 
#include "Resource.h"
#include "DoorModel.h"
// ---------- CDoorControl class -------------------
class CDoorControl : public CButton {
	DECLARE_MESSAGE_MAP()
public:
	CImage image;
	int m_current; // index of the current extreme point in array
	CDoorControl();
	bool Initialize(CNewVisionDoc* doc);
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void afx_msg OnLButtonUp(UINT nFlags, CPoint point);
	void afx_msg OnLButtonDown(UINT nFlags, CPoint point);
	void afx_msg OnMouseMove(UINT nFlags, CPoint point);
	void afx_msg OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	int PointInDot(CPoint point);
};

// ------------------- DoorDialog class ------------------
class DoorDialog : public CPropertyPage, public DoorModel
{
	DECLARE_MESSAGE_MAP()
	enum { IDD = IDD_DOORDIALOG };

public:
	CDoorControl m_image;
	CScrollBar m_vbar;
	CScrollBar m_hbar;
	int offsetx, offsety;

	DoorDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~DoorDialog();
	bool Initialize();
	void DeInitialize();
	virtual BOOL OnSetActive();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtondoorRemove();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void Serialize(CArchive& ar);
};
#endif // !defined( DOORDIALOG_H )