#include "StdAfx.h"
#include "NewVisionDoc.h"
#include "MainFrm.h"
#include "NewVision.h"
#include "NewVisionView.h"
#include "TrueColorToolbar.h"
#include "Globals.h"
#include "Model.h"
#include "BodyActivityModel.h"
#include "SwarmActivityModel.h"
#include "RecordDialog.h"
#include "ProgressDlg.h"
#include "TextEntryDlg.h"
#include "BodyPath.h"
#include "Body.h"
#include "PedestrianTags.h"
#include "Tracks.h"
#include "GroupTagWindow.h"

#include <afxtempl.h>
#using <mscorlib.dll>
#using <System.dll>

using namespace System::Collections;
using namespace System::Collections::Specialized;
using namespace System::Runtime::InteropServices;
using namespace System;
using namespace System::IO;

IMPLEMENT_SERIAL(CNewVisionDoc, CDocument, 1)

HINSTANCE g_hInstance = 0;


// linker inputs: 
// make always on top
// --------------------------------------------------------------------------
//	theApp.m_pMainWnd->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);


BEGIN_MESSAGE_MAP(CNewVisionDoc, CDocument)
	ON_COMMAND(ID_PLAY, OnPlay)
	ON_COMMAND(ID_PAUSE, OnPause)
	ON_COMMAND(ID_REW1, OnREW1)
	ON_COMMAND(ID_FFW1, OnFFW1)
	ON_COMMAND(ID_REW5, OnREW5)
	ON_COMMAND(ID_FFW5, OnFFW5)
	ON_COMMAND(ID_REW10, OnREW10)
	ON_COMMAND(ID_FFW10, OnFFW10)
	ON_COMMAND(ID_GOTOFRAME, OnGotoFrame)
	ON_COMMAND(ID_SETPLAYBACKRATE, OnSetPlaybackRate)
	ON_COMMAND(ID_GOTOTIME, OnGotoTime)
	ON_COMMAND(ID_STOP, OnStop)
	ON_COMMAND(ID_UPDATE_BG, OnUpdateBG)
	ON_COMMAND(ID_SETTINGS_PARAMETERS, OnSettingsParameters)
	ON_COMMAND(ID_RECORD, OnRecord)
	ON_COMMAND(ID_VIDEOPANEL_INPUTIMAGE, OnDisplayLeftInputImage)
	ON_COMMAND(ID_VIDEOPANEL_BODIES, OnDisplayLeftBodies)
	ON_COMMAND(ID_VIDEOPANEL_ORIENTATIONS, OnDisplayLeftOrientations)
	ON_COMMAND(ID_VIDEOPANEL_BLOBS, OnDisplayLeftBlobs)
	ON_COMMAND(ID_VIDEOPANEL_BODYHEIGHTS, OnDisplayLeftBodyHeights)
	ON_COMMAND(ID_VIDEOPANEL_BLOBHEIGHTS, OnDisplayLeftBlobHeights)
	ON_COMMAND(ID_VIDEOPANEL_TIMESTAMP, OnDisplayLeftTimeStamp)
	ON_COMMAND(ID_VIDEOPANEL_DOORS, OnDisplayLeftDoors)
	ON_COMMAND(ID_VIDEOPANEL_OBSTACLES, OnDisplayLeftObstacles)
	ON_COMMAND(ID_SCENEPANEL_BACKGROUND, OnDisplayRightBG)
	ON_COMMAND(ID_SCENEPANEL_Z, OnDisplayRightZ)
	ON_COMMAND(ID_SCENEPANEL_BODIES, OnDisplayRightBodies)
	ON_COMMAND(ID_SCENEPANEL_BLOBS, OnDisplayRightBlobs)
	ON_COMMAND(ID_SCENEPANEL_GROUPS, OnDisplayRightGroups)
	ON_COMMAND(ID_SCENEPANEL_PATHS, OnDisplayRightPaths)
	ON_COMMAND(ID_SCENEPANEL_DOORS, OnDisplayRightDoors)
	ON_COMMAND(ID_SCENEPANEL_CAMERA, OnDisplayRightCamera)
	ON_COMMAND(ID_SCENEPANEL_HISTOGRAMS, OnDisplayRightHistograms)
	ON_COMMAND(ID_SCENEPANEL_DISTANCEPLANE, OnDisplayRightDistancePlane)
	ON_COMMAND(ID_ACTIVITYPANEL_BODIES, OnDisplayActivityBodies)
	ON_COMMAND(ID_ACTIVITYPANEL_GROUPS, OnDisplayActivityGroups)
	ON_COMMAND(ID_ACTIVITYPANEL_SALESREPS, OnDisplayActivitySalesReps)
	ON_COMMAND(ID_ACTIVITYPANEL_DWELLINGBODIES, OnDisplayActivityDwellingBodies)
	ON_COMMAND(ID_FLOORPANEL_FLOORMAP, OnDisplayFloorFloorMap)
	ON_COMMAND(ID_FLOORPANEL_BODYMAPS, OnDisplayFloorBodyMaps)
	ON_COMMAND(ID_FLOORPANEL_BODYPATHS, OnDisplayFloorBodyPaths)
	ON_COMMAND(ID_FLOORPANEL_BODYTRACKS, OnDisplayFloorBodyTracks)
	ON_COMMAND(ID_FLOORPANEL_HEATMAP, OnDisplayFloorHeatMap)
	ON_COMMAND(ID_FLOORPANEL_OBSTACLES, OnDisplayFloorObstacles)
	ON_COMMAND(ID_FLOORPANEL_ZONES, OnDisplayFloorZones)
	ON_COMMAND(ID_FLOORPANEL_TIMESTAMP, OnDisplayFloorTimeStamp)
	ON_COMMAND(ID_DATA_SAVEFLOORVIEW, OnDataSaveFloorView)
	ON_COMMAND(ID_DATA_SAVEINPUTVIEW, OnDataSaveVideoView)
	ON_COMMAND(ID_DATA_SAVEOUTPUTVIEW, OnDataSaveSceneView)
	ON_COMMAND(ID_DATA_SAVETRACKINGDATA, SaveTrackingData)
	ON_COMMAND(ID_DATA_LOADTRACKINGDATA, LoadTrackingData)
	ON_COMMAND(ID_DATA_LOADACTIVITYDATA, LoadActivityData)
	ON_COMMAND(ID_DATA_SAVEACTIVITYDATA, SaveActivityData)
	ON_COMMAND(ID_DATA_LOADHEATMAPDATA, LoadHeatmapData)
	ON_COMMAND(ID_DATA_SAVEHEATMAPDATA, SaveHeatmapData)
	ON_COMMAND(ID_ACTIVITY_DETECTEVENTS, OnActivityDetectEvents)
	ON_COMMAND(ID_ACTIVITY_DETECTGROUPS, OnActivityDetectActivities)
	ON_COMMAND(ID_ACTIVITY_FILTERGROUPS, OnActivityFilterActivities)
	ON_COMMAND(ID_ACTIVITY_FILTERGROUPS2, OnActivityFilterActivities2)
	
	ON_BN_CLICKED(IDC_TRACKING_MODE, OnBnClickedTrackingMode)
	ON_BN_CLICKED(IDC_HEATMAP_MODE, OnBnClickedHeatmapMode)
	ON_BN_CLICKED(IDC_ACTIVITY_MODE, OnBnClickedActivityMode)
	ON_BN_CLICKED(IDC_PLAYBACK_MODE, OnBnClickedPlaybackMode)

	ON_COMMAND(ID_VIEW_TRACKS, OnViewTracks)
	ON_COMMAND(ID_BODY_EDITINFOID, OnBodyEditInfoID)
	ON_COMMAND(ID_BODY_MERGEID, OnBodyMergeID)
	ON_COMMAND(ID_BODY_APPENDID, OnBodyAppendID)
	ON_COMMAND(ID_BODY_CHANGEID, OnBodyChangeID)
	ON_COMMAND(ID_BODY_DELETE, OnBodyDeleteID)
	ON_COMMAND(ID_BODY_TERMINATE, OnBodyTerminateID)
	ON_COMMAND(ID_BODY_SETASDEFAULT, OnBodySetAsDefault)
	
	ON_COMMAND(ID_VISUALIZEDATA_TRACKS, &CNewVisionDoc::OnVisualizeDataTracks)
	ON_COMMAND(ID_VISUALIZEDATA_TIMETAGS, &CNewVisionDoc::OnVisualizeDataTimeTags)

	ON_COMMAND(ID_VISUALIZEDATA_HEATMAP, &CNewVisionDoc::OnVisualizeDataHeatmap)
	ON_COMMAND(ID_VISUALIZEDATA_DWELLHEATMAP, &CNewVisionDoc::OnVisualizeDataDwellHeatmap)
	ON_COMMAND(ID_VISUALIZEDATA_MOTION, &CNewVisionDoc::OnVisualizeDataMotion)
	ON_COMMAND(ID_BODY_ADDTOGROUP, &CNewVisionDoc::OnBodyAddToGroup)
END_MESSAGE_MAP()
// --------------------------------------------------------------------------

CNewVisionDoc::CNewVisionDoc() {
	m_initialized = false;
	settings.SetTitle(_T("Project Preferences"));
	//settings.SetEmptyPageText(_T("Please select a child item of '%s'."));
	//settings.SetTreeViewMode(TRUE, FALSE, FALSE);

	AddModel(dynamic_cast<Model*>(&trackermodel));
	AddModel(dynamic_cast<Model*>(&bgmodel));
	AddModel(dynamic_cast<Model*>(&blobmodel));
	AddModel(dynamic_cast<Model*>(&cameramodel));
	AddModel(dynamic_cast<Model*>(&bodymodel));
	AddModel(dynamic_cast<Model*>(&skinmodel));
	AddModel(dynamic_cast<Model*>(&doormodel));
	AddModel(dynamic_cast<Model*>(&floormodel));
	AddModel(dynamic_cast<Model*>(&obstaclemodel));
	AddModel(dynamic_cast<Model*>(&bodyactivitymodel));
	AddModel(dynamic_cast<Model*>(&swarmactivitymodel));
	AddModel(dynamic_cast<Model*>(&zonemodel));
	AddModel(dynamic_cast<Model*>(&messagemodel));
	// Create settings panel window
	//settings.Create(theApp.m_pMainWnd, WS_POPUP | WS_CAPTION | DS_MODALFRAME | DS_CONTEXTHELP);
	//settings.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	settings.Create(theApp.m_pMainWnd, WS_POPUP | WS_CAPTION | DS_CONTEXTHELP);
	// NOTE: Below is a silly hack to insure the window for "messagemodel" is created
	//       this is needed to create a window for CListCtrl component
	settings.SetActivePage(&messagemodel);
    settings.SetActivePage(&trackermodel);
}
// --------------------------------------------------------------------------
void CNewVisionDoc::AddModel(CObject* m) {
	settings.AddPage(dynamic_cast<CPropertyPage*>(m));
	(dynamic_cast<Model*>(m))->doc = this;
}
// --------------------------------------------------------------------------
CNewVisionView* CNewVisionDoc::GetView(int type) {
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CNewVisionView* pView = (CNewVisionView*) GetNextView(pos);
		if (pView->m_type == type)
			return pView;
	}   
	return NULL;
}
// --------------------------------------------------------------------------
CNewVisionDoc::~CNewVisionDoc()
{
}
// --------------------------------------------------------------------------
void CNewVisionDoc::DeleteContents() {
	m_initialized = false;

	trackermodel.DeInitialize();
	bgmodel.DeInitialize();
	blobmodel.DeInitialize();
	cameramodel.DeInitialize();
	bodymodel.DeInitialize();
	skinmodel.DeInitialize();
	doormodel.DeInitialize();
	floormodel.DeInitialize();
	obstaclemodel.DeInitialize();
	bodyactivitymodel.DeInitialize();
	swarmactivitymodel.DeInitialize();
	zonemodel.DeInitialize();
	messagemodel.DeInitialize();

	// De-Initialize persistent data structures
	m_TrackingData.DeInitialize();
	m_HeatmapData.DeInitialize();
	m_ActivityData.DeInitialize();

	m_ProcessingMode = PLAYBACK;
	m_cursor = CURSOR_DEFAULT;
	m_DefaultTrackID = -1;
	ResetMenus();
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnCloseDocument() {
	m_initialized = false;
	CDocument::OnCloseDocument();
}
// --------------------------------------------------------------------------
bool CNewVisionDoc::Initialize() {
	if (m_initialized)
		return true;

	EnableNavigation(true);
	ShowProgressDialog("Initializing Project", 11);
	// the order of model initialization is important here, because some models rely on others to be initialized
	if (!trackermodel.Initialize()) {
		AfxMessageBox("Please select the input source!", MB_ICONINFORMATION | MB_OK);
		return false;
	}
	StepProgressDialog();
	if (!bgmodel.Initialize()) {
		AfxMessageBox("Please initialize background model!", MB_ICONINFORMATION | MB_OK);
		return false;
	}
	StepProgressDialog();
	if (!blobmodel.Initialize()) {
		AfxMessageBox("Please initialize blob model!", MB_ICONINFORMATION | MB_OK);
		return false;
	}
	StepProgressDialog();
	if (!cameramodel.Initialize()) {
		AfxMessageBox("Please initialize camera model!", MB_ICONINFORMATION | MB_OK);
		return false;
	}
	StepProgressDialog();
	if (!bodymodel.Initialize()) {
		AfxMessageBox("Please initialize body model!", MB_ICONINFORMATION | MB_OK);
		return false;
	}
	StepProgressDialog();
	if (!skinmodel.Initialize()) {
		AfxMessageBox("Please initialize skin model!", MB_ICONINFORMATION | MB_OK);
		return false;
	}
	StepProgressDialog();
	if (!doormodel.Initialize()) {
		AfxMessageBox("Please initialize door model!", MB_ICONINFORMATION | MB_OK);
		return false;
	}
	StepProgressDialog();
	if (!floormodel.Initialize()) {
		AfxMessageBox("Please initialize floor model!", MB_ICONINFORMATION | MB_OK);
		return false;
	}
	StepProgressDialog();
	if (!obstaclemodel.Initialize()) {
		AfxMessageBox("Please initialize obstacle model!", MB_ICONINFORMATION | MB_OK);
		return false;
	}
	StepProgressDialog();
	if (!bodyactivitymodel.Initialize()) {
		AfxMessageBox("Please initialize individual activity model!", MB_ICONINFORMATION | MB_OK);
		return false;
	}
	StepProgressDialog();
	if (!swarmactivitymodel.Initialize()) {
		AfxMessageBox("Please initialize group activity model!", MB_ICONINFORMATION | MB_OK);
		return false;
	}
	StepProgressDialog();
	if (!zonemodel.Initialize()) {
		AfxMessageBox("Please initialize zone model!", MB_ICONINFORMATION | MB_OK);
		return false;
	}
	StepProgressDialog();
	if (!messagemodel.Initialize()) {
		AfxMessageBox("Please initialize message model!", MB_ICONINFORMATION | MB_OK);
		return false;
	}
	HideProgressDialog();
	// Initialize persistent data structures
	m_TrackingData.Initialize(this);
	m_HeatmapData.Initialize(this);
	m_ActivityData.Initialize(this);
	settings.GetActivePage()->UpdateData(false);
	m_initialized = true;
	OnPause(); 
	UpdateOverlayInfo();
	if (theApp.m_pMainWnd) {
		theApp.m_pMainWnd->PostMessage(WM_UPDATENAVBAR);
		theApp.m_pMainWnd->PostMessage(WM_UPDATESTATUSBAR);
	}
	return true;
}
// --------------------------------------------------------------------------
BOOL CNewVisionDoc::OnNewDocument()
{
	// TEMP
	//m_ActivityData.Initialize(this);
	//if (!swarmactivitymodel.Initialize()) {
	//	AfxMessageBox("Please initialize group activity model!", MB_ICONINFORMATION | MB_OK);
	//	return false;
	//}
	//CString filename = "F:\\My Documents\\My Research\\Results\\Data\\Tracking Data\\AE Manually Corrected\\2006-01-19\\AE - Jan 19, 2006 - frames 1to 52300.tracking";
	//CFile file(filename, CFile::modeRead);
	//CArchive ar(&file, CArchive::load);
	//m_TrackingData.Serialize(ar);
	//ar.Close();
	//file.Close();
	//OnVisualizeDataHeatmap();
	//swarmactivitymodel.OnBnClickedButtonDetectNow();
	//this->SaveActivityData();
	//return FALSE;
	// END TEMP
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}
// --------------------------------------------------------------------------
BOOL CNewVisionDoc::OnOpenDocument(LPCTSTR lpszPathName) {
	// two times in a row doesn't do anything at all: default behavior for ID_FILEOPEN button
	// this method is not called if the filename is the same as the open document
	if (!CDocument::OnOpenDocument(lpszPathName)) {
		HideProgressDialog();
		return FALSE;
	}
	bool res = Initialize();
	return res;
}
// --------------------------------------------------------------------------
BOOL CNewVisionDoc::OnSaveDocument(LPCTSTR lpszPathName) {
	if (!trackermodel.IsInitialized())
		return false;
	return CDocument::OnSaveDocument(lpszPathName);
}
// --------------------------------------------------------------------------
void CNewVisionDoc::Serialize(CArchive& ar)
{
 	CObject::Serialize(ar);
	CString s;
	if (ar.IsLoading()) 
		s = "Loading ";
	else
		s = "Saving ";
	ShowProgressDialog(s+"Project", 11);
	StepProgressDialog(s+"input model...");
	trackermodel.Serialize(ar);  
	if (ar.IsLoading()) 
		if (!trackermodel.Initialize()) {
			trackermodel.DeInitialize();
			return;
		}
	StepProgressDialog(s+"background model...");
	bgmodel.Serialize(ar); 
	StepProgressDialog(s+"blob model...");
	blobmodel.Serialize(ar);
	StepProgressDialog(s+"body model...");
	bodymodel.Serialize(ar);
	StepProgressDialog(s+"skin model...");
	skinmodel.Serialize(ar);
	StepProgressDialog(s+"door model...");
	doormodel.Serialize(ar);
	StepProgressDialog(s+"camera model...");
	cameramodel.Serialize(ar);
	StepProgressDialog(s+"floor model...");
	floormodel.Serialize(ar);
	StepProgressDialog(s+"obstacle model...");
	obstaclemodel.Serialize(ar);
	StepProgressDialog(s+"body activity model...");
	bodyactivitymodel.Serialize(ar);
	StepProgressDialog(s+"group activity model...");
	swarmactivitymodel.Serialize(ar);
	StepProgressDialog(s+"zone model...");
	zonemodel.Serialize(ar);
	HideProgressDialog();
}
// --------------------------------------------------------------------------
void CNewVisionDoc::AutoSave()
{
	//// BEGIN TEMP
	//return;
	//// END TEMP
	System::Configuration::Configuration ^XML = ConfigurationManager::OpenExeConfiguration(ConfigurationUserLevel::None);
	// if autosave is set to "on" and it's time to save
	if (XML->AppSettings->Settings["autosave"]->Value 
		&& trackermodel.m_frameNumber % Int32::Parse(XML->AppSettings->Settings["autosave interval"]->Value) == 0) {
			// save tracking data
			CTime t = CTime::GetCurrentTime();
			// if folder is chosen use it, otherwise save in current
			String ^dirname = "";
			if (XML->AppSettings->Settings["autosave folder"]->Value != "Choose folder...") {
				dirname = XML->AppSettings->Settings["autosave folder"]->Value;
				if (!Directory::Exists(dirname))
					Directory::CreateDirectory(dirname);
			}

			CString tname = CString(dirname) + "\\" + "autosave " + t.Format("%a, %B %d, %Y, %I-%M-%S%p")+String::Format(" frame {0}", trackermodel.m_frameNumber) + ".tracking";
			CFile file(tname, CFile::modeCreate | CFile::modeWrite);
			CArchive ar(&file, CArchive::store);
			m_TrackingData.Serialize(ar);
			ar.Close();
			file.Close();
			// save activity data
			CString aname = CString(dirname) + "\\" + "autosave "+t.Format("%a, %B %d, %Y, %I-%M-%S%p")+String::Format(" frame {0}", trackermodel.m_frameNumber)+".activity";
			CFile afile(aname, CFile::modeCreate | CFile::modeWrite);
			CArchive aar(&afile, CArchive::store);
			m_ActivityData.Serialize(aar);
			aar.Close();
			afile.Close();
	}
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnPause() {
	if (!trackermodel.IsInitialized())
		return;
	trackermodel.Pause();

	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_PLAY, false);
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnPlay() {
	if (!trackermodel.IsInitialized())
		return;

	if (trackermodel.IsRunning()) {
		trackermodel.Pause();
		m_wndToolBar.GetToolBarCtrl().CheckButton(ID_PLAY, false);
	}
	else {
		trackermodel.Run();
		m_wndToolBar.GetToolBarCtrl().CheckButton(ID_PLAY, true);
	}
	
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnREW1()
{
	trackermodel.SetPosition(trackermodel.m_frameNumber-1);
	UpdateOverlayInfo();
	theApp.m_pMainWnd->PostMessage(WM_UPDATENAVBAR);
	theApp.m_pMainWnd->PostMessage(WM_UPDATESTATUSBAR);
	theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnREW10()
{
	trackermodel.SetPosition(trackermodel.m_frameNumber-10);
	UpdateOverlayInfo();
	theApp.m_pMainWnd->PostMessage(WM_UPDATENAVBAR);
	theApp.m_pMainWnd->PostMessage(WM_UPDATESTATUSBAR);
	theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnFFW1()
{
	trackermodel.SetPosition(trackermodel.m_frameNumber+1);
	// BEGIN TEMP
	//UpdateOverlayInfo();
	//theApp.m_pMainWnd->PostMessage(WM_UPDATENAVBAR);
	//theApp.m_pMainWnd->PostMessage(WM_UPDATESTATUSBAR);
	//theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
	// END TEMP
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnFFW10()
{
	trackermodel.SetPosition(trackermodel.m_frameNumber+10);
	UpdateOverlayInfo();
	theApp.m_pMainWnd->PostMessage(WM_UPDATENAVBAR);
	theApp.m_pMainWnd->PostMessage(WM_UPDATESTATUSBAR);
	theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnREW5()
{
	trackermodel.SetPosition(trackermodel.m_frameNumber-5);
	UpdateOverlayInfo();
	theApp.m_pMainWnd->PostMessage(WM_UPDATENAVBAR);
	theApp.m_pMainWnd->PostMessage(WM_UPDATESTATUSBAR);
	theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnFFW5()
{
	trackermodel.SetPosition(trackermodel.m_frameNumber+5);
	UpdateOverlayInfo();
	theApp.m_pMainWnd->PostMessage(WM_UPDATENAVBAR);
	theApp.m_pMainWnd->PostMessage(WM_UPDATESTATUSBAR);
	theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnGotoFrame() {
	//// BEGIN TEMP
	//NameValueCollection^ coll = gcnew NameValueCollection();
	//// Show Dialog
	//NewVision::PedestrianTags ^infoDlg = gcnew NewVision::PedestrianTags(coll);
	//infoDlg->Text = String::Format("Information for ID = {0}", 0);
	//if (infoDlg->ShowDialog() == ::DialogResult::OK) 
	//	int a =1;
	//delete infoDlg;
	//delete coll;
	//return;
	//// END TEMP
	CTextEntryDlg dlgTextEntry;
	if (!trackermodel.IsInitialized()) {
		AfxMessageBox("Initialize tracker model first!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	if (dlgTextEntry.Show(theApp.m_pMainWnd, "Go To Frame (Ctrl-G)", "Go To Frame Number ...?") == IDOK) {
		CString strResult = dlgTextEntry.GetText();
		REFERENCE_TIME t = _atoi64(strResult);
		trackermodel.SetPosition(t);
		UpdateOverlayInfo();
		if (::IsWindow(theApp.m_pMainWnd->m_hWnd)) {
			theApp.m_pMainWnd->PostMessage(WM_UPDATENAVBAR);
			theApp.m_pMainWnd->PostMessage(WM_UPDATESTATUSBAR);
			theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
		}
	}
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnSetPlaybackRate() {
	CTextEntryDlg dlgTextEntry;
	if (!trackermodel.IsInitialized()) {
		AfxMessageBox("Initialize tracker model first!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	if (dlgTextEntry.Show(theApp.m_pMainWnd, "Set playback rate(Ctrl-G)", "Set playback speed to (default 1.0) ...?") == IDOK) {
		CString strResult = dlgTextEntry.GetText();
		double r = atof(strResult);
		trackermodel.SetRate(r);
	}
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnGotoTime() {
	if (!trackermodel.IsInitialized()) {
		AfxMessageBox("Initialize tracker model first!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	REFERENCE_TIME t;
	if (getTime("Go To Time (Ctrl-T)", "Go To Time (hh:mm:ss)?", "", t)) {
		trackermodel.SetPosition(t);
		if (::IsWindow(theApp.m_pMainWnd->m_hWnd)) {
			theApp.m_pMainWnd->PostMessage(WM_UPDATENAVBAR);
			theApp.m_pMainWnd->PostMessage(WM_UPDATESTATUSBAR);
			theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
		}
	}
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnStop() {
	if (!m_initialized)
		return;
	OnPause();
	{ // +++ begin data processing critical section	
		CAutoLock dataLock( &dataCriticalSection); 
		bodymodel.body.RemoveAll();
		m_TrackingData.Clear();
		m_ActivityData.Clear();
	} // +++ end data processing critical section	
	trackermodel.m_frameLastProcessed = trackermodel.m_frameNumber = trackermodel.m_frameStart;
	trackermodel.SetPosition();
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnUpdateBG()
{
	if (!m_initialized)
		return;
	bgmodel.Update();
}
// --------------------------------------------------------------------------
void CNewVisionDoc::UpdateOverlayInfo() {
	if (!m_initialized)
		return;
	{ // +++ begin data processing critical section	
		CAutoLock dataLock( &dataCriticalSection); 
		switch (((CMainFrame*)(theApp.m_pMainWnd))->GetActivePane()->m_type) {
			case VIDEO_VIEW : {
				cvSet(imgOverlayInfo, CV_INVISIBLE_COLOR);
				//cvCircle(imgOverlayInfo, cvPointFrom32f(test1), 4, CV_RGB(255,0,0), CV_FILLED);
				//cvCircle(imgOverlayInfo, cvPointFrom32f(test2), 2, CV_RGB(0,255,0), CV_FILLED);
				//cameramodel.DrawVanishingPoints(imgOverlayInfo);
				displayVideoInputImage ? cvCopy(imgFrame, imgOverlayInfo) : 1;
				displayVideoBlobs  ? blobmodel.Draw(imgOverlayInfo, CV_RGB(0,0,255)) : 1;
				//displayVideoBlobs  ? bgmodel.DrawHeat(imgFrame, imgOverlayInfo) : 1;
				displayVideoBlobHeights ? blobmodel.DrawHeight(imgOverlayInfo, CV_RGB(250,250,0)) : 1; 
				displayVideoTimeStamp ? trackermodel.ShowTimeStamp(imgOverlayInfo) : 1;
				displayVideoDoors  ? doormodel.DrawFrame(imgOverlayInfo, CV_RGB(255,0,0)) : 1;
				/*if (m_ProcessingMode == PLAYBACK) 
					displayVideoBodies ? bodymodel.DrawColorizingConfidence(imgOverlayInfo, CV_RGB(0,255,0), CV_RGB(255,0,0)) : 1; 	*/
				displayVideoOrientations ? bodymodel.DrawMotionOrientation(imgFrame, imgOverlayInfo) : 1;
				displayVideoBodies ? bodymodel.DrawFrame(imgOverlayInfo, CV_RGB(0,255,0)) : 1; 
				//displayVideoBodyHeights ? bodymodel.DrawHeight(imgOverlayInfo, CV_RGB(128,128,0)) : 1; 
				displayVideoBodyHeights ? bodymodel.DrawInfo(imgOverlayInfo, CV_RGB(255,255,255)) : 1; 
				displayActivityGroups ? bodymodel.DrawGroups(imgOverlayInfo, CV_RGB(255,255,255)) : 1;
				displayActivitySalesReps ? bodyactivitymodel.DrawSalesReps(imgOverlayInfo, CV_RGB(0,0,255)) : 1;
				//displayActivityDwellingBodies ? bodyactivitymodel.DrawDwellingBodies(imgOverlayInfo, CV_RGB(0,255,0)) : 1;
				displayActivityDwellingBodies ? bodymodel.DrawBrowsingMarks(imgOverlayInfo, CV_RGB(0,255,0)) : 1;
				// TEMP
				//cameramodel.DrawVanishingPoints(imgOverlayInfo);
				//CvPoint3D32f p = cameramodel.coordsImage2Real(cvPoint2D32f(cameramodel.w/2, cameramodel.h/2), 80);
				//for (int i=0;i<10;i++) {
				//	cameramodel.DrawVerticalSpheroid(imgOverlayInfo, cvPoint3D32f(-100+i*40, -300, 80), cvSize2D32f(30, 80), CV_RGB(i*25, 0,0), false);
				//	cvCircle(imgOverlayInfo, cvPointFrom32f(cameramodel.coordsReal2Image(cvPoint3D32f(-300+i*40, 100, 80))), 2, CV_RGB(0, 0, 255), 2);
				//}
				//for (int i=0;i<10;i++)
				//	cameramodel.DrawVerticalSpheroid(imgOverlayInfo, cvPoint3D32f(70, -200-i*50, 80), cvSize2D32f(30, 80), CV_RGB(0, i*25, 0), false);
				//for (int i=0;i<10;i++)
				//	cameramodel.DrawVerticalSpheroid(imgOverlayInfo, cvPoint3D32f(70, -300, 0+i*40), cvSize2D32f(30, 80), CV_RGB(0, 0, i*25), false);
				//for (int i=0;i<10;i++)
				//	cvCircle(imgOverlayInfo, cvPointFrom32f(cameramodel.coordsReal2Image(cvPoint3D32f(p.x, p.y-250+50*i, 80))), 2, CV_RGB(255, 0, 0), 2);
				//for (int i=0;i<10;i++)
				//	cvCircle(imgOverlayInfo, cvPointFrom32f(cameramodel.coordsReal2Image(cvPoint3D32f(p.x-250+50*i, p.y, 80))), 2, CV_RGB(0, 0, 255), 2);
				//for (int i=0;i<10;i++)
				//	cvCircle(imgOverlayInfo, cvPointFrom32f(cameramodel.coordsReal2Image(cvPoint3D32f(p.x, p.y, 80+50*i))), 2, CV_RGB(0, 255, 0), 2);
				// END TEMP
				trackermodel.IplImage2Overlay(imgOverlayInfo, trackermodel.m_bitmap);
				break;
			}
			case SCENE_VIEW :
				cvSet(imgOverlayInfo, CV_RGB(255,255,255));
				displaySceneBG     ? bgmodel.Draw(imgOverlayInfo) : 1;
				displaySceneZ      ? bodymodel.DrawZBuffer(imgOverlayInfo) : 1;
				displaySceneBlobs  ? blobmodel.Draw(imgOverlayInfo, CV_RGB(0,230,0)) : 1;
				displaySceneBodies ? bodymodel.DrawFrame(imgOverlayInfo, CV_RGB(0,255,0)) : 1; 	
				displaySceneDoors  ? doormodel.DrawFrame(imgOverlayInfo, CV_RGB(255,0,0)) : 1;
				displaySceneCamera ? cameramodel.Draw(imgOverlayInfo, CV_RGB(0,0,0)) : 1;
				displaySceneHistograms ? bodymodel.DrawHistograms(imgOverlayInfo) : 1;
				displaySceneDistancePlane ? cameramodel.DrawDistanceMap(imgOverlayInfo) : 1; 	
				trackermodel.IplImage2Overlay(imgOverlayInfo, trackermodel.m_bitmap);
				break;
			case ACTIVITY_VIEW :
				cvSet(imgOverlayInfo, CV_INVISIBLE_COLOR);
				displayActivityBodies ? bodymodel.DrawFrame(imgOverlayInfo, CV_RGB(0,255,0)) : 1; 	
				displayActivitySalesReps ? bodyactivitymodel.DrawSalesReps(imgOverlayInfo, CV_RGB(0,0,255)) : 1;
				displayActivityDwellingBodies ? bodyactivitymodel.DrawDwellingBodies(imgOverlayInfo, CV_RGB(0,255,0)) : 1;
				displayActivityGroups ? swarmactivitymodel.DrawEventsFrame(imgOverlayInfo, CV_RGB(255,255,255)) : 1;
				trackermodel.IplImage2Overlay(imgOverlayInfo, trackermodel.m_bitmap);
				break;
			case FLOOR_VIEW :
				cvSet(floorImg, CV_RGB(255,255,255));
				displayFloorFloorMap  ? floormodel.Draw(floorImg) : 1;
				displayFloorHeatMap ? m_HeatmapData.DrawHeatMap(floorImg, false) : 1;
				displayFloorObstacles ? obstaclemodel.DrawFloor(floorImg, CV_RGB(255,128,0)) : 1;
				displayFloorZones ? zonemodel.DrawFloor(floorImg, CV_RGB(0,0,128)) : 1;
				displayFloorBodyMaps  ? bodymodel.DrawFloor(floorImg, CV_RGB(0,255,0)) : 1; 
				displayFloorBodyPaths ? m_TrackingData.DrawFloor(floorImg, CV_RGB(0,255,0)) : 1; 
				//displayFloorBodyTracks ? m_TrackingData.DrawTracksFloor(floorImg, CV_RGB(0,255,0)) : 1; 
				displayFloorTimeStamp ? trackermodel.ShowTimeStamp(floorImg) : 1;
				//trackermodel.IplImage2Overlay(floorImg, trackermodel.m_floorBitmap);
				((CMainFrame*)(theApp.m_pMainWnd))->GetActivePane()->Invalidate();
				break;
			default:
				;
		}
	} // +++ end data processing critical section	
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnRecord() {
	if (!trackermodel.IsInitialized())
		return;
	else if (trackermodel.m_record) {
		CAutoLock singleLock(&recordCriticalSection);
		trackermodel.ReleaseVideoWriters();
		trackermodel.m_record = false;
		m_wndToolBar.GetToolBarCtrl().CheckButton(ID_RECORD, false);
	}
	else {
		bool wasRunning = trackermodel.IsRunning();
		if (wasRunning) {
			trackermodel.Pause();
		}
		
		RecordDialog recordDlg;
		if (recordDlg.DoModal() == IDOK) {
				CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, "AVI Files (*.avi)|*.avi|MPEG Files (*.mpg)|*.mpg|All Files (*.*)|*.*||", theApp.m_pMainWnd);
				char title[]= {"Select Output File"};
				dlg.m_ofn.lpstrTitle = title;
				if (dlg.DoModal() == IDOK) {
					// trackermodel->out = new FileSource(dlg.GetPathName());
					CString fileName = dlg.GetPathName();
					//fileName = fileName.Right(4) == ".avi" ? fileName : fileName + ".avi";
					m_recordVideoView = recordDlg.m_recordVideoView;
					m_recordSceneView = recordDlg.m_recordSceneView;
					m_recordActivityView = recordDlg.m_recordActivityView;
					m_recordFloorView = recordDlg.m_recordFloorView;
					CreateVideoWriters(fileName, trackermodel.m_frameRate);
					trackermodel.m_record = true;
					m_wndToolBar.GetToolBarCtrl().CheckButton(ID_RECORD, true);
				}
		}

		if (wasRunning)
			trackermodel.Run();
	}
}
// --------------------------------------------------------------------------
bool CNewVisionDoc::ChangeMenuItemState(int sub, int item) {
	CMenu* mmenu = theApp.m_pMainWnd->GetMenu();
	CMenu* submenu = mmenu->GetSubMenu(sub);
	MENUITEMINFO info;
	info.cbSize = sizeof (MENUITEMINFO); // must fill up this field
	info.fMask = MIIM_STATE;             // get the state of the menu item
	VERIFY(submenu->GetMenuItemInfo(item, &info));
	if (info.fState & MF_CHECKED) {
		submenu->CheckMenuItem(item, MF_UNCHECKED | MF_BYCOMMAND);
		return true;
	}
	else {
		submenu->CheckMenuItem(item, MF_CHECKED | MF_BYCOMMAND);
		return false;
	}
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnSettingsParameters() {
		//CRect r1, r2;
		//theApp.m_pMainWnd->GetWindowRect(&r1);
		//settings.GetWindowRect(&r2);
	if (theApp.m_pMainWnd->GetMenu()->GetMenuState(ID_SETTINGS_PARAMETERS, MF_BYCOMMAND) == MF_UNCHECKED) {
		theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_SETTINGS_PARAMETERS, MF_CHECKED | MF_BYCOMMAND);
		m_wndToolBar.GetToolBarCtrl().CheckButton(ID_SETTINGS_PARAMETERS, true);
		settings.GetActivePage()->UpdateData(false);
		settings.ShowWindow(SW_SHOW);
		//theApp.m_pMainWnd->SetWindowPos(&CWnd::wndNoTopMost, 0,0, r1.Width(), r1.Height()+1/*-r2.Height()*/, SWP_NOMOVE);
	}
	else {
		theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_SETTINGS_PARAMETERS, MF_UNCHECKED | MF_BYCOMMAND);
		m_wndToolBar.GetToolBarCtrl().CheckButton(ID_SETTINGS_PARAMETERS, false);
		settings.GetActivePage()->UpdateData(true);
		settings.ShowWindow(SW_HIDE);
		//theApp.m_pMainWnd->SetWindowPos(&CWnd::wndNoTopMost, 0,0, r1.Width(), r1.Height()-1/*+r2.Height()*/, SWP_NOMOVE);
	}
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayLeftInputImage() {
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_VIDEOPANEL_INPUTIMAGE, (displayVideoInputImage? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayVideoInputImage = !displayVideoInputImage;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayLeftBodies() {
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_VIDEOPANEL_BODIES, (displayVideoBodies ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayVideoBodies = !displayVideoBodies;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayLeftOrientations() {
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_VIDEOPANEL_ORIENTATIONS, (displayVideoOrientations ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayVideoOrientations = !displayVideoOrientations;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayLeftBlobs(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_VIDEOPANEL_BLOBS, (displayVideoBlobs ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayVideoBlobs = !displayVideoBlobs;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayLeftBodyHeights() {
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_VIDEOPANEL_BODYHEIGHTS, (displayVideoBodyHeights ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayVideoBodyHeights = !displayVideoBodyHeights;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayLeftBlobHeights() {
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_VIDEOPANEL_BLOBHEIGHTS, (displayVideoBlobHeights ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayVideoBlobHeights = !displayVideoBlobHeights;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayLeftTimeStamp(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_VIDEOPANEL_TIMESTAMP, (displayVideoTimeStamp? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayVideoTimeStamp = !displayVideoTimeStamp;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayLeftDoors(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_VIDEOPANEL_DOORS, (displayVideoDoors ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayVideoDoors = !displayVideoDoors;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayLeftObstacles(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_VIDEOPANEL_OBSTACLES, (displayVideoObstacles ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayVideoObstacles = !displayVideoObstacles;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayRightBG(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_SCENEPANEL_BACKGROUND, (displaySceneBG ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displaySceneBG = !displaySceneBG;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayRightZ()	{
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_SCENEPANEL_Z, (displaySceneZ ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displaySceneZ = !displaySceneZ;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayRightBodies(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_SCENEPANEL_BODIES, (displaySceneBodies ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displaySceneBodies = !displaySceneBodies;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayRightBlobs(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_SCENEPANEL_BLOBS, (displaySceneBlobs ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displaySceneBlobs = !displaySceneBlobs;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayRightGroups(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_SCENEPANEL_GROUPS, (displaySceneGroups ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displaySceneGroups = !displaySceneGroups;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayRightPaths(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_SCENEPANEL_PATHS, (displayScenePaths ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayScenePaths = !displayScenePaths;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayRightDoors(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_SCENEPANEL_DOORS, (displaySceneDoors ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displaySceneDoors = !displaySceneDoors;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayRightCamera(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_SCENEPANEL_CAMERA, (displaySceneCamera ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displaySceneCamera = !displaySceneCamera;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayRightHistograms(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_SCENEPANEL_HISTOGRAMS, (displaySceneHistograms ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displaySceneHistograms = !displaySceneHistograms;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayRightDistancePlane(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_SCENEPANEL_DISTANCEPLANE, (displaySceneDistancePlane ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displaySceneDistancePlane = !displaySceneDistancePlane;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayActivityBodies(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_ACTIVITYPANEL_BODIES, (displayActivityBodies ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayActivityBodies = !displayActivityBodies;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayActivityGroups(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_ACTIVITYPANEL_GROUPS, (displayActivityGroups ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayActivityGroups = !displayActivityGroups;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayActivitySalesReps(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_ACTIVITYPANEL_SALESREPS, (displayActivitySalesReps? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayActivitySalesReps = !displayActivitySalesReps;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayActivityDwellingBodies(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_ACTIVITYPANEL_DWELLINGBODIES, (displayActivityDwellingBodies? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayActivityDwellingBodies = !displayActivityDwellingBodies;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayFloorFloorMap(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_FLOORPANEL_FLOORMAP, (displayFloorFloorMap ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayFloorFloorMap = !displayFloorFloorMap;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayFloorBodyMaps(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_FLOORPANEL_BODYMAPS, (displayFloorBodyMaps ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayFloorBodyMaps = !displayFloorBodyMaps;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayFloorBodyPaths(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_FLOORPANEL_BODYPATHS, (displayFloorBodyPaths ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayFloorBodyPaths = !displayFloorBodyPaths;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayFloorBodyTracks(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_FLOORPANEL_BODYTRACKS, (displayFloorBodyTracks ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayFloorBodyTracks = !displayFloorBodyTracks;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayFloorHeatMap(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_FLOORPANEL_HEATMAP, (displayFloorHeatMap ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayFloorHeatMap= !displayFloorHeatMap;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayFloorObstacles(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_FLOORPANEL_OBSTACLES, (displayFloorObstacles ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayFloorObstacles = !displayFloorObstacles;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayFloorZones(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_FLOORPANEL_ZONES, (displayFloorZones ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayFloorZones = !displayFloorZones;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDisplayFloorTimeStamp(){ 
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_FLOORPANEL_TIMESTAMP, (displayFloorTimeStamp ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayFloorTimeStamp = !displayFloorTimeStamp;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDataSaveVideoView()
{
	CFileDialog dlg(FALSE, ".bmp", "Video View 01", OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, "BMP Files (*.bmp)|*.bmp||", NULL);
	char title[]= {"Save Video View As..."};
	dlg.m_ofn.lpstrTitle = title;
	if (dlg.DoModal() == IDOK) {
		CString filename = dlg.GetPathName();
		cvSaveImage(filename, imgOverlayInfo);
	}	
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDataSaveSceneView()
{

	// TEMP
	System::IO::StreamWriter^ log = File::CreateText("F:\\My Documents\\My Research\\Results\\Data\\histograms.csv");
	POSITION pos = bodymodel.body.GetStartPosition();
	while (pos) {
		int id; Body *b;
		bodymodel.body.GetNextAssoc(pos, id, b);
		log->WriteLine(b->color.ToString());
	}
	log->Close();	
	// END TEMP

	// TEMP
	log = File::CreateText("F:\\My Documents\\My Research\\Results\\Data\\weights.csv");
	pos = bodymodel.body.GetStartPosition();
	while (pos) {
		int id; Body *b;
		bodymodel.body.GetNextAssoc(pos, id, b);
		cvSaveImage("F:\\My Documents\\My Research\\Results\\Data\\weights.jpg", weight);
		//log->WriteLine(b->color.ToString());
	}
	log->Close();	
	// END TEMP

	CFileDialog dlg(FALSE, ".bmp", "Scene View 01", OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, "BMP Files (*.bmp)|*.bmp||", NULL);
	char title[]= {"Save Scene View As..."};
	dlg.m_ofn.lpstrTitle = title;
	if (dlg.DoModal() == IDOK) {
		CString filename = dlg.GetPathName();
		cvSaveImage(filename, imgOverlayInfo);
	}	
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDataSaveActivityView()
{
	CFileDialog dlg(FALSE, ".bmp", "Activity View 01", OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, "BMP Files (*.bmp)|*.bmp||", NULL);
	char title[]= {"Save Activity View As..."};
	dlg.m_ofn.lpstrTitle = title;
	if (dlg.DoModal() == IDOK) {
		CString filename = dlg.GetPathName();
		cvSaveImage(filename, activityImg);
	}	
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnDataSaveFloorView()
{
	CFileDialog dlg(FALSE, ".bmp", "Floor View 01", OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, "BMP Files (*.bmp)|*.bmp||", NULL);
	char title[]= {"Save Floor View As..."};
	dlg.m_ofn.lpstrTitle = title;
	if (dlg.DoModal() == IDOK) {
		CString filename = dlg.GetPathName();
		cvSaveImage(filename, floorImg);
	}	
}
// ------------------------------------------------------------------------
void CNewVisionDoc::LoadLastTrackedFrame() {
	REFERENCE_TIME t = m_TrackingData.GetLastRecordedFrame();
	if (trackermodel.m_frameNumber < t) {
		OnPause();
		trackermodel.SetPosition(t);
		m_TrackingData.Get((int)t);
		trackermodel.m_frameLastProcessed = t;
		trackermodel.Pause();
		if (::IsWindow(theApp.m_pMainWnd->m_hWnd)) {
			theApp.m_pMainWnd->PostMessage(WM_UPDATENAVBAR);
			theApp.m_pMainWnd->PostMessage(WM_UPDATESTATUSBAR);
		}
		trackermodel.m_frameLastProcessed = t;
	}
}
// ------------------------------------------------------------------------
void CNewVisionDoc::EnableNavigation(bool enable) {
	if (enable) {
		theApp.m_pMainWnd->GetMenu()->EnableMenuItem(ID_GOTOFRAME, MF_ENABLED | MF_BYCOMMAND);
		theApp.m_pMainWnd->GetMenu()->EnableMenuItem(ID_GOTOTIME, MF_ENABLED | MF_BYCOMMAND);
		m_wndTrackToolBar.EnableWindow(true);
		m_wndTrackToolBar.EnableScrollBar(SB_HORZ, ESB_ENABLE_BOTH);
	}
	else {
		theApp.m_pMainWnd->GetMenu()->EnableMenuItem(ID_GOTOFRAME, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
		theApp.m_pMainWnd->GetMenu()->EnableMenuItem(ID_GOTOTIME, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
		m_wndTrackToolBar.EnableWindow(false);
		m_wndTrackToolBar.EnableScrollBar(SB_HORZ, ESB_DISABLE_BOTH);
	}
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnBnClickedTrackingMode() {
	// make sure that tracking from the last frame recorded
	EnableNavigation(false);
	//trackermodel.SetRate(1.0);
	//trackermodel.pMediaFilter->SetSyncSource(NULL);
	LoadLastTrackedFrame();
	m_ProcessingMode = TRACKING;
}
// ------------------------------------------------------------------------
void CNewVisionDoc::OnBnClickedHeatmapMode() {
	EnableNavigation(false);
	//trackermodel.SetRate(1.0);
	//trackermodel.pGraph->SetDefaultSyncSource();
	m_ProcessingMode = HEATMAP;
}
// ------------------------------------------------------------------------
void CNewVisionDoc::OnBnClickedActivityMode() {
	EnableNavigation(true);
	//trackermodel.SetRate(1.0);
	//trackermodel.pGraph->SetDefaultSyncSource();
	m_ProcessingMode = ACTIVITY;
}
// ------------------------------------------------------------------------
void CNewVisionDoc::OnBnClickedPlaybackMode() {
	EnableNavigation(true);
	//trackermodel.SetRate(1.0);
	//trackermodel.pGraph->SetDefaultSyncSource();
	m_ProcessingMode = PLAYBACK;
}
// ------------------------------------------------------------------------
void CNewVisionDoc::OnViewTracks() {
	theApp.m_pMainWnd->GetMenu()->CheckMenuItem(ID_VIEW_TRACKS, (displayTrackInfo? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
	displayTrackInfo = !displayTrackInfo;
	if (displayTrackInfo) {
		// Show Dialog
		infoDlg = gcnew NewVision::Tracks(&m_TrackingData);
		infoDlg->Text = gcnew String("Recorded tracks information");
		infoDlg->Show();
	}	
	else {
		// Hide Dialog
		infoDlg->Hide();
		delete infoDlg;
	}
}
// ------------------------------------------------------------------------
void CNewVisionDoc::OnBodyAddToGroup()
{
	// Show Dialog
	gcroot<NewVision::GroupTagWindow^> tagDlg = gcnew NewVision::GroupTagWindow(&this->m_ActivityData, GetView(VIDEO_VIEW)->m_match);
	tagDlg->Text = gcnew String("Shopper Groups");
	tagDlg->ShowDialog();
	delete tagDlg;
}
// ------------------------------------------------------------------------
void CNewVisionDoc::UpdateAll() {
	// update display
	UpdateOverlayInfo();
	trackermodel.SetPosition();
	if (theApp.m_pMainWnd)
		theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
}
// ------------------------------------------------------------------------
void CNewVisionDoc::OnBodyEditInfoID() {
	CAutoLock dataLock( &dataCriticalSection); 
	m_TrackingData.EditInfoID(GetView(VIDEO_VIEW)->m_match);
}
// ------------------------------------------------------------------------
void CNewVisionDoc::OnBodyMergeID() {
	{ // +++ begin data processing critical section	
		CAutoLock dataLock( &dataCriticalSection); 
		if (m_DefaultTrackID != -1) 
			m_TrackingData.MergeID(GetView(VIDEO_VIEW)->m_match);
		else
			m_TrackingData.MergeID(m_DefaultTrackID);
	} // +++ end data processing critical section	
	UpdateAll();
}
// ------------------------------------------------------------------------
void CNewVisionDoc::OnBodyAppendID() {
	{ // +++ begin data processing critical section	
		CAutoLock dataLock( &dataCriticalSection);
		m_TrackingData.AppendID(GetView(VIDEO_VIEW)->m_match);
	} // +++ end data processing critical section	
	UpdateAll();
}
// ------------------------------------------------------------------------
void CNewVisionDoc::OnBodyChangeID() {
	{ // +++ begin data processing critical section	
		CAutoLock dataLock( &dataCriticalSection); 
		int id1 = GetView(VIDEO_VIEW)->m_match;
		String ^s = String::Format("From {0} to: ", id1);
		CTextEntryDlg dlgTextEntry;
		if (dlgTextEntry.Show(theApp.m_pMainWnd, "Change body ID", CString(s)) != IDOK)
			return;

		CString strResult = dlgTextEntry.GetText();
		int id2 = _atoi64(strResult);

		Body* b;BodyPath* bp;
		int result = IDOK;
		if (bodymodel.body.Lookup(id2, b) || m_TrackingData.data.Lookup(id2, bp)) {
			CString s; s.Format("There is a track with ID = %d, overwrite? ", id2);
			result = AfxMessageBox(s, MB_ICONQUESTION | MB_OKCANCEL);
			if (result == IDCANCEL)
				return;
		}
		if (result == IDOK)
			m_TrackingData.ChangeID(id1, id2);

		// replace id1's in current tracking
		bodymodel.body.ChangeID(id1, id2);
		bodymodel.body_deleted.ChangeID(id1, id2);
		
		// if ID was the last one, then update last_id
		if (id1 == bodymodel.last_id && id2 < bodymodel.last_id)
			bodymodel.last_id--;
		if (id2 > bodymodel.last_id)
			bodymodel.last_id = id2;
	} // +++ end data processing critical section	
	UpdateAll();
}
// ------------------------------------------------------------------------
void CNewVisionDoc::OnBodyDeleteID() {
	{  // +++ begin data processing critical section	
		CAutoLock dataLock( &dataCriticalSection); 
		int id = GetView(VIDEO_VIEW)->m_match;
		String ^s = String::Format("Delete ID = {0} ?", id);
		if (AfxMessageBox(CString(s), MB_ICONQUESTION | MB_YESNO ) == IDNO)
				return;
		m_TrackingData.data.RemoveKey(id);
		bodymodel.body.RemoveKey(id);
		bodymodel.body_deleted.RemoveKey(id);

		// if ID was the last one update last_id
		if (id == bodymodel.last_id)
			bodymodel.last_id--;
	} // +++ end data processing critical section	

	UpdateAll();
}

// ------------------------------------------------------------------------
void CNewVisionDoc::OnBodySetAsDefault() {
	{  // +++ begin data processing critical section	
		CAutoLock dataLock( &dataCriticalSection); 
		int id = GetView(VIDEO_VIEW)->m_match;
		m_DefaultTrackID = id;		
	} // +++ end data processing critical section	
}
// ------------------------------------------------------------------------
void CNewVisionDoc::OnBodyTerminateID() {
	{  // +++ begin data processing critical section	
		CAutoLock dataLock( &dataCriticalSection); 
		int id = GetView(VIDEO_VIEW)->m_match;
		String ^s = String::Format("Discontinue track {0} ?", id);
		if (AfxMessageBox(CString(s), MB_ICONQUESTION | MB_YESNO ) == IDNO)
				return;

		bodymodel.body.RemoveKey(id);
		bodymodel.body_deleted.RemoveKey(id);

		BodyPath* bp;
		if (m_TrackingData.data.Lookup(id, bp))
			bp->CutTail(trackermodel.m_frameNumber);
	} // +++ end data processing critical section	
	UpdateAll();
}
// ------------------------------------------------------------------------
void CNewVisionDoc::Message(String^ s) {
	messagemodel.m_list.InsertItem(messagemodel.m_list.GetItemCount(), (const char*)(Marshal::StringToHGlobalAnsi(s).ToPointer())  );
}
// ------------------------------------------------------------------------
void CNewVisionDoc::Message(CString s) {
	messagemodel.m_list.InsertItem(messagemodel.m_list.GetItemCount(), s);
}
// --------------------------------------------------------------------------
void CNewVisionDoc::LoadTrackingData() {
	char szFilters[] = "Tracking Data Files (*.tracking)|*.tracking|XML Files (*.xml)|*.xml|CVML Files (*.cvml)|*.cvml|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, ".tracking", "", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, szFilters, NULL);
	char title[]= {"Load Tracking Data From..."};
	dlg.m_ofn.lpstrTitle = title;
	if (dlg.DoModal() == IDOK) {
		{  // +++ begin data processing critical section	
			CAutoLock dataLock( &dataCriticalSection); 
			CString filename = dlg.GetPathName();
			ShowProgressDialog("Loading tracking data...");
			if (dlg.GetFileExt() == "xml")
				;
			else if (dlg.GetFileExt() == "cvml")
				m_TrackingData.ImportFromCVML(filename);
			else if (dlg.GetFileExt() == "csv")
				m_TrackingData.ImportFromCSV(filename);
			else {
				CFile file(dlg.GetPathName(), CFile::modeRead);
				CArchive ar(&file, CArchive::load);
				m_TrackingData.Serialize(ar);
				ar.Close();
				file.Close();
			}
		} // +++ end data processing critical section	
		HideProgressDialog();
		if (this->m_ProcessingMode == TRACKING)
			LoadLastTrackedFrame();
		else {
			// update display
			UpdateOverlayInfo();
			trackermodel.SetPosition();
			if (theApp.m_pMainWnd)
				theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
		}
	
	}	
}
// --------------------------------------------------------------------------
void CNewVisionDoc::SaveTrackingData() {
	char szFilters[] = "Tracking Data Files (*.tracking)|*.tracking|Comma Separated Files (*.csv)|*.csv|XML Files (*.xml)|*.xml|CVML Files (*.cvml)|*.cvml|X3D Files (*.x3d)|*.x3d|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, ".tracking", "Tracking Data 01", OFN_OVERWRITEPROMPT |OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, szFilters, NULL);
	char title[]= {"Save Tracking Data As..."};
	dlg.m_ofn.lpstrTitle = title;
	if (dlg.DoModal() == IDOK) {
		CString filename = dlg.GetPathName();
		ShowProgressDialog("Saving tracking data...");
		if (dlg.GetFileExt() == "xml")
			m_TrackingData.ExportToXML(filename);
		else if (dlg.GetFileExt() == "cvml")
			m_TrackingData.ExportToCVML(filename);
		else if (dlg.GetFileExt() == "csv") {
			int result = AfxMessageBox("Generate static model?", MB_YESNOCANCEL);
			if (result == IDYES) {
				//m_TrackingData.ExportToX3DStatic(filename);
				m_TrackingData.ExportToCSVStatic(filename);
			}
			else if (result == IDNO)
				m_TrackingData.ExportToCSV(filename);
		}
		else if (dlg.GetFileExt() == "x3d") {
			int result = AfxMessageBox("Generate static model?", MB_YESNOCANCEL);
			if (result == IDYES) {
				//m_TrackingData.ExportToX3DStatic(filename);
				m_TrackingData.ExportToX3DClusteredTraffic(filename);
			}
			else if (result == IDNO)
				m_TrackingData.ExportToX3D(filename);
		}
		else {
			CFile file(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite);
			CArchive ar(&file, CArchive::store);
			m_TrackingData.Serialize(ar);
			ar.Close();
			file.Close();
		}
		HideProgressDialog();
	}	
}
// --------------------------------------------------------------------------
void CNewVisionDoc::LoadHeatmapData() {
	char szFilters[] = "Heatmap Data Files (*.heatmap)|*.heatmap|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, ".heatmap", "", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, szFilters, NULL);
	char title[]= {"Load Heatmap Data From..."};
	dlg.m_ofn.lpstrTitle = title;
	if (dlg.DoModal() == IDOK) {
		{  // +++ begin data processing critical section	
			CAutoLock dataLock( &dataCriticalSection); 
			CString filename = dlg.GetPathName();
			ShowProgressDialog("Loading Heatmap data...");
			CFile file(dlg.GetPathName(), CFile::modeRead);
			CArchive ar(&file, CArchive::load);
			m_HeatmapData.Serialize(ar);
			ar.Close();
			file.Close();
			HideProgressDialog();
		} // +++ end data processing critical section	
	} 
}
// --------------------------------------------------------------------------
void CNewVisionDoc::SaveHeatmapData() {
	char szFilters[] = "Heatmap Data Files (*.heatmap)|*.heatmap|Image Files(*.png, *.bmp)|*.png;*.bmp|Comma Separated Files (*.csv)|*.csv|XML Files (*.xml)|*.xml|X3D Files (*.x3d)|*.x3d|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, ".heatmap", "Heatmap Data 01", OFN_OVERWRITEPROMPT |OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, szFilters, NULL);
	char title[]= {"Save Heatmap Data As..."};
	dlg.m_ofn.lpstrTitle = title;
	if (dlg.DoModal() == IDOK) {
		CString filename = dlg.GetPathName();
		ShowProgressDialog("Saving heatmap data...");
		if (dlg.GetFileExt() == "xml") {
			cvSave(filename, m_HeatmapData.map);
			cvSave(filename, m_HeatmapData.count);
		}
		else if (dlg.GetFileExt() == "csv")
			;//m_HeatmapData.ExportToCSV(filename);
		else if (dlg.GetFileExt() == "x3d") {
			int result = AfxMessageBox("Generate static model?", MB_YESNOCANCEL);
			if (result == IDYES) {
				double SCALE = 400;
				m_HeatmapData.ExportToX3DStatic(filename, SCALE);
			}
			else if (result == IDNO)
				m_HeatmapData.ExportToX3D(filename);
		}
		else if (dlg.GetFileExt() == "bmp" || dlg.GetFileExt() == "png") {
			m_HeatmapData.DrawHeatMap(motionmapscaled, false);
			cvSaveImage(filename, motionmapscaled);
			cvScale(m_HeatmapData.count, motionmaptemp);
			cvSaveImage(filename+"_COUNTS.png", motionmaptemp);
		}
		else {
			CFile file(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite);
			CArchive ar(&file, CArchive::store);
			m_HeatmapData.Serialize(ar);
			ar.Close();
			file.Close();
		}
		HideProgressDialog();
	}	
}
// --------------------------------------------------------------------------
void CNewVisionDoc::LoadActivityData() {
	char szFilters[] = "Data Files (*.activity)|*.activity|Comma Separated Files (*.csv)|*.csv|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, ".csv", "Activity Data 01", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, szFilters, NULL);
	char title[]= {"Load Activity Data ..."};
	dlg.m_ofn.lpstrTitle = title;
	if (dlg.DoModal() == IDOK) {
		CString filename = dlg.GetPathName();
		if (dlg.GetFileExt() == "csv")
			m_ActivityData.ImportFromCSV(filename);
		else {
			CFile file(dlg.GetPathName(), CFile::modeRead);
			CArchive ar(&file, CArchive::load);
			m_ActivityData.Serialize(ar);
			ar.Close();
			file.Close();
		}
	}	
}
// --------------------------------------------------------------------------
void CNewVisionDoc::SaveActivityData() {
	char szFilters[] = "Data Files (*.activity)|*.activity|Comma Separated Files (*.csv)|*.csv|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, ".csv", "Activity Data 01", OFN_OVERWRITEPROMPT |OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, szFilters, NULL);
	char title[]= {"Save Activity Data As..."};
	dlg.m_ofn.lpstrTitle = title;
	if (dlg.DoModal() == IDOK) {
		CString filename = dlg.GetPathName();
		if (dlg.GetFileExt() == "csv")
			m_ActivityData.ExportToCSV(filename);
		else {
			CFile file(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite);
			CArchive ar(&file, CArchive::store);
			m_ActivityData.Serialize(ar);
			ar.Close();
			file.Close();
		}
	}	
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnVisualizeDataTracks() {
	ShowProgressDialog("Generating track image", m_TrackingData.data.GetCount());
	cvSet(floorImg, CV_RGB(255,255,255));
	floormodel.Draw(floorImg);
	m_TrackingData.DrawTracksFloor(floorImg, CV_RGB(0,255,0)); 
	obstaclemodel.DrawFloor(floorImg, CV_RGB(255,128,0));
	doormodel.DrawFloor(floorImg, CV_RGB(255,0,0));
	HideProgressDialog();
	OnDataSaveFloorView();
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnVisualizeDataTimeTags() {
	ShowProgressDialog("Generating time tag image", m_TrackingData.data.GetCount());
	cvSet(floorImg, CV_RGB(255,255,255));
	floormodel.Draw(floorImg);
	m_TrackingData.DrawTimeTagsFloor(floorImg, CV_RGB(0,255,0)); 
	//zonemodel.DrawFloor(floorImg, CV_RGB(255,128,0));
	obstaclemodel.DrawFloor(floorImg, CV_RGB(255,128,0));
	HideProgressDialog();
	OnDataSaveFloorView();
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnVisualizeDataHeatmap()
{
	cvZero(motionmap);
	// Get heatmap time range from the user
	CTextEntryDlg dlgTextEntry;
	if (!trackermodel.IsInitialized()) {
		AfxMessageBox("Initialize tracker model first!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	REFERENCE_TIME start, stop;
	if (
		getTime("Enter Start Time (0 - for all)", "Enter Time (hh:mm:ss)?", _T("0"), start)
		&& getTime("Enter Start Time (0 - for all)", "Enter Time (hh:mm:ss)?", _T("0"), stop) ) {

		int segments = 1;
		if (getNumber("How many segments to break into? (1 - default)", "Enter number of segments?", _T("1"), segments)) {
			m_TrackingData.GenerateTrackHeatmap(start, stop, segments);
			//cvSet(floorImg, CV_RGB(255,255,255));
			//floormodel.Draw(floorImg);
			//m_HeatmapData.DrawHeatMap(motionmapscaled);
			//obstaclemodel.DrawFloor(floorImg, CV_RGB(255,128,0));
			//doormodel.DrawFloor(floorImg, CV_RGB(255,0,0));
			//SaveHeatmapData();
		}
	}
}
// --------------------------------------------------------------------------
bool CNewVisionDoc::getTime(LPCTSTR pszTitle, LPCTSTR pszPrompt, LPCTSTR pszDefault, REFERENCE_TIME &t) {
	CTextEntryDlg dlgTextEntry;
	if (dlgTextEntry.Show(theApp.m_pMainWnd, pszTitle, pszPrompt, pszDefault) == IDOK) {
		CString strResult = dlgTextEntry.GetText();
		int H=0, M=0, S=0;
		if (sscanf_s(strResult, "%2d%2d%2d", &H, &M, &S) < 3)
			if (sscanf_s(strResult, "%d.%d.%d", &H, &M, &S) < 3)
				if (sscanf_s(strResult, "%d:%d:%d", &H, &M, &S) < 3)
					return false;
		int h = trackermodel.m_startTime.GetHour();
		int m = trackermodel.m_startTime.GetMinute();
		int s = trackermodel.m_startTime.GetSecond();
		t = (_int64)((((H-h)*60+(M-m))*60+(S-s))*trackermodel.m_frameRate);
		return true;
	}
	return false;
}
// --------------------------------------------------------------------------
bool CNewVisionDoc::getNumber(LPCTSTR pszTitle, LPCTSTR pszPrompt, LPCTSTR pszDefault, int &t) {
	CTextEntryDlg dlgTextEntry;
	if (dlgTextEntry.Show(theApp.m_pMainWnd, pszTitle, pszPrompt, pszDefault) == IDOK) {
		CString strResult = dlgTextEntry.GetText();
		t = atoi(strResult);
		return true;
	}
	return false;
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnVisualizeDataDwellHeatmap()
{
	m_TrackingData.GenerateDwellHeatmap();
	//cvSet(floorImg, CV_RGB(255,255,255));
	//floormodel.Draw(floorImg);
	m_HeatmapData.DrawHeatMap(motionmapscaled);
	SaveHeatmapData();
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnVisualizeDataMotion() {
	//visDlg = gcnew NewVision::VisualizationForm(this);
	//visDlg->Text = gcnew String("Visual Direction of Motion ");
	//visDlg->Show();
}
// --------------------------------------------------------------------------
void CNewVisionDoc::OnActivityDetectEvents()
{
	int nFrames = trackermodel.m_frameTotal;
	ShowProgressDialog("Detecting swarming events", nFrames);
	for (int nFrame = trackermodel.m_frameStart; nFrame < trackermodel.m_frameStart+nFrames; nFrame++) {
		StepProgressDialog((CString)String::Format("Processing frame {0} out of {1}", nFrame, nFrames));
		//bodyactivitymodel.DetectEvents(nFrame);
		swarmactivitymodel.DetectEvents(nFrame);
		//m_ActivityData.Put(nFrame);
	}
	HideProgressDialog();
}
// ------------------------------------------------------------------------
void CNewVisionDoc::OnActivityDetectActivities()
{
	swarmactivitymodel.DetectActivities();
}
// ------------------------------------------------------------------------
void CNewVisionDoc::OnActivityFilterActivities()
{
	CMap<int, int, int, int> keysToDelete;
	int dummy;
	// =============== Filter Our Insignificant Actors
	for (POSITION pos1 = m_ActivityData.sActivity.GetStartPosition(); pos1 != NULL;) {
		int cid1; SwarmActivity* c1;
		m_ActivityData.sActivity.GetNextAssoc(pos1, cid1, c1);
		// exclude employees
		if (m_TrackingData.m_initialized) {
			for (POSITION posA = c1->actors.GetStartPosition(); posA != NULL;) {
				int aid; double w;
				c1->actors.GetNextAssoc(posA, aid, w);
				BodyPath *bp;
				m_TrackingData.data.Lookup(aid, bp);
				CString employee="";
				bp->tags.Lookup("Employee", employee);
				// if employee in event, delete it
				if (employee == "Yes") {
					keysToDelete[cid1] = 1;
					break;
				}
			}
		}
		c1->Normalize();
		c1->FilterActors(((GroupingEventDetector*)(swarmactivitymodel.m_eventDetector[1]))->m_minActorWeight);
		c1->Normalize();
	}
	// =============== Join duplicate clusters (i.e. containing all same actors)
	for (POSITION pos1 = m_ActivityData.sActivity.GetStartPosition(); pos1 != NULL;) {
		int cid1; SwarmActivity* c1;
		m_ActivityData.sActivity.GetNextAssoc(pos1, cid1, c1);
		// if activity is marked for deletion
		if (keysToDelete.Lookup(cid1, dummy))
			continue;
		if (c1->actors.GetCount() < 2) {
			keysToDelete[cid1] = 1;
			continue;
		}
		for (POSITION pos2 = m_ActivityData.sActivity.GetStartPosition(); pos2 != NULL;) {
			int cid2; SwarmActivity* c2;
			m_ActivityData.sActivity.GetNextAssoc(pos2, cid2, c2);
			// if activity is marked for deletion
			if (keysToDelete.Lookup(cid2, dummy))
				continue;
			if (cid1 == cid2)
				continue;
			// if found a match
			int aCount = c1->EqualActors(c2);
			if (aCount == c1->actors.GetCount() && aCount == c2->actors.GetCount()) {
				c1->Merge(c2);
				keysToDelete[cid2] = 1;
			}
		}
	}
	for (POSITION pos1 = keysToDelete.GetStartPosition();pos1;) {
		int i, dummy;
		keysToDelete.GetNextAssoc(pos1, i, dummy);
		m_ActivityData.sActivity.RemoveKey(i);
	}

}
// ------------------------------------------------------------------------
void CNewVisionDoc::OnActivityFilterActivities2()
{
	double CONFIDENCE = 0.55;
	CMap<int, int, int, int> keysToDelete;
	int dummy;
	// =============== Filter Our Insignificant Actors
	double pMean = 0;
	for (POSITION pos1 = m_ActivityData.sActivity.GetStartPosition(); pos1 != NULL;) {
		int cid1; SwarmActivity* c1;
		m_ActivityData.sActivity.GetNextAssoc(pos1, cid1, c1);
		c1->Normalize();
		c1->FilterActors(((GroupingEventDetector*)(swarmactivitymodel.m_eventDetector[1]))->m_minActorWeight);
		c1->Normalize();
		pMean += c1->p;
	}
	pMean /= m_ActivityData.sActivity.GetCount();

	// =============== Merge High Prob. Clusters with at Least One Common Actor ==================
	for (POSITION pos1 = m_ActivityData.sActivity.GetStartPosition(); pos1 != NULL;) {
		int cid1; SwarmActivity* c1;
		m_ActivityData.sActivity.GetNextAssoc(pos1, cid1, c1);
		// if activity is marked for deletion
		if (keysToDelete.Lookup(cid1, dummy))
			continue;
		if (c1->p < CONFIDENCE*pMean)
			continue;
		for (POSITION pos2 = m_ActivityData.sActivity.GetStartPosition(); pos2 != NULL;) {
			int cid2; SwarmActivity* c2;
			m_ActivityData.sActivity.GetNextAssoc(pos2, cid2, c2);
			// if activity is marked for deletion
			if (keysToDelete.Lookup(cid2, dummy))
				continue;
			if (c2->p < CONFIDENCE*pMean )
				continue;
			if (cid1 == cid2)
				continue;
			// if found a match
			if (c1->EqualActors(c2) > 0) {
				c1->Merge(c2);
				keysToDelete[cid2] = 1;
			}
		}
		if (c1->actors.GetCount() < 2) {
			keysToDelete[cid1] = 1;
			continue;
		}
	}
	for (POSITION pos1 = keysToDelete.GetStartPosition();pos1;) {
		int i, dummy;
		keysToDelete.GetNextAssoc(pos1, i, dummy);
		m_ActivityData.sActivity.RemoveKey(i);
	}
	// =============== Add Low Probability Clusters to with at Least One Common Actor to High Prob Clusters ==================
	for (POSITION pos1 = m_ActivityData.sActivity.GetStartPosition(); pos1 != NULL;) {
		int cid1; SwarmActivity* c1;
		m_ActivityData.sActivity.GetNextAssoc(pos1, cid1, c1);
		if (c1->p < CONFIDENCE*pMean)
			continue;
		for (POSITION pos2 = m_ActivityData.sActivity.GetStartPosition(); pos2 != NULL;) {
			int cid2; SwarmActivity* c2;
			m_ActivityData.sActivity.GetNextAssoc(pos2, cid2, c2);
			// if activity is marked for deletion
			if (keysToDelete.Lookup(cid2, dummy))
				continue;
			if (c2->p >= CONFIDENCE*pMean)
				continue;
			if (cid1 == cid2)
				continue;
			// if found a match
			if (c1->EqualActors(c2) > 0) {
				c1->Merge(c2);
			}
		}
	}

	// =============== Remove Low Prob. Clusters ==================
	keysToDelete.RemoveAll();
	for (POSITION pos1 = m_ActivityData.sActivity.GetStartPosition(); pos1 != NULL;) {
		int cid1; SwarmActivity* c1;
		m_ActivityData.sActivity.GetNextAssoc(pos1, cid1, c1);
		// if activity is marked for deletion
		if (keysToDelete.Lookup(cid1, dummy))
			continue;
		if (c1->p < CONFIDENCE*pMean)
			keysToDelete[cid1] = 1;
	}
	for (POSITION pos1 = keysToDelete.GetStartPosition();pos1;) {
		int i, dummy;
		keysToDelete.GetNextAssoc(pos1, i, dummy);
		m_ActivityData.sActivity.RemoveKey(i);
	}

	// =============== Filter Our Insignificant Actors
	for (POSITION pos1 = m_ActivityData.sActivity.GetStartPosition(); pos1 != NULL;) {
		int cid1; SwarmActivity* c1;
		m_ActivityData.sActivity.GetNextAssoc(pos1, cid1, c1);
		c1->Normalize();
		c1->FilterActors(((GroupingEventDetector*)(swarmactivitymodel.m_eventDetector[1]))->m_minActorWeight);
		c1->Normalize();
		if (c1->actors.GetCount() < 2) {
			m_ActivityData.sActivity.RemoveKey(cid1);
			continue;
		}
	}

	// ================= Update Activity Start/Stop information
	if (m_TrackingData.m_initialized) {
		for (POSITION pos = m_ActivityData.sActivity.GetStartPosition(); pos != NULL; ) {
			int id; SwarmActivity *ac;
			m_ActivityData.sActivity.GetNextAssoc(pos, id, ac);	
			ac->UpdateStartStop(&m_TrackingData);
		}
	}
}
// ------------------------------------------------------------------------
// CNewVisionDoc diagnostics
#ifdef _DEBUG
void CNewVisionDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNewVisionDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif //_DEBUG


// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void CNewVisionDoc::SetTagValue(CString key, CString value) {
	BodyPath* bp;
	if (m_TrackingData.data.Lookup(m_DefaultTrackID, bp)) {
		// no value provided, use current video time as a values
		if (value.IsEmpty()) {
			//CTimeSpan elapsed((int)(trackermodel.m_frameNumber / trackermodel.m_frameRate));
			//CTime time = trackermodel.m_startTime + elapsed;
			// time.Format( "%a, %m/%d/%y, %I:%M:%S%p")
			CString oldValue = bp->GetTagValue(key);
			if (oldValue != "")
				oldValue += ",";
			bp->SetTagValue(key, oldValue+String::Format("{0}", (int)(trackermodel.m_frameNumber) ));
		}
	}
}
