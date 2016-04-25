#include "stdafx.h"
#include "afxtempl.h"
#include "NewVisionDoc.h"
#include "FloorDialog.h"
#include "Globals.h"
#include "NewVision.h"
#include "MainFrm.h"

#using <mscorlib.dll>
#using <System.dll>

using namespace System::IO;
using namespace System;

BEGIN_MESSAGE_MAP(FloorDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTONFLOORREMOVE, OnBnClickedRemove)
	ON_BN_CLICKED(IDC_FLOORMODELSELECTBUTTON, OnBnClickedFloorModelSelectButton)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CFloorControl, CButton)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// --------------------------------------------------------------------------
FloorDialog::FloorDialog(CWnd* pParent /*=NULL*/)
	: CPropertyPage(FloorDialog::IDD), FloorModel()
{
}
// --------------------------------------------------------------------------
FloorDialog::~FloorDialog()
{
}
// --------------------------------------------------------------------------
bool FloorDialog::Initialize() {
	CvSize sz = cvSize(w, h);
	floorImg = cvCreateImage(sz, IPL_DEPTH_8U, 3);	
	floormap = cvCreateImage(sz, IPL_DEPTH_8U, 3);	
	motionmap = cvCreateImage(sz, IPL_DEPTH_32F, 1);	
	motionmapscaled = cvCreateImage(sz, IPL_DEPTH_8U, 3);
	motionmaptemp = cvCreateImage(sz, IPL_DEPTH_8U, 1);	
	fbuffer = cvCreateImage(sz, IPL_DEPTH_8U, 1);
	cvSet(motionmap, cvScalar(0));
	cvSet(floorImg, cvScalar(255, 255, 255));
	cvZero(fbuffer);

	if (!m_image.Initialize(doc))
		return false;
	
	// check if there are enough extreme points
	if (m_extreme.GetCount() < 2)
		return false;

	if (m_sourcename != "" && File::Exists(gcnew String(m_sourcename))) 
		floormap = cvLoadImage(m_sourcename);

	GetMinMaxXY(floormin, floormax);
	return true;
}
// --------------------------------------------------------------------------
void FloorDialog::DeInitialize() {
	cvReleaseImage(&floormap);
	cvReleaseImage(&floorImg);
	cvReleaseImage(&motionmap);
	cvReleaseImage(&motionmapscaled);
	cvReleaseImage(&motionmaptemp);
	cvReleaseImage(&fbuffer);
	m_extreme.RemoveAll();
	m_type = 0;
	m_sourcename = "";
	w = 800;
	h = 600;
}
// --------------------------------------------------------------------------
BOOL FloorDialog::OnSetActive() {
	CPropertyPage::OnSetActive();
	return m_image.Initialize(doc);
}
// --------------------------------------------------------------------------
void FloorDialog::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_FLOORIMAGE, m_image);
	DDX_Text(pDX, IDC_FLOORNAME, m_sourcename);
	DDX_Radio(pDX, IDC_RADIOFLOORIMAGE, m_type);
	DDX_Control(pDX, IDC_FLOOR_SCROLLBAR1, m_vbar);
	DDX_Control(pDX, IDC_FLOOR_SCROLLBAR2, m_hbar);
	DDX_Text(pDX, IDC_FLOORW, w);
	DDX_Text(pDX, IDC_FLOORH, h);

	DDV_MinMaxInt(pDX, w, 1, 10000);
	DDV_MinMaxInt(pDX, h, 1, 10000);
	
	GetMinMaxXY(floormin, floormax);
}
// --------------------------------------------------------------------------
void FloorDialog::OnBnClickedRemove()
{
	m_extreme.RemoveAll();
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void FloorDialog::OnBnClickedFloorModelSelectButton()
{
	UpdateData();
	if (m_type == 0) {
		CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, "BMP files (*.bmp)|*.bmp|JPEG files (*.jpg; *.jpeg)|*.jpeg; *.jpg|All Files (*.*)|*.*||", this);
		char title[]= {"Open Floor Plan Image File"};
		dlg.m_ofn.lpstrTitle = title;
		if (dlg.DoModal() == IDOK) {
			m_sourcename = dlg.GetPathName();
			floormap = cvLoadImage(m_sourcename);
		}
	}
	else {
	}
	if (!Initialize())
		m_sourcename = "";
	UpdateData(false);
	if (::IsWindow(theApp.m_pMainWnd->m_hWnd))
		theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
}
// --------------------------------------------------------------------------
void FloorDialog::Serialize( CArchive& ar )
 {
	FloorModel::Serialize( ar );
 }
// --------------------------------------------------------------------------
void FloorDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
	pScrollBar->SetScrollPos(curpos);
	m_image.Invalidate();
    CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
// --------------------------------------------------------------------------
void FloorDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
	pScrollBar->SetScrollPos(curpos);
	m_image.Invalidate();
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
//==========================================================================
// ------------------ CFloorControl class -----------------------------------
 CFloorControl::CFloorControl() {
}
// --------------------------------------------------------------------------
bool CFloorControl::Initialize(CNewVisionDoc* doc) {
	// Get first frame from file or camera
	image.CopyOf(imgFrame);
	// Show scroll bars
	if (m_hWnd) {
		FloorDialog* tab = (FloorDialog*)GetParent();
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
		this->Invalidate();
	}
	return true;
}
// --------------------------------------------------------------------------
void CFloorControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);
	FloorDialog* tab = (FloorDialog*)GetParent();
	CPoint off = CPoint(tab->m_hbar.GetScrollPos(), tab->m_vbar.GetScrollPos());
	int match = PointInDot(point+off);
	if (match != -1) {
		m_current = match;
	}
	else {
		tab->m_extreme.Add(cvPoint(point.x+off.x, point.y+off.y));
		m_current = tab->m_extreme.GetCount()-1;
	}
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void CFloorControl::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);
	if (m_current != -1 && (nFlags & MK_LBUTTON)) {
		FloorDialog* tab = (FloorDialog*)GetParent();
		if (tab->m_extreme.GetCount() == 0)
			return;
		CPoint off = CPoint(tab->m_hbar.GetScrollPos(), tab->m_vbar.GetScrollPos());
		tab->m_extreme[m_current].x = point.x+off.x;
		tab->m_extreme[m_current].y = point.y+off.y;
		Invalidate();
		UpdateWindow();	
	}
}
// --------------------------------------------------------------------------
void CFloorControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);
	m_current = -1;
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void CFloorControl::OnKeyDown(UINT nChar, UINT nRepCnt,	UINT nFlags) {
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	if (nChar == VK_DELETE && m_current != -1) {
		FloorDialog* tab = (FloorDialog*)GetParent();
		tab->m_extreme.RemoveAt(m_current);
		m_current = -1;
		Invalidate();
		UpdateWindow();
	}
}
// --------------------------------------------------------------------------
int CFloorControl::PointInDot(CPoint point) {
	FloorDialog* tab = (FloorDialog*)GetParent();
	for (int i=0;i<tab->m_extreme.GetCount();i++) {
		if(d(cvPoint(point), tab->m_extreme[i]) <= DOT) {
            Invalidate();
			UpdateWindow();
			return i; // return the index of the match found
		}
	}
	return -1; // no match found
}
// --------------------------------------------------------------------------
void CFloorControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	FloorDialog* tab = (FloorDialog*)GetParent();
	int ox = tab->m_hbar.GetScrollPos(); 
	int oy = tab->m_vbar.GetScrollPos();
	image.Show(pDC->m_hDC, 0, 0, image.Width(), image.Height(), ox, oy);
	CBrush bNew(RGB(255, 255, 0));
	CBrush* pOldBrush = pDC->SelectObject(&bNew);
	for (int i=0;i<tab->m_extreme.GetSize();i++) {
		CvPoint d = tab->m_extreme[i];
		pDC->Ellipse(d.x-DOT - ox, d.y-DOT - oy, d.x+DOT - ox, d.y+DOT - oy);
	}
	pDC->SelectObject(pOldBrush);
}