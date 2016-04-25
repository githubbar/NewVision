// NewVisionView.cpp : implementation of the CNewVisionView class

#include "stdafx.h"
#include "MainFrm.h"
#include "NewVision.h"
#include "NewVisionDoc.h"
#include "NewVisionView.h"
#include "TrackerModel.h"
#include "NewVisionView.h"
#include "Globals.h"
#include "Body.h"
#include "BodyPath.h"

#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::Diagnostics;


// CNewVisionView

IMPLEMENT_DYNCREATE(CNewVisionView, CZoomView)

BEGIN_MESSAGE_MAP(CNewVisionView, CZoomView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_VIEW_FULLSCREEN, OnViewFullscreen)
END_MESSAGE_MAP()

// CNewVisionView construction/destruction
// --------------------------------------------------------------------------
CNewVisionView::CNewVisionView(ViewType type)
{
	m_type = type;
	image = NULL;
	m_rect = false;
	m_drag = false;
	m_rotate = false;
	m_click = false;
	m_fullScreen = false;
	m_wasRunning = false;
}
// --------------------------------------------------------------------------
CNewVisionView::CNewVisionView()
{
	image = NULL;
	m_rect = false;
	m_drag = false;
	m_rotate = false;
	m_click = false;
	m_fullScreen = false;
	m_wasRunning = false;
}
// --------------------------------------------------------------------------
CNewVisionView::~CNewVisionView()
{
}
// --------------------------------------------------------------------------
void CNewVisionView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CSize cameraView= CSize(GetDocument()->cameramodel.w, GetDocument()->cameramodel.h);
	CSize floorView = CSize(GetDocument()->floormodel.w, GetDocument()->floormodel.h);
	switch (m_type) {
		case VIDEO_VIEW :
			SetScrollSizes(MM_TEXT,  cameraView, CSize(cameraView.cx/10, cameraView.cy/10), CSize(1, 1));
			break;
		case SCENE_VIEW :
			SetScrollSizes(MM_TEXT,  cameraView, CSize(cameraView.cx/10, cameraView.cy/10), CSize(1, 1));
			break;
		case ACTIVITY_VIEW :
			SetScrollSizes(MM_TEXT,  cameraView, CSize(cameraView.cx/10, cameraView.cy/10), CSize(1, 1));
			break;
		case FLOOR_VIEW :
			SetScrollSizes(MM_TEXT,  floorView, CSize(floorView.cx/10, floorView.cy/10), CSize(1, 1));
			break;
		default :
			SetScrollSizes(MM_TEXT,  cameraView, CSize(cameraView.cx/10, cameraView.cy/10), CSize(1, 1));
	}
	CZoomView::OnUpdate(pSender, lHint, pHint);
}
// --------------------------------------------------------------------------
void CNewVisionView::OnContextMenu(CWnd* pWnd, CPoint pos) {
	// handle body right click menu
	CPoint point = pos;
	pWnd->ScreenToClient(&point);
	point = ClientToImage(point);
	{ // +++ begin data processing critical section	
		CAutoLock dataLock( &dataCriticalSection); 
		m_match = GetDocument()->bodymodel.FindBodyIDTag(GetDocument()->bodymodel.body, cvPoint2D32f(point));
	} // +++ end data processing critical section	
	if (m_match >= 0) {
		m_bodyMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x , pos.y, this);
		return;
	}

	// handle display right click menu
	CMenu *submenu, *menu = &m_displayMenu;
	switch (m_type) {
		case VIDEO_VIEW :
			submenu = menu->GetSubMenu(0);break;
		case SCENE_VIEW :
			submenu = menu->GetSubMenu(1);break;
		case ACTIVITY_VIEW :
			submenu = menu->GetSubMenu(2);break;
		case FLOOR_VIEW :
			submenu = menu->GetSubMenu(3);break;
		default:
			submenu = menu->GetSubMenu(0);
	}
    submenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x , pos.y, this);
}
// --------------------------------------------------------------------------
void CNewVisionView::OnLButtonDown(UINT nFlags, CPoint pos)
{
	m_click = true;
	CWnd::OnLButtonDown(nFlags, pos);
	CPoint point = ClientToImage(pos);
	CNewVisionDoc* pDoc = GetDocument();
	if (m_type == VIDEO_VIEW && pDoc && pDoc->trackermodel.m_initialized) {
		if (pDoc->trackermodel.IsRunning()) {
			m_wasRunning = true;
			pDoc->OnPause(); 
			//// pause and wait for the filter thread to pause
			//pDoc->trackermodel.IsPaused();
		}
		else                            
			m_wasRunning = false;
	
		CAutoLock dataLock( &dataCriticalSection); 
		BodyModel* bm = &pDoc->bodymodel;
		m_match = bm->FindBodyIDTag(bm->body, cvPoint2D32f(point));
		// On Ctrl+click toggle body path manual confidence to zero
		if (pDoc->m_ProcessingMode == PLAYBACK && (nFlags & MK_CONTROL) ) {
			BodyPath *bp;
			if (pDoc->m_TrackingData.data.Lookup(m_match, bp)) {
				if (bp->isManual)
					bp->isManual = false;
				else {
					bp->isManual = true;
					bp->manualConfidence = 0;
				}
			}
			m_match = -1;
		}
		// On Shift+Drag rotate the body		
		else if (pDoc->m_ProcessingMode == PLAYBACK && (nFlags & MK_SHIFT) ) {
			m_rotate = true;
		}
		else if (m_match >= 0) {
			m_drag = true;
		}
		else {
			m_click_point = cvPoint(point); 
		}
	}
}
// --------------------------------------------------------------------------
void CNewVisionView::OnLButtonUp(UINT nFlags, CPoint pos) {
	CWnd::OnLButtonUp(nFlags, pos);
	if (m_click) {m_click = false;} else {return;}
	CNewVisionDoc* pDoc = GetDocument();
	CPoint point = ClientToImage(pos);
	if (point.x > pDoc->trackermodel.cx || point.y > pDoc->trackermodel.cy)
		return;
	if (m_type == VIDEO_VIEW && pDoc && pDoc->trackermodel.m_initialized) {
		if (pDoc->m_ProcessingMode == PLAYBACK && (nFlags & MK_CONTROL) ) {
		}
		else if (pDoc->m_ProcessingMode == PLAYBACK && (nFlags & MK_SHIFT) ) {
			if (m_rotate) {
				CAutoLock dataLock( &dataCriticalSection); 
				m_rotate = false;
				Body* b;
				if (!pDoc->bodymodel.body.Lookup(m_match, b))
					return;
				CvPoint3D32f p2 = pDoc->cameramodel.coordsImage2Real(cvPoint2D32f(point));
				CvPoint3D32f p1 = b->GetFoot();
				double cc =  p2.x-p1.x;
				double ss = p2.y - p1.y;
				b->orientation = atan2(ss, cc);
				pDoc->m_TrackingData.Put((int)pDoc->trackermodel.m_frameNumber);
			}
		}
		else if (m_drag) {
			CAutoLock dataLock( &dataCriticalSection); 
			m_drag = false;
			pDoc->m_TrackingData.Put((int)pDoc->trackermodel.m_frameNumber);
		}
		else {
			CAutoLock dataLock( &dataCriticalSection); 
			/*
			CvPoint3D32f head;
			double zz = pDoc->cameramodel.coordsImage2RealSameXY_Feet2Floor(cvPointTo32f(m_click_point), cvPoint2D32f(point), &head);
			pDoc->test1 = pDoc->cameramodel.coordsReal2Image(head);
			pDoc->test2 = pDoc->cameramodel.coordsReal2Image(pDoc->cameramodel.coordsImage2Real(cvPoint2D32f(point)));
*/


			CvPoint3D32f p2 = pDoc->cameramodel.coordsImage2Real(cvPoint2D32f(point));
			BlobRay br; br.p1 = m_click_point; br.p2 = cvPoint(point);
			// added last_id ++ here!

			pDoc->bodymodel.body[pDoc->bodymodel.last_id] = new Body(&br, pDoc, ++pDoc->bodymodel.last_id, (int)pDoc->trackermodel.m_frameNumber);
			pDoc->m_TrackingData.Put((int)pDoc->trackermodel.m_frameNumber);
			if (pDoc->m_ProcessingMode == PLAYBACK && pDoc->m_DefaultTrackID != -1) {
				m_match = pDoc->bodymodel.last_id;
				pDoc->m_TrackingData.MergeID(pDoc->m_DefaultTrackID, m_match);

			}

		}
		//pDoc->UpdateOverlayInfo();		
		pDoc->trackermodel.SetPosition();
		Invalidate(); UpdateWindow();
		if (m_wasRunning)
			pDoc->OnPlay();
	}
}
// --------------------------------------------------------------------------
void CNewVisionView::OnMouseMove(UINT nFlags, CPoint pos) {
	CWnd::OnMouseMove(nFlags, pos);
	CNewVisionDoc* pDoc = GetDocument();
	CPoint point = ClientToImage(pos);
// 	CPoint pos2 = ImageToClient(point);
// 	m_cClickPoint = point;
// 	Invalidate();
// 	UpdateWindow();
// 	char c[40];
// 	sprintf(c, "(%d:%d)>(%d:%d)>(%d:%d)", pos.x, pos.y, point.x, point.y, pos2.x, pos2.y);
// 	m_wndStatusBar.SetPaneText(FRAME_NUMBER_STATUS_POS, c);
	if (m_drag && m_match >= 0 && m_type == VIDEO_VIEW && pDoc && pDoc->trackermodel.m_initialized) {
		CAutoLock dataLock( &dataCriticalSection); 
		BodyModel* bm = &pDoc->bodymodel;
		Body *b; bm->body.Lookup(m_match, b);
		CvPoint3D32f p = pDoc->cameramodel.coordsImage2Real(cvPoint2D32f(point), b->top.z);
		b->MoveHeadTo(p);
	}
}
// --------------------------------------------------------------------------
BOOL CNewVisionView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point )
{
	CWnd::OnMouseWheel(nFlags, zDelta, point);
	GetDocument()->cameramodel.m_lambda += 1.0/1200.0*zDelta;
	GetDocument()->UpdateOverlayInfo();	Invalidate(); UpdateWindow();
	return true;
}
// --------------------------------------------------------------------------
void CNewVisionView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	if (nChar == VK_DELETE && m_type == VIDEO_VIEW && m_drag  && m_match >= 0) {
		CNewVisionDoc* pDoc = GetDocument();
		if (pDoc->m_cursor == CURSOR_DEFAULT ) {
			pDoc->OnBodyDeleteID();
			m_match = -1; // no body to match
			pDoc->trackermodel.SetPosition();
			Invalidate(); UpdateWindow();
			if (m_wasRunning)
				pDoc->OnPlay();
		}
	}
}
// --------------------------------------------------------------------------
BOOL CNewVisionView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}
// --------------------------------------------------------------------------
// CNewVisionView drawing
//BOOL CNewVisionView::OnEraseBkgnd(CDC* pDC) 
//{
//	return TRUE;
//}
// --------------------------------------------------------------------------
void CNewVisionView::OnDraw(CDC* pDC)
{
	// display poster frame
	CNewVisionDoc *doc = GetDocument();
	if(doc->m_initialized) {
		switch (m_type) {
			case FLOOR_VIEW :
				ShowImage(pDC, floorImg);
				break;
			default:
				doc->trackermodel.ShowPosterFrame(this->m_hWnd, pDC->m_hDC);
				//CPen *pen = new CPen(PS_SOLID, 2, RGB(0,0,255));
				//displayVideoBodies ? doc->bodymodel.DrawFrame(pDC, 0, 0, pen) : 1; 	
				//delete pen;
		}	
	}
}
// --------------------------------------------------------------------------
void CNewVisionView::ShowImage(CDC* pDC, IplImage* pIplImage)
{
	// Works only for 3-channel images !
	CImage pImage;
	pImage.CopyOf(pIplImage);
	pImage.Show(pDC->m_hDC, 0, 0, pImage.Width(), pImage.Height(), 0,0);
}

// CNewVisionView printing
// --------------------------------------------------------------------------
BOOL CNewVisionView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}
// --------------------------------------------------------------------------
void CNewVisionView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}
// --------------------------------------------------------------------------
void CNewVisionView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

// --------------------------------------------------------------------------
// CNewVisionView diagnostics
#ifdef _DEBUG
void CNewVisionView::AssertValid() const
{
	CView::AssertValid();
}
// --------------------------------------------------------------------------
void CNewVisionView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
// --------------------------------------------------------------------------
CNewVisionDoc* CNewVisionView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNewVisionDoc)));
	return (CNewVisionDoc*)m_pDocument;
}
#endif //_DEBUG
// --------------------------------------------------------------------------
void CNewVisionView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) {
	if (bActivate && m_pDocument) {
		CNewVisionDoc *doc = GetDocument();
		m_layout.cx = doc->trackermodel.cx;
		m_layout.cy = doc->trackermodel.cy;
		if (!doc->m_initialized)
			return;
		doc->trackermodel.SetVideoWindow(GetSafeHwnd());
		CPoint p1 = this->GetScrollPosition();
		switch (((CMainFrame*)(theApp.m_pMainWnd))->GetActivePane()->m_type) {
			case VIDEO_VIEW :
				m_layout.cx = doc->trackermodel.cx;
				m_layout.cy = doc->trackermodel.cy;
				SetDisplaySize();
				doc->trackermodel.SetBitmapImage(doc->trackermodel.m_bitmap, doc->trackermodel.cx, doc->trackermodel.cy);
				doc->trackermodel.SetVideoPosition(new CRect(CPoint(-p1.x, -p1.y), m_displaySize), false);
				break;
			case SCENE_VIEW :
				m_layout.cx = doc->trackermodel.cx;
				m_layout.cy = doc->trackermodel.cy;
				SetDisplaySize();
				doc->trackermodel.SetTransparency(100);
				doc->trackermodel.SetBitmapImage(doc->trackermodel.m_bitmap, doc->trackermodel.cx, doc->trackermodel.cy);
				doc->trackermodel.SetVideoPosition(new CRect(0,0,doc->trackermodel.cx, doc->trackermodel.cy), false);
				break;
			case ACTIVITY_VIEW :
				m_layout.cx = doc->trackermodel.cx;
				m_layout.cy = doc->trackermodel.cy;
				SetDisplaySize();
				doc->trackermodel.SetBitmapImage(doc->trackermodel.m_bitmap, doc->trackermodel.cx, doc->trackermodel.cy);
				doc->trackermodel.SetVideoPosition(new CRect(0,0,doc->trackermodel.cx, doc->trackermodel.cy), false);
				break;
			case FLOOR_VIEW :
				m_layout.cx = doc->trackermodel.fcx;
				m_layout.cy = doc->trackermodel.fcx;
				SetDisplaySize();
				doc->trackermodel.SetTransparency(100); 
				doc->trackermodel.SetBitmapImage(doc->trackermodel.m_floorBitmap, doc->trackermodel.fcx, doc->trackermodel.fcy);
				//doc->trackermodel.SetVideoPosition(new CRect(0,0,doc->trackermodel.fcx, doc->trackermodel.fcy), false);
				doc->trackermodel.SetVideoPosition(new CRect(0,0,0,0), false);
				break;	
			default:
				;
		}
	}
}
// --------------------------------------------------------------------------
void CNewVisionView::OnViewFullscreen()
{
    /* if normal mode - start (enable) full screen mode */
    if(!m_fullScreen){
        m_fullScreen = true; 

        /* Variable saveParent is used to save 
        address of parent window (address is necessary 
        in disabling this mode -to place view in former position) */
        m_savedParent = this->GetParent();

        /* this (our view) is placed on desktop window -
        CWnd::GetDesktopWindow() - Returns the Windows desktop window */
        this->SetParent(GetDesktopWindow());

        CRect rect; /* temporary variable */

        /* Next function copies the dimensions of 
        the bounding rectangle of the desktop */
        GetDesktopWindow()->GetWindowRect(&rect);

        /* view is placed on desktop */
        this->SetWindowPos(&wndTopMost,rect.left,rect.top,
                rect.right,rect.bottom,SWP_SHOWWINDOW);
		//this->GetDocument()->trackermodel.SetVideoPosition(&rect, true);
		SetZoomArea(&rect);
    }
    else {
        /* disable full screen mode */
        m_fullScreen = false;

        /* our view is placed in old window - using address saveParent 
        (pointer that has been used in former if-section) */
        this->SetParent(m_savedParent);

        /* pointer to the main frame and function RecalcLayout() 
        (run on behalf of CmainFrame object) 
        - that repositions all control bars 
        in the frame (#include <MainFrame.h>) */
        ((CMainFrame *)AfxGetMainWnd())->RecalcLayout();
		//this->GetDocument()->trackermodel.SetVideoPosition(new CRect(0,0,this->GetDocument()->trackermodel.cx,this->GetDocument()->trackermodel.cy), false);
		CRect rect(0,0,this->GetDocument()->trackermodel.cx,this->GetDocument()->trackermodel.cy);
		SetZoomArea(&rect);
		this->GetParentFrame()->SetActiveView(this);
    }
}
// --------------------------------------------------------------------------
BOOL CNewVisionView::OnScroll(UINT nSBCode, UINT nPos, BOOL bDoScroll) {
	SequenceProcessor *sp = &this->GetDocument()->trackermodel;
	CPoint p1 = this->GetScrollPosition();
	sp->SetVideoPosition(new CRect(CPoint(-p1.x, -p1.y), m_displaySize), false);
	return CZoomView::OnScroll(nSBCode,nPos,bDoScroll);
}
// --------------------------------------------------------------------------
void CNewVisionView::UpdateVideoPosition() {
	SequenceProcessor *sp = &GetDocument()->trackermodel;
	CPoint p1 = GetScrollPosition();
	if (sp->m_initialized) { // +++ begin data processing critical section	
		GetDocument()->Message(String::Format("x={0} y={1} cx={2} cy={3}", p1.x, p1.y, m_displaySize.cx, m_displaySize.cy));
		CRect r(CPoint(-p1.x, -p1.y), m_displaySize);
		sp->SetVideoPosition(&r, false);
	} // +++ end data processing critical section	

}
// --------------------------------------------------------------------------
//void CNewVisionView::OnInitialUpdate() {
//// initialize OpenGL
//	bindGL( this->GetDC() );
//	LadybugError error;
//	CNewVisionDoc* pDoc = GetDocument();
//	error = ladybugInitializeOpenGL(
//	  pDoc->m_context, 
//	  LADYBUG_NUM_CAMERAS,
//	  pDoc->m_uiRawCols,
//	  pDoc->m_uiRawRows,
//	  pDoc->m_uiTextureCols,
//	  pDoc->m_uiTextureRows );
//
//	unBindGL( this->GetDC() );
//}
// CNewVisionView message handlers

