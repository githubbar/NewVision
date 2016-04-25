#if !defined( ZONEDIALOG_H )
#define ZONEDIALOG_H

#pragma once
#include "afxtempl.h"
#include "cv.h" 
#include "highgui.h" 
#include "Resource.h"
#include "ZoneModel.h"
#include "ResizablePage.h"

enum ZoneViewType {ZONE_FRAME_VIEW=0, ZONE_FLOOR_VIEW=1};
// ---------- CZoneControl class -------------------
class CZoneControl : public CButton {
	DECLARE_MESSAGE_MAP()
public:
	static const int DOT = 2;
	CImage image;
	int m_iZone;  // index of the current zone in array
	int m_iPoint; // index of the current point in array

	CZoneControl();
	bool Initialize(CNewVisionDoc* doc);
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void afx_msg OnLButtonUp(UINT nFlags, CPoint point);
	void afx_msg OnLButtonDown(UINT nFlags, CPoint point);
	void afx_msg OnLButtonDblClk(UINT nFlags, CPoint point);
	void afx_msg OnMouseMove(UINT nFlags, CPoint point);
	void afx_msg OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	int PointInDotFrame(CPoint point, int &iZone, int &iPoint);
	int PointInDotFloor(CPoint point, int &iZone, int &iPoint);
	int PointInsertIndex(CvPoint3D32f point, int iZone);
};

// ------------------- ZoneDialog class ------------------
class ZoneDialog : public CPropertyPage, public ZoneModel
{
	DECLARE_MESSAGE_MAP()
	enum { IDD = IDD_ZONEDIALOG };

public:
	CZoneControl m_image;
	CScrollBar m_vbar;
	CScrollBar m_hbar;
	int offsetx, offsety;
	int m_viewType;      // frame or floor view          
	int m_creatingNew;

	ZoneDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~ZoneDialog();
	bool Initialize();
	void DeInitialize();
	virtual BOOL OnSetActive();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonZoneRemove();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void Serialize(CArchive& ar);
	afx_msg void OnBnClickedButtonZoneFromObstacle();
	afx_msg void OnBnClickedButtonZoneNew();
	afx_msg void OnBnClickedZoneFrameView();
	afx_msg void OnBnClickedZoneFloorView();
};
#endif // !defined( ZONEDIALOG_H )