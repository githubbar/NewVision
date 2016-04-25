#include "stdafx.h"
#include "afxtempl.h"
#include "NewVisionDoc.h"
#include "DoorDialog.h"
#include "Globals.h"

BEGIN_MESSAGE_MAP(DoorDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTONDOORREMOVE, OnBnClickedButtondoorRemove)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDoorControl, CButton)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// --------------------------------------------------------------------------
DoorDialog::DoorDialog(CWnd* pParent /*=NULL*/)
	: CPropertyPage(DoorDialog::IDD)
{
}
// --------------------------------------------------------------------------
DoorDialog::~DoorDialog()
{
}
// --------------------------------------------------------------------------
bool DoorDialog::Initialize() {
	if (!m_image.Initialize(doc))
		return false;
	if (m_doorType == DOORS_INTEREST_REGION || m_doorType == DOORS_INTEREST_BOUNDARY || m_doorType == DOORS_COUNTER) {
		// create interest region mask
		cvZero(door_mask);
		Rasterize(door_mask, cvScalar(1));
	}
	return true;
}
// --------------------------------------------------------------------------
void DoorDialog::DeInitialize() {
	m_doorType = DOORS_UNIFORM;
	nInVertices = 0;
	door.RemoveAll();
}
// --------------------------------------------------------------------------
BOOL DoorDialog::OnSetActive() {
	CPropertyPage::OnSetActive();
	return m_image.Initialize(doc);
}
// --------------------------------------------------------------------------
void DoorDialog::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_DOORIMAGE, m_image);
	DDX_Control(pDX, IDC_DOOR_SCROLLBAR1, m_vbar);
	DDX_Control(pDX, IDC_DOOR_SCROLLBAR2, m_hbar);
	DDX_Radio(pDX, IDC_DOORS_UNIFORM, m_doorType);
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void DoorDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
void DoorDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
void DoorDialog::OnBnClickedButtondoorRemove()
{
	door.RemoveAll();
	nInVertices = 0;
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void DoorDialog::Serialize( CArchive& ar )
 {
	DoorModel::Serialize( ar );
 }

//==========================================================================
// ------------------ CDoorControl class -----------------------------------
 CDoorControl::CDoorControl() {
}
// --------------------------------------------------------------------------
bool CDoorControl::Initialize(CNewVisionDoc* doc) {
	// Get first frame from file or camera
	image.CopyOf(imgFrame);
	// Show scroll bars
	if (m_hWnd) {
		DoorDialog* tab = (DoorDialog*)GetParent();
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
void CDoorControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);
	DoorDialog* tab = ((DoorDialog*)this->GetParent());
	CPoint off = CPoint(tab->m_hbar.GetScrollPos(), tab->m_vbar.GetScrollPos());
	int match = PointInDot(point+off);
	if (match != -1) {
		m_current = match;
	}
	else {
		CvPoint3D32f newDoor = tab->doc->cameramodel.coordsImage2Real(cvPoint2D32f(point.x+off.x, point.y+off.y));
		if (nFlags & MK_CONTROL) {
			tab->door.Add(cvPoint2D32f(newDoor.x, newDoor.y));
			m_current = tab->door.GetCount()-1;
		}
		else {
			tab->door.InsertAt(tab->nInVertices, cvPoint2D32f(newDoor.x, newDoor.y));
			m_current = tab->nInVertices++;
		}
	}
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void CDoorControl::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);
	if (m_current != -1 && (nFlags & MK_LBUTTON)) {
		DoorDialog* tab = (DoorDialog*)GetParent();
		CPoint off = CPoint(tab->m_hbar.GetScrollPos(), tab->m_vbar.GetScrollPos());
		CvPoint3D32f newDoor = tab->doc->cameramodel.coordsImage2Real(cvPoint2D32f(point.x+off.x, point.y+off.y));
		tab->door[m_current] = cvPoint2D32f(newDoor.x, newDoor.y);
		Invalidate();
		UpdateWindow();
	}
}
// --------------------------------------------------------------------------
void CDoorControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);
	m_current = -1;
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void CDoorControl::OnKeyDown(UINT nChar, UINT nRepCnt,	UINT nFlags) {
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	if (nChar == VK_DELETE && m_current != -1) {
		DoorDialog* tab = (DoorDialog*)GetParent();
		tab->door.RemoveAt(m_current);
		m_current = -1;
		Invalidate();
		UpdateWindow();
	}
}
// --------------------------------------------------------------------------
int CDoorControl::PointInDot(CPoint point) {
	DoorDialog* tab = (DoorDialog*)GetParent();
	for (int i=0;i<tab->door.GetCount();i++) {
		CvPoint2D32f p2 = tab->doc->cameramodel.coordsReal2Image(cvPoint3D32f(tab->door[i].x, tab->door[i].y, 0));
		if(d(cvPointFrom32f(p2), cvPoint(point)) <= tab->DOT) {
            Invalidate();
			UpdateWindow();
			return i; // return the index of the match found
		}
	}
	return -1; // no match found
}
// --------------------------------------------------------------------------
void CDoorControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	DoorDialog* dlg=((DoorDialog*)this->GetParent());
	image.Show(pDC->m_hDC, 0, 0, image.Width(), image.Height(), dlg->offsetx, dlg->offsety);
	dlg->DrawFrame(pDC, CPoint(dlg->offsetx, dlg->offsety));
}