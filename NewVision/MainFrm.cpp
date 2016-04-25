// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "NewVision.h"
#include "MainFrm.h"
#include "NewVisionDoc.h"
#include "NewVisionView.h"
#include "Globals.h"
#include "AboutDialog.h" 
#include <psapi.h>


#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::Diagnostics;

// CMainFrame

BEGIN_MESSAGE_MAP(CTrackToolBar, CDialogBar)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CBlendToolBar, CDialogBar)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CZoomToolBar, CDialogBar)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_UPDATESTATUSBAR, OnUpdateStatusBar)
	ON_MESSAGE(WM_UPDATENAVBAR, OnUpdateNavigationBar)
	ON_MESSAGE(WM_UPDATEACTIVEVIEW, OnUpdateActiveView)
	ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphNotify)
	ON_MESSAGE(WM_LOADLASTFRAME, OnLoadLastFrame)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_MEM_INDICATOR,    
	ID_FRAME_INDICATOR,    
	ID_AUX_INDICATOR
};

// global toolbar variables
CTrackToolBar      m_wndTrackToolBar; 
CBlendToolBar      m_wndBlendToolBar;
CDialogBar         m_wndControlToolBar;
CZoomToolBar       m_wndZoomToolBar;
CReBar             m_wndPreferencesToolBar;
CTrueColorToolBar  m_wndToolBar;
CMenu		       m_displayMenu;
CMenu		       m_bodyMenu;
CMenu  		       m_settingsMenu;
CStatusBar         m_wndStatusBar;

// CMainFrame construction/destruction
// --------------------------------------------------------------------------
CMainFrame::CMainFrame()
{
	m_bAutoMenuEnable  = FALSE;
}
// --------------------------------------------------------------------------
CMainFrame::~CMainFrame()
{
}
// --------------------------------------------------------------------------
CNewVisionView* CMainFrame::GetActivePane() {
	return (CNewVisionView*) imgFramework.GetActivePane()->GetSafeWnd();
}
// --------------------------------------------------------------------------
CNewVisionView* CMainFrame::GetPane(int index) {
	return (CNewVisionView*) view[index]->GetSafeWnd();
}
// --------------------------------------------------------------------------
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT, CCreateContext* pContext) {
	TVisualObject *pTab = new TVisualObject(1,"",pContext,RUNTIME_CLASS(TTabWnd));
	view[0] = new TVisualObject(2,"Video",pContext,RUNTIME_CLASS(CNewVisionView));
	view[1] = new TVisualObject(3,"Scene", pContext,RUNTIME_CLASS(CNewVisionView));
	view[2] = new TVisualObject(4,"Activity", pContext,RUNTIME_CLASS(CNewVisionView));
	view[3] = new TVisualObject(5,"Floor",pContext,RUNTIME_CLASS(CNewVisionView));
	view[0]->SetIcon(IDI_MEN);
	view[1]->SetIcon(IDI_MEN);
	view[2]->SetIcon(IDI_MEN);
	view[3]->SetIcon(IDI_BALLS);

	imgFramework.Add(pTab);
	imgFramework.Add(pTab, view[0]);
	imgFramework.Add(pTab, view[1]);
	imgFramework.Add(pTab, view[2]);
	imgFramework.Add(pTab, view[3]);
	
	imgFramework.Create(this);
	
	((CNewVisionView*)(view[0]->GetWnd()))->m_type = VIDEO_VIEW;
	((CNewVisionView*)(view[1]->GetWnd()))->m_type = SCENE_VIEW;
	((CNewVisionView*)(view[2]->GetWnd()))->m_type = ACTIVITY_VIEW;
	((CNewVisionView*)(view[3]->GetWnd()))->m_type = FLOOR_VIEW;

	return TRUE; 
} 
// --------------------------------------------------------------------------
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableDocking(CBRS_ALIGN_ANY);

	// -------------- Add control toolbar -------------- 
	if (!m_wndControlToolBar.Create(this, IDD_CONTROL_TOOLBAR,
      WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_SIZE_DYNAMIC 
	  | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_CONTROL_TOOLBAR))
	{
      TRACE0("Failed to create control toolbar!\n");
      return -1;      // Fail to create.
	}
	// -------------- Add program toolbar -------------- 
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | 
        WS_VISIBLE | CBRS_TOP  | CBRS_GRIPPER | 
        CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !m_wndToolBar.LoadToolBar(IDR_FILE))
    {
        TRACE0("Failed to create program toolbar\n");
        return -1; //Failed to create
    }
    m_wndToolBar.LoadTrueColorToolBar(
		32,
        IDB_TOOLBAR_TC,
        IDB_TOOLBAR_TC_HOT,
		IDB_TOOLBAR_GR);

	// -------------- Add track toolbar -------------- 
	if (!m_wndTrackToolBar.Create(this, IDD_TRACKTOOLBAR, WS_CHILD | 
        WS_VISIBLE | CBRS_TOP  | CBRS_GRIPPER | 
        CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, IDD_TRACKTOOLBAR))
    {
        TRACE0("Failed to create track toolbar\n");
        return -1; //Failed to create
    }
	CScrollBar* scroller = (CScrollBar*)m_wndTrackToolBar.GetDlgItem(ID_SCROLLER);
	SCROLLINFO horz;
	horz.cbSize = sizeof(SCROLLINFO);
	horz.fMask = SIF_ALL;
	horz.nPage = 50;
	horz.nMin = 0;
	horz.nMax = 1000;
	horz.nPos = 0;
	horz.nTrackPos = 0;
	scroller->SetScrollInfo(&horz);
	// -------------- Add zoom toolbar -------------- 
	if (!m_wndZoomToolBar.Create(this, IDD_ZOOMTOOLBAR,
		WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_SIZE_DYNAMIC 
		| CBRS_TOOLTIPS | CBRS_FLYBY, IDD_CONTROL_TOOLBAR))
	{
		TRACE0("Failed to create zoom toolbar!\n");
		return -1;      // Fail to create.
	}

	// -------------- Add blend toolbar -------------- 
	if (!m_wndBlendToolBar.Create(this, IDD_BLENDTOOLBAR, WS_CHILD | 
        WS_VISIBLE | CBRS_TOP  | CBRS_GRIPPER | 
        CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, IDD_BLENDTOOLBAR))
    {
        TRACE0("Failed to create blend toolbar\n");
        return -1; //Failed to create
    }
	//CSliderCtrl* slider = (CSliderCtrl*)m_wndBlendToolBar.GetDlgItem(ID_SLIDER);
	//slider->SetRange(0, 100);
	//slider->SetPos(100);
	//slider->SetPageSize(10);

	// -------------- Add preferences toolbar -------------- 
	// , , WS_CHILD | WS_VISIBLE | CBRS_BOTTOM  | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, AFX_IDW_REBAR
	//if (!m_wndPreferencesToolBar.Create(this, RBS_BANDBORDERS, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
 //   {
 //       TRACE0("Failed to create track toolbar\n");
 //       return -1; //Failed to create
 //   }
	//CNewVisionDoc* doc = ((CNewVisionDoc*)this->GetActiveDocument());
	//m_wndPreferencesToolBar.AddBar(&(doc->settings));

	// -------------- Add DropDown Button ---------------
    // m_wndToolBar.AddDropDownButton(this, ID_BUTTON1, IDR_MENU1);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);	
	m_wndControlToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBarLeftOf(&m_wndControlToolBar, &m_wndToolBar);
	m_wndTrackToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBarLeftOf(&m_wndTrackToolBar, &m_wndControlToolBar);
	m_wndZoomToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBarLeftOf(&m_wndZoomToolBar, &m_wndTrackToolBar);

//	m_wndPreferencesToolBar.EnableDocking(CBRS_ALIGN_BOTTOM);
//	DockControlBar(&m_wndPreferencesToolBar);	


	// -------------- Create status bar -------------- 
	if (!m_wndStatusBar.Create(this))
	{
		AfxMessageBox("Failed to create status bar\n");
		return -1;      // fail to create
	}
	
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	m_wndStatusBar.SetPaneInfo(MEM_INFO_STATUS_POS, ID_MEM_INDICATOR, SBPS_NORMAL, 100);
	m_wndStatusBar.SetPaneInfo(FRAME_NUMBER_STATUS_POS, ID_FRAME_INDICATOR, SBPS_NORMAL, 100);
	m_wndStatusBar.SetPaneInfo(FRAME_NUMBER_STATUS_POS+1, ID_AUX_INDICATOR, SBPS_STRETCH, 100);

	// --------------  Add application icon -------------- 
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);		// Set big icon
	SetIcon(hIcon, FALSE);		// Set small icon

	// --------------  Add display sub-menu to the main menu -------------- 
	m_displayMenu.LoadMenu(IDR_MENU_DISPLAY);
	m_settingsMenu.LoadMenu(IDR_MENU_SETTINGS);
	m_bodyMenu.LoadMenu(IDR_MENU_BODY);
	BOOL results = GetMenu()->InsertMenu(GetMenu()->GetMenuItemCount()-1, MF_BYPOSITION | MF_POPUP, (UINT) m_displayMenu.GetSafeHmenu(), "&Display");
	     results = GetMenu()->InsertMenu(GetMenu()->GetMenuItemCount()-1, MF_BYPOSITION | MF_POPUP, (UINT) m_settingsMenu.GetSafeHmenu(), "&Settings");
	return 0;
}
// --------------------------------------------------------------------------
LRESULT CMainFrame::OnUpdateStatusBar(UINT wParam, LONG lParam) {
	CNewVisionDoc* doc = ((CNewVisionDoc*)this->GetActiveDocument());
	if (!doc->m_initialized)
		return FALSE;
	char s[15];
	PROCESS_MEMORY_COUNTERS pmc;
	if ( GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof(pmc)) )
	{
		StringCchPrintf(s, 15, "MEM: %6.2f Mb", (float)((1.0*pmc.WorkingSetSize)/(1048576)));
		m_wndStatusBar.SetPaneText(MEM_INFO_STATUS_POS, s);
	}

	StringCchPrintf(s, 15, "Frame: %d", doc->trackermodel.m_frameNumber);
	m_wndStatusBar.SetPaneText(FRAME_NUMBER_STATUS_POS, s);

	return TRUE;
}
// --------------------------------------------------------------------------
LRESULT CMainFrame::OnUpdateNavigationBar(UINT wParam, LONG lParam) {
	CNewVisionDoc* doc = ((CNewVisionDoc*)this->GetActiveDocument());
	if (!doc->m_initialized)
		return FALSE;
    // Read the current stream position
	REFERENCE_TIME rtNow = doc->trackermodel.m_frameNumber;
    // Convert position into a percentage value and update slider position
	CScrollBar* slider = (CScrollBar*)m_wndTrackToolBar.GetDlgItem(ID_SCROLLER);
    if (doc->trackermodel.m_frameTotal != 0)
    {
		int minTick, maxTick;
		slider->GetScrollRange(&minTick, &maxTick);
		long lTick = (long)((1.0* rtNow * maxTick) / doc->trackermodel.m_frameTotal);
        slider->SetScrollPos(lTick);
    }
    else
        slider->SetScrollPos(0);
    
    // Update the 'current position' string on the main dialog
	int nSeconds = (int)(1.0* rtNow / doc->trackermodel.m_frameRate);
    int nMinutes = nSeconds / 60;
    nSeconds %= 60;
	int allSeconds = (int)(1.0* doc->trackermodel.m_frameTotal / doc->trackermodel.m_frameRate);
    int allMinutes = allSeconds / 60;
    allSeconds %= 60;
	CStatic* pos = (CStatic*)m_wndTrackToolBar.GetDlgItem(ID_POSITION);
    // Update the display
    TCHAR szPosition[24], szCurrentString[24];
    StringCchPrintf(szPosition, 24, _T(" %02d:%02d / %02d:%02d \0"), nMinutes, nSeconds, allMinutes, allSeconds);
    // Read current string and compare to the new string.  To prevent flicker,
    // don't update this label unless the string has changed.

	pos->GetWindowText(szCurrentString, 24);
	if (strcmp(szCurrentString, szPosition))
		pos->SetWindowText(szPosition);
	return TRUE;
}
// --------------------------------------------------------------------------
LRESULT CMainFrame::OnUpdateActiveView(UINT wParam, LONG lParam) {
	GetActivePane()->Invalidate();
	GetActivePane()->UpdateWindow();
	return TRUE;
}
// --------------------------------------------------------------------------
LRESULT CMainFrame::OnGraphNotify ( WPARAM wParam, LPARAM lParam )
{
    return TRUE;
}
// --------------------------------------------------------------------------
LRESULT CMainFrame::OnLoadLastFrame ( WPARAM wParam, LPARAM lParam )
{
	CNewVisionDoc* doc = ((CNewVisionDoc*)this->GetActiveDocument());
	if (!doc->m_initialized)
		return FALSE;
	doc->LoadLastTrackedFrame();
	return TRUE;
}
// --------------------------------------------------------------------------
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}
// --------------------------------------------------------------------------

// CMainFrame diagnostics
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

// --------------------------------------------------------------------------
void CMainFrame::OnDestroy()
{
	imgFramework.Destroy();
	CFrameWnd::OnDestroy();
}
// --------------------------------------------------------------------------
void CMainFrame::OnHelpAbout()
{
   AboutDialog aboutDlg;
   INT_PTR nRet = -1;
   nRet = aboutDlg.DoModal();
}
// --------------------------------------------------------------------------
//void CMainFrame::Update() {
//	GetActivePane()->UpdateWindow();
//}
// --------------------------------------------------------------------------
void CMainFrame::DockControlBarLeftOf(CControlBar* Bar, CControlBar* LeftOf)
{
	CRect rect;
	DWORD dw;
	UINT n;
	
	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	RecalcLayout(TRUE);
	
	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;
	
	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a seperate line. By calculating a rectangle, we
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(Bar,n,&rect);
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
CTrackToolBar::CTrackToolBar() {
	bStartOfScroll = TRUE;
	CDialogBar();
}
// --------------------------------------------------------------------------
void CTrackToolBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	//theApp.m_pMainWnd->PostMessage(WM_HSCROLL, MAKEWPARAM(nSBCode, nPos), (LPARAM)pScrollBar->m_hWnd); 
	CNewVisionDoc* doc = ((CNewVisionDoc*)((CMainFrame*)theApp.m_pMainWnd)->GetActiveDocument());
	if (!doc->trackermodel.IsInitialized())
        return;

	// Restore the state at the end and exit
    if (nSBCode == SB_ENDSCROLL)
    {
        bStartOfScroll = TRUE;
		return;
    }

    // Pause when the scroll action begins.
    if (bStartOfScroll)
    {
        bStartOfScroll = FALSE;
    }

	// Get the minimum and maximum scroll-bar positions.
	int curpos = pScrollBar->GetScrollPos();
	int minpos;
	int maxpos;
	pScrollBar->GetScrollRange(&minpos, &maxpos);
	SCROLLINFO   info;
	pScrollBar->GetScrollInfo(&info, SIF_ALL);
	REFERENCE_TIME rtNew = doc->trackermodel.GetCurrentPosition();
	int line_step = (int) (1.0        * doc->trackermodel.m_frameTotal / (maxpos - minpos) );
	int page_step = (int) (info.nPage * doc->trackermodel.m_frameTotal / (maxpos - minpos) );
	switch (nSBCode)
	{
		case SB_TOP:      
			rtNew = 0;
			break;
		case SB_BOTTOM:     
			rtNew = doc->trackermodel.m_frameTotal;
			break;
		case SB_LINEUP:      
			rtNew = max(0, rtNew - line_step);
			break;
		case SB_LINEDOWN:   
			rtNew = min(doc->trackermodel.m_frameTotal, rtNew + line_step);
			break;
		case SB_PAGEUP:    
			rtNew = max(0, rtNew - page_step);
			break;
		case SB_PAGEDOWN:
			rtNew = min(doc->trackermodel.m_frameTotal, rtNew + page_step);
			break;
		case SB_THUMBPOSITION: 
			rtNew = (int) (1.0 * doc->trackermodel.m_frameTotal * nPos / (maxpos - minpos) );
			break;
		case SB_THUMBTRACK:
			rtNew = (int) (1.0 * doc->trackermodel.m_frameTotal * nPos / (maxpos - minpos) );
	}    
	doc->trackermodel.SetPosition(rtNew);
	theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
	theApp.m_pMainWnd->PostMessage(WM_UPDATENAVBAR);
	theApp.m_pMainWnd->PostMessage(WM_UPDATESTATUSBAR);
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
void CBlendToolBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	if (nSBCode == SB_ENDSCROLL) {
		theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
		return;
	}
	CNewVisionDoc* doc = ((CNewVisionDoc*)((CMainFrame*)theApp.m_pMainWnd)->GetActiveDocument());
	if (doc->trackermodel.IsInitialized()) {
		doc->trackermodel.SetTransparency(((CSliderCtrl*)(m_wndBlendToolBar.GetDlgItem(ID_SLIDER)))->GetPos());
	}
	CDialogBar::OnHScroll(nSBCode, nPos, pScrollBar);
}
// --------------------------------------------------------------------------
void CZoomToolBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	if (nSBCode == SB_ENDSCROLL) {
		theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
		return;
	}
	// get range here
	double minFactor = ((CMainFrame*)(theApp.m_pMainWnd))->GetActivePane()->m_minZoomFactor;
	double maxFactor = ((CMainFrame*)(theApp.m_pMainWnd))->GetActivePane()->m_maxZoomFactor;
	int minpos, maxpos;
	nPos = ((CSliderCtrl*)(m_wndZoomToolBar.GetDlgItem(IDC_ZOOMSLIDER)))->GetPos();
	((CSliderCtrl*)(m_wndZoomToolBar.GetDlgItem(IDC_ZOOMSLIDER)))->GetRange(minpos, maxpos);
	double scale;
	const int CENTER = 100;
	if (nPos >= CENTER)
		scale = 1+(maxFactor-1)*2*(nPos-CENTER)/maxpos;
	else
		scale = 1+(minFactor-1)*2*(CENTER-nPos)/maxpos;
	((CMainFrame*)(theApp.m_pMainWnd))->GetActivePane()->SetZoomScale(scale);
	CStatic* pos = (CStatic*)GetDlgItem(ID_ZOOM);
	TCHAR szZoom[6];
	StringCchPrintf(szZoom, 24, _T("%.2fX\0"), scale);
	pos->SetWindowText(szZoom);
	//((CMainFrame*)(theApp.m_pMainWnd))->GetActivePane()->OnHScroll(SB_ENDSCROLL, 0, NULL);
	((CMainFrame*)(theApp.m_pMainWnd))->GetActivePane()->SetDisplaySize();
	((CMainFrame*)(theApp.m_pMainWnd))->GetActivePane()->UpdateVideoPosition();
	theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
	CDialogBar::OnHScroll(nSBCode, nPos, pScrollBar);
}
// --------------------------------------------------------------------------
