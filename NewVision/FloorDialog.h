#if !defined( FLOORDIALOG_H )
#define FLOORDIALOG_H

#pragma once
#include "afxtempl.h"
#include "cv.h" 
#include "highgui.h" 
#include "FloorModel.h"

#include "afxwin.h"

// ---------- CFloorControl class -------------------
class CFloorControl : public CButton {
	DECLARE_MESSAGE_MAP()
public:
	static const int DOT = 2;
	CImage image;
	int m_current; // index of the current extreme point in array
	CFloorControl();
	bool Initialize(CNewVisionDoc* doc);
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void afx_msg OnLButtonUp(UINT nFlags, CPoint point);
	void afx_msg OnLButtonDown(UINT nFlags, CPoint point);
	void afx_msg OnMouseMove(UINT nFlags, CPoint point);
	void afx_msg OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	int PointInDot(CPoint point);
};

// ------------------- FloorDialog class ------------------
class FloorDialog : public CPropertyPage, public FloorModel
{
	DECLARE_MESSAGE_MAP()
	enum { IDD = IDD_FLOORDIALOG };

public:
	CFloorControl m_image;
	CScrollBar m_vbar;
	CScrollBar m_hbar;

	FloorDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~FloorDialog();
	bool Initialize();
	void DeInitialize();
	virtual BOOL OnSetActive();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void Serialize(CArchive& ar);
	afx_msg void OnBnClickedRemove();
	afx_msg void OnBnClickedFloorModelSelectButton();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
#endif // !defined( FLOORDIALOG_H )