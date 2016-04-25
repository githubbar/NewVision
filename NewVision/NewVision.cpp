// NewVision.cpp : Defines the class behaviors for the application.
#include "stdafx.h"
#include "NewVision.h"
#include "MainFrm.h"
#include "NewVisionDoc.h"
#include "NewVisionView.h"
#include "AppSettings.h"
#include "Expire.h"

using namespace System;
using namespace System::Reflection;

[assembly: AssemblyVersionAttribute("1.0.1.0")];

// CNewVisionApp
BEGIN_MESSAGE_MAP(CNewVisionApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_SETTINGS_NEWVISIONSETTINGS, OnAppSettings)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

// CNewVisionApp construction

CNewVisionApp::CNewVisionApp()
{
//	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
	EnableHtmlHelp();
}


// The one and only CNewVisionApp object

CNewVisionApp theApp;

// CNewVisionApp initialization

BOOL CNewVisionApp::InitInstance()
{
	// Check software expiration (set to 60 days)
	//CExpire expire("NewVision Expiration Date", "{3EFBE126-D585-4ab1-853F-634B8383DCF0}", 60, TYPEDAYS);
	//if(expire.HasExpired()) {
	//	AfxMessageBox("Your trial version has expired!");
	//	return FALSE;
	//}

	CoInitialize(NULL);
	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	SetRegistryKey(_T("Alex Leykin Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CNewVisionDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CNewVisionView));
	if (!pDocTemplate)
		return FALSE;
	
	AddDocTemplate(pDocTemplate);
	
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// create an empty new document first to initialize main frame
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew; 
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// then actually process command line arguments
	ParseCommandLine(cmdInfo);
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	ResetMenus();
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->PostMessage(WM_UPDATENAVBAR);
	m_pMainWnd->PostMessage(WM_UPDATESTATUSBAR);
	m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
	
	// Read configuration file
	appSettingsForm = gcnew NewVision::AppSettings();

	// Set Keyboard Shortcuts
	tagShortcutsMap.ReadProfile();
	return TRUE;
}
//---------------------------------------------------------------------------------
int CNewVisionApp::ExitInstance() {
	CoUninitialize();
	return 0;
}
//---------------------------------------------------------------------------------
void CNewVisionApp::OnAppSettings()
{
	if (appSettingsForm->ShowDialog() == ::DialogResult::OK) {
	}
}

// =================================================================================
class CAboutDlg : public CDialog
{
	public:
		CAboutDlg() : CDialog(CAboutDlg::IDD){};

	// Dialog Data
		enum { IDD = IDD_ABOUTBOX };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX) {CDialog::DoDataExchange(pDX);};

	// Implementation
	protected:
		DECLARE_MESSAGE_MAP()
};
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()
//-------------------------------------------------------------
// App command to run the dialog
void CNewVisionApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// ---------------- Global Menu Initialization Routines
bool displayTrackInfo;
bool displayVideoInputImage;
bool displayVideoBodies;
bool displayVideoOrientations;
bool displayVideoBlobs;
bool displayVideoBodyHeights;
bool displayVideoBlobHeights;
bool displayVideoTimeStamp;
bool displayVideoDoors;
bool displayVideoObstacles;
bool displaySceneBG;
bool displaySceneZ;
bool displaySceneBodies;
bool displaySceneBlobs;
bool displaySceneGroups;
bool displayScenePaths;
bool displaySceneDoors;
bool displaySceneCamera;
bool displaySceneHistograms;
bool displaySceneDistancePlane;
bool displayActivityBodies;
bool displayActivityGroups;
bool displayActivitySalesReps;
bool displayActivityDwellingBodies;
bool displayFloorFloorMap;
bool displayFloorBodyMaps;
bool displayFloorBodyPaths;
bool displayFloorBodyTracks;
bool displayFloorHeatMap;
bool displayFloorObstacles;
bool displayFloorZones;
bool displayFloorTimeStamp;
//-------------------------------------------------------------
void ResetMenus() {
	displayTrackInfo = false;
	displayVideoInputImage = false;
	displayVideoBodies = true;
	displayVideoOrientations = false;
	displayVideoBlobs = false;
	displayVideoBodyHeights = false;
	displayVideoBlobHeights = false;
	displayVideoTimeStamp = false;
	displayVideoDoors = false;
	displayVideoObstacles = false;

	displaySceneBG = false;
	displaySceneZ = false;
	displaySceneBodies = false;
	displaySceneBlobs = false;
	displaySceneGroups = false;
	displayScenePaths = false;
	displaySceneDoors = false;
	displaySceneCamera = false;
	displaySceneHistograms = false;
	displaySceneDistancePlane = false;

	displayActivityBodies = true;
	displayActivityGroups = true;
	displayActivitySalesReps = false;
	displayActivityDwellingBodies = false;

	displayFloorFloorMap = false;
	displayFloorBodyMaps = false;
	displayFloorBodyPaths = false;
	displayFloorBodyTracks = false;
	displayFloorHeatMap = false;
	displayFloorObstacles = false;
	displayFloorZones = false;
	displayFloorTimeStamp = false;

	// Initialize zoom slider
	if (IsWindow(m_wndZoomToolBar.m_hWnd)) {
		CSliderCtrl* s = (CSliderCtrl*)(m_wndZoomToolBar.GetDlgItem(IDC_ZOOMSLIDER));
		s->SetRange(0, 200);
		s->SetPos(100);
		s->SetTic(100);
		CStatic* pos = (CStatic*)m_wndZoomToolBar.GetDlgItem(ID_ZOOM);
		pos->SetWindowText("1.0x");
	}

	if (theApp.m_pMainWnd) {
		m_wndControlToolBar.CheckDlgButton(IDC_TRACKING_MODE, false);
		m_wndControlToolBar.CheckDlgButton(IDC_HEATMAP_MODE, false);
		m_wndControlToolBar.CheckDlgButton(IDC_ACTIVITY_MODE, false);
		m_wndControlToolBar.CheckDlgButton(IDC_PLAYBACK_MODE, true);

		m_wndToolBar.GetToolBarCtrl().CheckButton(ID_RECORD, false);
		m_wndToolBar.GetToolBarCtrl().CheckButton(ID_PLAY_RECORDED_TRACKING_DATA, false);
		m_wndToolBar.GetToolBarCtrl().CheckButton(ID_SETTINGS_PARAMETERS, false);

		m_displayMenu.CheckMenuItem(ID_VIDEOPANEL_INPUTIMAGE, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_VIDEOPANEL_BODIES, MF_CHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_VIDEOPANEL_BLOBS, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_VIDEOPANEL_BODYHEIGHTS, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_VIDEOPANEL_BLOBHEIGHTS, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_VIDEOPANEL_TIMESTAMP, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_VIDEOPANEL_DOORS, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_SCENEPANEL_BACKGROUND, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_SCENEPANEL_BODIES, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_SCENEPANEL_BLOBS, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_SCENEPANEL_GROUPS, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_SCENEPANEL_PATHS, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_SCENEPANEL_DOORS, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_SCENEPANEL_CAMERA, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_SCENEPANEL_HISTOGRAMS, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_ACTIVITYPANEL_BODIES, MF_CHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_ACTIVITYPANEL_GROUPS, MF_CHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_ACTIVITYPANEL_SALESREPS, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_ACTIVITYPANEL_DWELLINGBODIES, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_FLOORPANEL_FLOORMAP, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_FLOORPANEL_BODYMAPS, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_FLOORPANEL_BODYPATHS, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_FLOORPANEL_HEATMAP, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_FLOORPANEL_OBSTACLES, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_FLOORPANEL_ZONES, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_FLOORPANEL_TIMESTAMP, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_DATA_SAVEHEATMAP, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_DATA_SAVEFLOORVIEW, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_DATA_SAVEINPUTVIEW, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_DATA_SAVEOUTPUTVIEW, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_LEFTCLICK_MARK_SALES_PERSON, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_LEFTCLICK_MOVEBODY, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_DATA_SAVETRACKINGDATA, MF_UNCHECKED | MF_BYCOMMAND);
		m_displayMenu.CheckMenuItem(ID_DATA_LOADTRACKINGDATA, MF_UNCHECKED | MF_BYCOMMAND);
		m_settingsMenu.CheckMenuItem(ID_SETTINGS_PARAMETERS, MF_UNCHECKED | MF_BYCOMMAND);
	}
}
//-------------------------------------------------------------
BOOL CNewVisionApp::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN)
	{
		tagShortcutsMap.ExecuteTagShortcut(GetDocument(), pMsg);
		//if(pMsg->wParam==VK_RETURN)
		//	pMsg->wParam=VK_TAB;
	}	
	if(pMsg->message==WM_KEYUP)
	{
		tagShortcutsMap.ResetModifier(pMsg);
	}
	return CWinApp::PreTranslateMessage(pMsg);
}

//-------------------------------------------------------------
CNewVisionDoc* CNewVisionApp::GetDocument() {
	POSITION posT = GetFirstDocTemplatePosition();
	if (posT) {
		CDocTemplate* pTemplate = (CDocTemplate*)GetNextDocTemplate(posT);
		POSITION pos = pTemplate->GetFirstDocPosition();
		if (pos)
			return (CNewVisionDoc*) pTemplate->GetNextDoc(pos);
	}
	return NULL;
}