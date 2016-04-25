#include "stdafx.h"
#include "afxtempl.h"
#include "NewVisionDoc.h"
#include "FloorRegion.h"
#include "ZoneDialog.h"
#include "Globals.h"
#include "TextEntryDlg.h"
#include "NewVision.h"
#include "Cuboid.h"

BEGIN_MESSAGE_MAP(ZoneDialog, CDialog)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BUTTONZONEREMOVE, OnBnClickedButtonZoneRemove)
	ON_BN_CLICKED(IDC_BUTTONZONEFROMOBSTACLE, &ZoneDialog::OnBnClickedButtonZoneFromObstacle)
	ON_BN_CLICKED(IDC_BUTTONZONENEW, &ZoneDialog::OnBnClickedButtonZoneNew)
	ON_BN_CLICKED(IDC_ZONE_FRAME_VIEW, &ZoneDialog::OnBnClickedZoneFrameView)
	ON_BN_CLICKED(IDC_ZONE_FLOOR_VIEW, &ZoneDialog::OnBnClickedZoneFloorView)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CZoneControl, CButton)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// --------------------------------------------------------------------------
ZoneDialog::ZoneDialog(CWnd* pParent /*=NULL*/)
	: CPropertyPage (ZoneDialog::IDD)
{
	m_creatingNew = 0;
}
// --------------------------------------------------------------------------
ZoneDialog::~ZoneDialog()
{
}
// --------------------------------------------------------------------------
BOOL ZoneDialog::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// preset layout
//	AddAnchor(IDC_ZONEIMAGE, ANCHOR(80,95));
	return TRUE;
}

// --------------------------------------------------------------------------
bool ZoneDialog::Initialize() {
	if (!m_image.Initialize(doc))
		return false;
	return true;
}
// --------------------------------------------------------------------------
void ZoneDialog::DeInitialize() {
	m_viewType = ZONE_FRAME_VIEW;
	zone.RemoveAll();
}
// --------------------------------------------------------------------------
BOOL ZoneDialog::OnSetActive() {
	CPropertyPage::OnSetActive();
	return m_image.Initialize(doc);
}
// --------------------------------------------------------------------------
void ZoneDialog::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_ZONEIMAGE, m_image);
	DDX_Control(pDX, IDC_ZONE_SCROLLBAR1, m_vbar);
	DDX_Control(pDX, IDC_ZONE_SCROLLBAR2, m_hbar);
	DDX_Check(pDX, IDC_BUTTONZONENEW, m_creatingNew);
	DDX_Radio(pDX, IDC_ZONE_FRAME_VIEW, m_viewType);
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void ZoneDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
void ZoneDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
void ZoneDialog::OnBnClickedButtonZoneRemove()
{
	zone.RemoveAll();
	m_image.m_iZone = -1;
	m_image.m_iPoint = -1;
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void ZoneDialog::Serialize( CArchive& ar )
 {
	ZoneModel::Serialize( ar );
 }

//==========================================================================
// ------------------ CZoneControl class -----------------------------------
 CZoneControl::CZoneControl() {
	m_iZone = -1;
	m_iPoint = -1;
}
// --------------------------------------------------------------------------
bool CZoneControl::Initialize(CNewVisionDoc* doc) {
	if (m_hWnd) {
		ZoneDialog* tab = (ZoneDialog*)GetParent();
		switch (tab->m_viewType) {
			case ZONE_FRAME_VIEW:
				image.Destroy();
				image.CopyOf(imgFrame);
				break;
			case ZONE_FLOOR_VIEW:
				image.Destroy();
				image.CopyOf(floorImg);
				cvZero(image.GetImage());
				tab->doc->floormodel.Draw(image.GetImage());
		}
	
		// Show scroll bars
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
void CZoneControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);
	ZoneDialog* tab = ((ZoneDialog*)this->GetParent());
	CPoint off = CPoint(tab->m_hbar.GetScrollPos(), tab->m_vbar.GetScrollPos());
	point += off;
	switch (tab->m_viewType) {
		case ZONE_FRAME_VIEW:
			if (tab->m_creatingNew) {
				CvPoint3D32f newPoint = tab->doc->cameramodel.coordsImage2Real(cvPoint2D32f(point));
				tab->zone.SetAt(++(tab->last_id), new FloorRegion(tab->last_id, tab->doc));
				m_iZone = tab->last_id;
				tab->zone[m_iZone]->p.Add(cvPoint2D32f(newPoint.x, newPoint.y));
				m_iPoint = 0;
			}
			else {
				if (PointInDotFrame(point, m_iZone, m_iPoint) != -1) {
					;// nothing to do m_iPoint was already updated
				}
				else if (m_iZone >= 0) {
					CvPoint3D32f newPoint = tab->doc->cameramodel.coordsImage2Real(cvPoint2D32f(point));
					tab->zone[m_iZone]->p.Add(cvPoint2D32f(newPoint.x, newPoint.y));
					m_iPoint = tab->zone[m_iZone]->p.GetUpperBound();
				}
			}
			break;
		case ZONE_FLOOR_VIEW:
			if (tab->m_creatingNew) {
				CvPoint3D32f newPoint = tab->doc->floormodel.coordsFloor2Real(cvPoint(point));
				tab->zone.SetAt(++(tab->last_id), new FloorRegion(tab->last_id, tab->doc));
				m_iZone = tab->last_id;
				tab->zone[m_iZone]->p.Add(cvPoint2D32f(newPoint.x, newPoint.y));
				m_iPoint = 0;
			}
			else {
				if (PointInDotFloor(point, m_iZone, m_iPoint) != -1) {
					;// nothing to do m_iPoint was already updated
				}
				else if (m_iZone >= 0) {
					CvPoint3D32f newPoint = tab->doc->floormodel.coordsFloor2Real(cvPoint(point));
					m_iPoint = PointInsertIndex(newPoint, m_iZone); // determine where to add a new point
					tab->zone[m_iZone]->p.InsertAt(m_iPoint, cvPoint2D32f(newPoint.x, newPoint.y));
					//m_iPoint = tab->zone[m_iZone]->p.GetUpperBound();
				}
			}
	}
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void CZoneControl::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);
	ZoneDialog* tab = (ZoneDialog*)GetParent();
	if (m_iZone != -1 && (nFlags & MK_LBUTTON)) {
		CPoint off = CPoint(tab->m_hbar.GetScrollPos(), tab->m_vbar.GetScrollPos());
		point += off;
		switch (tab->m_viewType) {
			case ZONE_FRAME_VIEW: {
				CvPoint3D32f newPoint = tab->doc->cameramodel.coordsImage2Real(cvPoint2D32f(point));
				tab->zone[m_iZone]->p[m_iPoint] = cvPoint2D32f(newPoint.x, newPoint.y);
				break;
			}
			case ZONE_FLOOR_VIEW:
				CvPoint3D32f newPoint = tab->doc->floormodel.coordsFloor2Real(cvPoint(point));
				tab->zone[m_iZone]->p[m_iPoint] = cvPoint2D32f(newPoint.x, newPoint.y);
		}
		Invalidate();
		UpdateWindow();
	}
}
// --------------------------------------------------------------------------
void CZoneControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);
	ZoneDialog* tab = (ZoneDialog*)GetParent();
	m_iPoint = -1;
	tab->m_creatingNew = 0;
	tab->UpdateData(false);
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void CZoneControl::OnLButtonDblClk(UINT nFlags, CPoint point) {
	// prompt for label
	ZoneDialog* tab = (ZoneDialog*)GetParent();
	CPoint off = CPoint(tab->m_hbar.GetScrollPos(), tab->m_vbar.GetScrollPos());
	point += off;
	switch (tab->m_viewType) {
		case ZONE_FRAME_VIEW:
			{
				int match = PointInDotFrame(point, m_iZone, m_iPoint);
				if (match != -1) {
					point -= off;
					drawEditableToolTip(this, tab->zone[m_iZone]->label, CRect(point, CSize(80,16)));								
				}
			}
			break;
		case ZONE_FLOOR_VIEW:
			{
				int match = PointInDotFloor(point, m_iZone, m_iPoint);
				if (match != -1) {
					point -= off;
					drawEditableToolTip(this, tab->zone[m_iZone]->label, CRect(point, CSize(80,16)));								
				}
			}
	}
}
// --------------------------------------------------------------------------
void CZoneControl::OnKeyDown(UINT nChar, UINT nRepCnt,	UINT nFlags) {
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	if (nChar == VK_DELETE && m_iZone != -1 && m_iPoint != -1) {
		ZoneDialog* tab = (ZoneDialog*)GetParent();
		FloorRegion *z;
		if (tab->zone.Lookup(m_iZone, z)) {  
			z->p.RemoveAt(m_iPoint); // delete one point from the current zone
			if (!z->p.GetCount()) { // zone is empty
				tab->zone.RemoveKey(m_iZone);
				m_iZone = -1; m_iPoint = -1;			
				if (m_iZone == tab->last_id)
					tab->last_id--;
			}
		}
		UpdateData(false);
		Invalidate();
		UpdateWindow();
	}
}
// --------------------------------------------------------------------------
int CZoneControl::PointInDotFrame(CPoint point, int &iZone, int &iPoint) {
	ZoneDialog* tab = (ZoneDialog*)GetParent();
	POSITION pos = tab->zone.GetStartPosition();
	while (pos) {
		int i; FloorRegion *z;
		tab->zone.GetNextAssoc(pos, i, z);
		int j = z->FramePointIn(cvPoint(point));
		if (j != -1) {
			Invalidate();
			UpdateWindow();
			iZone = i; iPoint = j;
			return i; // return the index of the match found
		}
	}
	return -1; // no match found
}
// --------------------------------------------------------------------------
int CZoneControl::PointInDotFloor(CPoint point, int &iZone, int &iPoint) {
	ZoneDialog* tab = (ZoneDialog*)GetParent();
	POSITION pos = tab->zone.GetStartPosition();
	while (pos) {
		int i; FloorRegion *z;
		tab->zone.GetNextAssoc(pos, i, z);
		int j = z->FloorPointIn(cvPoint(point));
		if (j != -1) {
			Invalidate();
			UpdateWindow();
			iZone = i; iPoint = j;
			return i; // return the index of the match found
		}
	}
	return -1; // no match found
}
// --------------------------------------------------------------------------
int CZoneControl::PointInsertIndex(CvPoint3D32f p3d, int iZone) {
	ZoneDialog* tab = (ZoneDialog*)GetParent();
	FloorRegion *z;
	if (!tab->zone.Lookup(iZone, z))
		return 0;


	// find the closest point
	CvPoint2D32f point = cvPoint2D32f(p3d.x, p3d.y);
	int minI = 0;
	double minDist = d(point, z, &minI);


	// insert before or after?
	int prevI = (minI == 0) ? z->p.GetUpperBound() : minI-1;
	int nextI = (minI == z->p.GetUpperBound()) ? 0 : minI+1;
	double dPrev = d(z->p[prevI], point);
	double dNext = d(z->p[nextI], point);
	if (dPrev < dNext)
		return minI;
	else 
		return nextI; 
}
// --------------------------------------------------------------------------
void CZoneControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CPen bNew(PS_SOLID, 1, RGB(250, 200, 0));
	ZoneDialog* tab = (ZoneDialog*)GetParent();
	image.Show(pDC->m_hDC, 0, 0, image.Width(), image.Height(), tab->offsetx, tab->offsety);
	switch (tab->m_viewType) {
		case ZONE_FRAME_VIEW: {
			// draw obstacles
			tab->doc->obstaclemodel.DrawFrame(pDC, tab->offsetx, tab->offsety, &bNew);
			// draw zones
			tab->DrawFrame(pDC, CPoint(tab->offsetx, tab->offsety));
			break;
		}
		case ZONE_FLOOR_VIEW: {
			// draw obstacles
			tab->doc->obstaclemodel.DrawFloor(pDC, tab->offsetx, tab->offsety, &bNew);
			// draw zones
			tab->DrawFloor(pDC, CPoint(tab->offsetx, tab->offsety));
		}
	}
}
// --------------------------------------------------------------------------
void ZoneDialog::OnBnClickedButtonZoneFromObstacle()
{
	CTextEntryDlg dlgTextEntry;
	if (!AfxMessageBox("This will create a number of interest zones based on existing obstacles. \nAre you sure?", MB_ICONEXCLAMATION | MB_YESNO))
		return;
	int span = 0;
	if (dlgTextEntry.Show(theApp.m_pMainWnd, "Obstacle border span", "How wide does the zone span beyond the obstacle? (cm): ") == IDOK) {
		CString strResult = dlgTextEntry.GetText();
		span = _atoi64(strResult);
	}

	// Go through obstacles array
	for (int i=0;i<doc->obstaclemodel.obstacle.GetCount();i++) {
		FloorRegion *z = new FloorRegion(++last_id, doc);
		zone.SetAt(last_id, z);
		
		// NOTE! Now only works for Cuboid obstacles
		Cuboid *cube;
		if (doc->obstaclemodel.obstacle[i]->IsKindOf(RUNTIME_CLASS( Cuboid )))
			cube = (Cuboid*)(doc->obstaclemodel.obstacle[i]);
		else 
			continue;
		
		z->p.Add(cvPoint2D32f(cube->v[0].x,cube->v[0].y));
		z->p.Add(cvPoint2D32f(cube->v[1].x,cube->v[1].y));
		z->p.Add(cvPoint2D32f(cube->v[2].x,cube->v[2].y));
		z->p.Add(cvPoint2D32f(cube->v[3].x,cube->v[3].y));
		z->label = cube->label;
		z->AddBorder(span);
	}
	UpdateData(false);
}
// --------------------------------------------------------------------------
void ZoneDialog::OnBnClickedButtonZoneNew()
{
	UpdateData();	
}

void ZoneDialog::OnBnClickedZoneFrameView()
{
	UpdateData();
	m_image.Initialize(this->doc);
}

void ZoneDialog::OnBnClickedZoneFloorView()
{
	UpdateData();
	m_image.Initialize(this->doc);
}
