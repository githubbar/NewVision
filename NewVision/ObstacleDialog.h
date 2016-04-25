#if !defined( OBSTACLEDIALOG_H )
#define OBSTACLEDIALOG_H
#pragma once

#include "cv.h" 
#include "highgui.h" 
#include "ObstacleModel.h"

class Object3D;
// ---------- CObstacleControl class -------------------
enum ObstacleSelectionStage {POINT1=0, POINT2=1, POINT3=2, POINT4=3};
enum ObstacleType {CUBOID=0, POLYHEDRON=1, CYLINDER=2};
enum ObstacleViewType {OBSTACLE_FRAME_VIEW=0, OBSTACLE_FLOOR_VIEW=1};

class CObstacleControl : public CButton {
	DECLARE_MESSAGE_MAP()
public:
	CImage image;
	
	ObstacleSelectionStage m_stage; // obstacle drawing stage
	int m_current; // index of the current obstacle in array
	CvPoint3D32f m_pcurrent; // selected point in the current obstacle
	bool m_holding; // the status of selecting and object and holding the left mouse button 

	CObstacleControl();
	bool Initialize(CNewVisionDoc* doc);
	void ChangeView();
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void afx_msg OnLButtonUp(UINT nFlags, CPoint point);
	void afx_msg OnMButtonUp(UINT nFlags, CPoint point);
	void afx_msg OnRButtonUp(UINT nFlags, CPoint point);
	void afx_msg OnLButtonDown(UINT nFlags, CPoint point);
	void afx_msg OnMButtonDown(UINT nFlags, CPoint point);
	void afx_msg OnRButtonDown(UINT nFlags, CPoint point);
	void afx_msg OnLButtonDblClk(UINT nFlags, CPoint point);
	void afx_msg OnMouseMove(UINT nFlags, CPoint point);
	void afx_msg OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	int PointInDotFrame(CPoint point, CvPoint3D32f &pcurrent);
	int PointInDotFloor(CPoint point, CvPoint3D32f &pcurrent);
};

// ---------------------------------------------------
// ObstacleDialog dialog
class ObstacleDialog : public CPropertyPage, public ObstacleModel
{
	enum { IDD = IDD_OBSTACLEDIALOG };
	DECLARE_MESSAGE_MAP()

public:
	CObstacleControl m_image;
	CScrollBar m_vbar;
	CScrollBar m_hbar;
	int offsetx, offsety;
	int m_obstacleType;  // cuboid, polyhedron or cylinder
	int m_viewType;      // frame or floor view          

	ObstacleDialog(CWnd* pParent = NULL);
	virtual ~ObstacleDialog();
	bool Initialize();
	void DeInitialize();
	virtual BOOL OnSetActive();
	virtual void Serialize(CArchive& ar);
	virtual void DoDataExchange(CDataExchange* pDX);   
	Object3D* CreateNewObstacle(CvPoint2D32f point);
	afx_msg void OnBnClickedRemoveAll();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCuboid();
	afx_msg void OnBnClickedPolyhedron();
	afx_msg void OnBnClickedCylinder();
	afx_msg void OnBnClickedFrameView();
	afx_msg void OnBnClickedFloorView();
};
#endif // !defined( OBSTACLEDIALOG_H )