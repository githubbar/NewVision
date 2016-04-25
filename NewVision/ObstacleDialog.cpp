// ObstacleDialog.cpp : implementation file
#include "stdafx.h"
#include "NewVisionDoc.h"
#include "ObstacleDialog.h"
#include "Globals.h"
#include "Object3D.h"
#include "Cuboid.h"
#include "Cylinder.h"
#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::Diagnostics;

// ObstacleDialog dialog

BEGIN_MESSAGE_MAP(ObstacleDialog, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTONOBSTACLEREMOVE, OnBnClickedRemoveAll)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_CUBOID, OnBnClickedCuboid)
	ON_BN_CLICKED(IDC_POLYHEDRON, OnBnClickedPolyhedron)
	ON_BN_CLICKED(IDC_CYLINDER, OnBnClickedCylinder)
	ON_BN_CLICKED(IDC_FRAME_VIEW, OnBnClickedFrameView)
	ON_BN_CLICKED(IDC_FLOOR_VIEW, OnBnClickedFloorView)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CObstacleControl, CButton)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()
// --------------------------------------------------------------------------
ObstacleDialog::ObstacleDialog(CWnd* pParent /* = NULL */)
	: CPropertyPage(ObstacleDialog::IDD), m_image()
{
}
// --------------------------------------------------------------------------
ObstacleDialog::~ObstacleDialog()
{
}
// --------------------------------------------------------------------------
void ObstacleDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OBSTACLEIMAGE, m_image);
	DDX_Control(pDX, IDC_OBSTACLE_SCROLLBAR1, m_vbar);
	DDX_Control(pDX, IDC_OBSTACLE_SCROLLBAR2, m_hbar);
	DDX_Radio(pDX, IDC_CUBOID, m_obstacleType);
	DDX_Radio(pDX, IDC_FRAME_VIEW, m_viewType);
}
// --------------------------------------------------------------------------
void ObstacleDialog::Serialize( CArchive& ar )
 {
	ObstacleModel::Serialize(ar);
}
// --------------------------------------------------------------------------
BOOL ObstacleDialog::OnSetActive() {
	CPropertyPage::OnSetActive();
	((CButton*)GetDlgItem(IDC_CUBOID))->SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_CUBOID)));
	((CButton*)GetDlgItem(IDC_POLYHEDRON))->SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_POLYHEDRON)));
	((CButton*)GetDlgItem(IDC_CYLINDER))->SetBitmap(::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_CYLINDER)));
	return m_image.Initialize(doc);
}
// --------------------------------------------------------------------------
bool ObstacleDialog::Initialize() {
	if (!m_image.Initialize(doc))
		return false;
	return true;
}
// --------------------------------------------------------------------------
void ObstacleDialog::DeInitialize() {
	m_image.m_stage = POINT1;
	m_obstacleType = 0;
	m_viewType = OBSTACLE_FRAME_VIEW;
	obstacle.RemoveAll();
}
// --------------------------------------------------------------------------
void ObstacleDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int curpos = pScrollBar->GetScrollPos();
	int minpos;
	int maxpos;
	pScrollBar->GetScrollRange(&minpos, &maxpos);
	SCROLLINFO   info;
    pScrollBar->GetScrollInfo(&info, SIF_ALL);
	switch (nSBCode)
    {
		case SB_TOP:      
			curpos = minpos;
			break;
		case SB_BOTTOM:     
			curpos = maxpos;
			break;
		case SB_ENDSCROLL: 
			break;
		case SB_LINEUP:      
			if (curpos > minpos)
				curpos--;
			break;
		case SB_LINEDOWN:   
			if (curpos < maxpos)
				curpos++;
			break;
		case SB_PAGEUP:    
			curpos = max(minpos, curpos - (int) info.nPage);
			break;
		case SB_PAGEDOWN:
			curpos = min(maxpos, curpos + (int) info.nPage);
			break;
		case SB_THUMBPOSITION: 
			curpos = nPos;     
			break;
		case SB_THUMBTRACK:
			curpos = nPos;     
			break;
    }    
	offsety = curpos;
	pScrollBar->SetScrollPos(curpos);
	m_image.Invalidate();
    CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
// --------------------------------------------------------------------------
void ObstacleDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int curpos = pScrollBar->GetScrollPos();
	int minpos;
	int maxpos;
	pScrollBar->GetScrollRange(&minpos, &maxpos);
	SCROLLINFO   info;
    pScrollBar->GetScrollInfo(&info, SIF_ALL);
	switch (nSBCode)
    {
		case SB_LEFT:      
			curpos = minpos;
			break;
		case SB_RIGHT:     
			curpos = maxpos;
			break;
		case SB_ENDSCROLL: 
			break;
		case SB_LINELEFT:      
			if (curpos > minpos)
				curpos--;
			break;
		case SB_LINERIGHT:   
			if (curpos < maxpos)
				curpos++;
			break;
		case SB_PAGELEFT:    
			curpos = max(minpos, curpos - (int) info.nPage);
			break;
		case SB_PAGERIGHT:
			curpos = min(maxpos, curpos + (int) info.nPage);
			break;
		case SB_THUMBPOSITION: 
			curpos = nPos;     
			break;
		case SB_THUMBTRACK:
			curpos = nPos;     
			break;
    }    
	offsetx = curpos; 
	pScrollBar->SetScrollPos(curpos);
	m_image.Invalidate();
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
// --------------------------------------------------------------------------
void ObstacleDialog::OnBnClickedRemoveAll()
{
	m_image.m_stage = POINT1;
	obstacle.RemoveAll();
	Invalidate();
	UpdateWindow();
}

// =========================================================================
// --------------------- CObstacleControl class ---------------------------
CObstacleControl::CObstacleControl() {
}
// --------------------- CObstacleControl class ---------------------------
bool CObstacleControl::Initialize(CNewVisionDoc* doc) {
	m_stage = POINT1;
	m_holding = false;
	m_current = -1;

	// Show scroll bars
	if (m_hWnd) {
		ObstacleDialog* tab = (ObstacleDialog*)GetParent();
		switch (tab->m_viewType) {
			case OBSTACLE_FRAME_VIEW:
				image.Destroy();
				image.CopyOf(imgFrame);
				break;
			case OBSTACLE_FLOOR_VIEW:
				image.Destroy();
				image.CopyOf(floorImg);
				cvZero(image.GetImage());
				tab->doc->floormodel.Draw(image.GetImage());
		}
		CRect r; this->GetWindowRect(&r);
		SCROLLINFO horz;
		horz.cbSize = sizeof(SCROLLINFO);
		horz.fMask = SIF_ALL;
		horz.nPage = 50;
		horz.nMin = 0;
		horz.nMax = image.Width()-r.Width()+horz.nPage;
		horz.nPos = 0;
		horz.nTrackPos = 0;
		tab->m_hbar.SetScrollInfo(&horz);
		SCROLLINFO vert;
		vert.cbSize = sizeof(SCROLLINFO);
		vert.fMask = SIF_ALL;
		vert.nPage = 50;
		vert.nMin = 0;
		vert.nMax = image.Height()-r.Height()+vert.nPage;
		vert.nPos = 0;
		vert.nTrackPos = 0;
		tab->m_vbar.SetScrollInfo(&vert);
		tab->m_vbar.ShowWindow(true);  //Hide Vertical Scroll Bar
		tab->m_hbar.ShowWindow(true);  //Hide Horizontal Scroll Bar
		tab->offsetx = 0; tab->offsety = 0;
		this->Invalidate();
	}
	return true;
}
// --------------------------------------------------------------------------
void CObstacleControl::ChangeView() {
}
// --------------------------------------------------------------------------
Object3D* ObstacleDialog::CreateNewObstacle(CvPoint2D32f point) {
	switch (m_obstacleType) {
		case CUBOID: 
			return new Cuboid(point, doc);
		case POLYHEDRON:
			//return new Polyhedron(point, doc);
			return NULL;
		case CYLINDER:
			return new Cylinder(point, doc);
		default:
			return NULL;
	}
	
}
// --------------------------------------------------------------------------
void CObstacleControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	ObstacleDialog* dlg=(ObstacleDialog*)this->GetParent();
	image.Show(pDC->m_hDC, 0, 0, image.Width(), image.Height(), dlg->offsetx, dlg->offsety);
	CPen bNew(PS_SOLID, 1, RGB(0, 0, 255));
	switch (dlg->m_viewType) {
		case OBSTACLE_FRAME_VIEW:
			dlg->DrawFrame(pDC, dlg->offsetx, dlg->offsety, &bNew);
			if (m_holding && m_current != -1) {
				CvPoint3D32f p3 = dlg->obstacle[m_current]->GetCenter();
				CvPoint point = cvPointFrom32f(dlg->doc->cameramodel.coordsReal2Image(p3));
				//CRect rToolTip(CPoint(point.x-70-dlg->offsetx, point.y-16-dlg->offsety), CSize(70, 16));
				CRect rToolTip(CPoint(point.x-70-dlg->offsetx, point.y-16-dlg->offsety), CSize(140, 16));
				double ddd = dlg->doc->cameramodel.coordsDistanceFromCamera(p3);
				CString text;text.Format("x=%-.0f y=%-.0f z=%-.0f", p3.x, p3.y, p3.z);
				drawStaticToolTip(pDC, text, rToolTip);
			}
			break;
		case OBSTACLE_FLOOR_VIEW:
			dlg->DrawFloor(pDC, dlg->offsetx, dlg->offsety, &bNew);
	}
}
// --------------------------------------------------------------------------
void CObstacleControl::OnMButtonDown(UINT nFlags, CPoint point) {
	OnLButtonDown(nFlags, point);
}
// --------------------------------------------------------------------------
void CObstacleControl::OnRButtonDown(UINT nFlags, CPoint point) {
	OnLButtonDown(nFlags, point);
}
// --------------------------------------------------------------------------
void CObstacleControl::OnLButtonDown(UINT nFlags, CPoint point) {
	CWnd::OnLButtonDown(nFlags, point);
	ObstacleDialog* dlg=((ObstacleDialog*)this->GetParent());
	CPoint off = CPoint(dlg->m_hbar.GetScrollPos(), dlg->m_vbar.GetScrollPos());
	point += off;
	switch (dlg->m_viewType) {
		case OBSTACLE_FRAME_VIEW: {
			switch (m_stage) {
				case POINT1: {
					RESTART:
					int match = PointInDotFrame(point, m_pcurrent);
					if (match != -1) {
						m_current = match;
						m_holding = true;
					}
					else {
						Object3D* obst = dlg->CreateNewObstacle(cvPoint2D32f(point));
						dlg->obstacle.Add(obst);
						m_current = dlg->obstacle.GetCount()-1;
					}
					break;
				}
				case POINT2: {
					if (!dlg->obstacle[m_current]->Move2(cvPoint2D32f(point))) {
						m_stage = POINT1;
						goto RESTART;
					}
					break;
				}
				case POINT3: {
					if (!dlg->obstacle[m_current]->Move3(cvPoint2D32f(point))) {
						m_stage = POINT1;
						goto RESTART;
					}
					break;
				}
				case POINT4: {
					if (!dlg->obstacle[m_current]->Move4(cvPoint2D32f(point))) {
						m_stage = POINT1;
						goto RESTART;
					}
				}
			}
		}
		break;
		case OBSTACLE_FLOOR_VIEW: {
			if (m_stage == POINT1) {// move existing obstacles only!
				int match = PointInDotFloor(point, m_pcurrent);
				if (match != -1) {
					m_current = match;
					m_holding = true;
				}
			}
		}
	}
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void CObstacleControl::OnLButtonDblClk(UINT nFlags, CPoint point) {
	// prompt for label
	ObstacleDialog* tab = (ObstacleDialog*)GetParent();
	CPoint off = CPoint(tab->m_hbar.GetScrollPos(), tab->m_vbar.GetScrollPos());
	point += off;
	switch (tab->m_viewType) {
		case OBSTACLE_FRAME_VIEW: {
			int match = PointInDotFrame(point, m_pcurrent);
			if (match != -1 && m_stage == POINT1) {
				point -= off;
				drawEditableToolTip(this, tab->obstacle[match]->label, CRect(point, CSize(80,16)));								
				m_stage = POINT1;
				m_current = -1;
			}
			else {
				tab->obstacle.RemoveAt(tab->obstacle.GetCount()-1);
				m_stage = POINT1;
				m_current = -1;
			}
		}
								  break;
		case OBSTACLE_FLOOR_VIEW: {
			int match = PointInDotFloor(point, m_pcurrent);
			if (match != -1) {
				point -= off;
				drawEditableToolTip(this, tab->obstacle[match]->label, CRect(point, CSize(80,16)));								
			}
		}
	}
}
// --------------------------------------------------------------------------
void CObstacleControl::OnMouseMove(UINT nFlags, CPoint point) {
	CWnd::OnMouseMove(nFlags, point);
	if (m_current == -1)
		return;
	ObstacleDialog* tab = (ObstacleDialog*)GetParent();
	CPoint off = CPoint(tab->m_hbar.GetScrollPos(), tab->m_vbar.GetScrollPos());
	point += off;
	switch (tab->m_viewType) {
		case OBSTACLE_FRAME_VIEW: {
				if (m_holding) {
					// move obstacle
					double diffX = 0, diffY = 0, diffZ = 0;
					if (nFlags & MK_LBUTTON)
						diffX = tab->doc->cameramodel.computeX(cvPoint2D32f(point), m_pcurrent) - m_pcurrent.x;
					else if (nFlags & MK_MBUTTON)
						diffY = tab->doc->cameramodel.computeY(cvPoint2D32f(point), m_pcurrent) - m_pcurrent.y;
					else if (nFlags & MK_RBUTTON)
						diffZ = tab->doc->cameramodel.computeZ(cvPoint2D32f(point), m_pcurrent) - m_pcurrent.z;
					tab->obstacle[m_current]->Move(cvPoint3D32f((float)diffX, diffY, diffZ));
					m_pcurrent.x += (float)diffX; m_pcurrent.y += (float)diffY; m_pcurrent.z += (float)diffZ;
				}
				else if (nFlags & MK_LBUTTON) {
					switch (m_stage) {
						case POINT1: {
							tab->obstacle[m_current]->Move1(cvPoint2D32f(point));
							break;
						}
						case POINT2: {
							tab->obstacle[m_current]->Move2(cvPoint2D32f(point));
							break;
						}
						case POINT3: {
							tab->obstacle[m_current]->Move3(cvPoint2D32f(point));
							break;
						}
						case POINT4: {
							tab->obstacle[m_current]->Move4(cvPoint2D32f(point));
						}
					}
				}
		}
		break;
		case OBSTACLE_FLOOR_VIEW: {
			if (m_holding) {
				// move obstacle in XY-plane
				CvPoint3D32f p3 = tab->doc->floormodel.coordsFloor2Real(cvPoint(point));
				double dx = p3.x-m_pcurrent.x;
				double dy = p3.y-m_pcurrent.y;
				tab->obstacle[m_current]->Move(cvPoint3D32f((float)dx, (float)dy, 0));
				m_pcurrent = p3;
			}
		 }
	}
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void CObstacleControl::OnLButtonUp(UINT nFlags, CPoint point) {
	CWnd::OnLButtonUp(nFlags, point);
	if (m_holding) {
		m_holding = false;
		m_current = -1;
		return;
	}
	if (m_current == -1)
		return;

	switch (m_stage) {
		case POINT1: {
			m_stage = POINT2;
			break;
		}
		case POINT2: {
			m_stage = POINT3;
			break;
		}
		case POINT3: {
			m_stage = POINT4;
			break;
		}
		case POINT4: {
			m_stage = POINT1;
			m_current = -1;
		}
	}
	Invalidate();
	UpdateWindow();
}
void CObstacleControl::OnMButtonUp(UINT nFlags, CPoint point) {
	OnLButtonUp(nFlags, point);
}
// --------------------------------------------------------------------------
void CObstacleControl::OnRButtonUp(UINT nFlags, CPoint point) {
	OnLButtonUp(nFlags, point);
}
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
void CObstacleControl::OnKeyDown(UINT nChar, UINT nRepCnt,	UINT nFlags) {
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	if (nChar == VK_DELETE && m_current != -1) {
		ObstacleDialog* tab = (ObstacleDialog*)GetParent();
		tab->obstacle.RemoveAt(m_current);
		m_current = -1;
		m_stage = POINT1;
		Invalidate();
		UpdateWindow();
	}
}
// --------------------------------------------------------------------------
int CObstacleControl::PointInDotFrame(CPoint point, CvPoint3D32f& pcurrent) {
	ObstacleDialog* tab = (ObstacleDialog*)GetParent();
	for (int i=0;i<tab->obstacle.GetCount();i++) {
		if (tab->obstacle[i]->IsFramePointIn(cvPoint(point), pcurrent)) {
			Invalidate();
			UpdateWindow();
			return i; // return the index of the match found
		}
	}
	return -1; // no match found
}
// --------------------------------------------------------------------------
int CObstacleControl::PointInDotFloor(CPoint point, CvPoint3D32f& pcurrent) {
	ObstacleDialog* tab = (ObstacleDialog*)GetParent();
	for (int i=0;i<tab->obstacle.GetCount();i++) {
		if (tab->obstacle[i]->IsFloorPointIn(cvPoint(point), pcurrent)) {
			Invalidate();
			UpdateWindow();
			return i; // return the index of the match found
		}
	}
	return -1; // no match found
}
// --------------------------------------------------------------------------
void ObstacleDialog::OnBnClickedCuboid()
{
	UpdateData();
}
// --------------------------------------------------------------------------
void ObstacleDialog::OnBnClickedPolyhedron()
{
	UpdateData();
}
// --------------------------------------------------------------------------
void ObstacleDialog::OnBnClickedCylinder()
{
	UpdateData();
}
// --------------------------------------------------------------------------
void ObstacleDialog::OnBnClickedFrameView()
{
	UpdateData();
	m_image.Initialize(this->doc);
}
// --------------------------------------------------------------------------
void ObstacleDialog::OnBnClickedFloorView()
{
	UpdateData();
	m_image.Initialize(this->doc);
}
// --------------------------------------------------------------------------
