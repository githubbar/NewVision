#if !defined( NEWVISIONDOC_H )
#define NEWVISIONDOC_H 
#pragma once
#include "cv.h"    
#include "TrackerDialog.h"
#include "BackgroundDialog.h"
#include "BlobDialog.h"
#include "BodyDialog.h"
#include "BodyActivityDialog.h"
#include "SwarmActivityDialog.h"
#include "SkinDialog.h"
#include "DoorDialog.h"
#include "CameraDialog.h"
#include "FloorDialog.h"
#include "ObstacleDialog.h"
#include "ZoneDialog.h"
#include "MessageDialog.h"
#include "TreePropSheet.h"

using namespace TreePropSheet;
class CNewVisionView;		
class CMainFrame;

// designates the type of left mouse click action
enum CursorType {CURSOR_DEFAULT=0, CURSOR_PICK_SALES_REP=1, CURSOR_MOVE_BODY=2};
enum ProcessingMode {TRACKING=0, HEATMAP=1, ACTIVITY=2, PLAYBACK=3};

namespace NewVision {
	ref class Tracks;
	//ref class VisualizationForm;
};


class CNewVisionDoc : public CDocument
{
protected: // create from serialization only
	DECLARE_SERIAL(CNewVisionDoc)
	DECLARE_MESSAGE_MAP()

public:
	CvPoint2D32f test1, test2;
	TrackerDialog         trackermodel;
	BackgroundDialog      bgmodel;
	BlobDialog            blobmodel;
	BodyDialog            bodymodel;
	SkinDialog		      skinmodel;
	DoorDialog            doormodel;
	CameraDialog          cameramodel;
	FloorDialog           floormodel;
	ObstacleDialog	      obstaclemodel;
	BodyActivityDialog    bodyactivitymodel;
	SwarmActivityDialog   swarmactivitymodel;
	ZoneDialog			  zonemodel;
	MessageDialog         messagemodel;
	CTreePropSheet        settings;
	HWND                  m_parentFrame;
	bool                  m_initialized;
	// --------------- Persistant variables --------------------------
	TrackingData     m_TrackingData;
	HeatmapData      m_HeatmapData;
	ActivityData     m_ActivityData;

	// -------------- Computing control variables ---------------------
	int  m_ProcessingMode;
	int  m_DefaultTrackID; // default ID to merge with, -1 if none is set
	// -------------- Display variables -------------------------------
	gcroot<NewVision::Tracks^>				infoDlg; 
	//gcroot<NewVision::VisualizationForm^>	visDlg; 
	int  m_cursor;

	void Message(System::String^ s); // display debug message to the message window
	void Message(CString s); // display debug message to the message window
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
					CNewVisionDoc();
	virtual			~CNewVisionDoc();
	bool			Initialize();
	void			AddModel(CObject* m);
	CNewVisionView* GetView(int type);
	bool			ChangeMenuItemState(int sub, int item);
	void 			UpdateOverlayInfo();
	void			UpdateAll();
	virtual void 	DeleteContents();
	virtual void 	Serialize(CArchive& ar);

	virtual BOOL 	OnNewDocument();
	virtual BOOL 	OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL 	OnSaveDocument(LPCTSTR lpszPathName);
	virtual void 	OnCloseDocument();
	void			EnableNavigation(bool enable);
	void			AutoSave();
	void			SetTagValue(CString key, CString value="");
	bool			getTime(LPCTSTR pszTitle, LPCTSTR pszPrompt, LPCTSTR pszDefault, REFERENCE_TIME &t);
	bool			getNumber(LPCTSTR pszTitle, LPCTSTR pszPrompt, LPCTSTR pszDefault, int &t);

	afx_msg void	LoadLastTrackedFrame();
	afx_msg void 	OnPlay();
	afx_msg void 	OnPause();
	afx_msg void 	OnREW1();
	afx_msg void 	OnREW5();
	afx_msg void 	OnREW10();
	afx_msg void 	OnFFW1();
	afx_msg void 	OnFFW5();
	afx_msg void 	OnFFW10();
	afx_msg void 	OnGotoFrame();
	afx_msg void 	OnGotoTime();
	afx_msg void 	OnSetPlaybackRate();
	afx_msg void 	OnStop();
	afx_msg void 	OnUpdateBG();
	afx_msg void 	OnSettingsParameters();
	afx_msg void 	OnDisplayLeftInputImage();
	afx_msg void 	OnDisplayLeftBodies();
	afx_msg void 	OnDisplayLeftOrientations();
	afx_msg void 	OnDisplayLeftBlobs();
	afx_msg void 	OnDisplayLeftBodyHeights();
	afx_msg void 	OnDisplayLeftBlobHeights();
	afx_msg void 	OnDisplayLeftTimeStamp();
	afx_msg void 	OnDisplayLeftDoors();
	afx_msg void 	OnDisplayLeftObstacles();
	afx_msg void 	OnDisplayRightBG();
	afx_msg void 	OnDisplayRightZ();
	afx_msg void 	OnDisplayRightBodies();
	afx_msg void 	OnDisplayRightBlobs();
	afx_msg void 	OnDisplayRightGroups();
	afx_msg void 	OnDisplayRightPaths();
	afx_msg void 	OnDisplayRightDoors();
	afx_msg void 	OnDisplayRightCamera();
	afx_msg void 	OnDisplayRightHistograms();
	afx_msg void 	OnDisplayRightDistancePlane();
	afx_msg void 	OnDisplayActivityBodies();
	afx_msg void 	OnDisplayActivityGroups();
	afx_msg void 	OnDisplayActivitySalesReps();
	afx_msg void 	OnDisplayActivityDwellingBodies();
	afx_msg void 	OnDisplayFloorFloorMap();
	afx_msg void 	OnDisplayFloorBodyMaps();
	afx_msg void 	OnDisplayFloorBodyPaths();
	afx_msg void 	OnDisplayFloorBodyTracks();
	afx_msg void 	OnDisplayFloorHeatMap();
	afx_msg void 	OnDisplayFloorObstacles();
	afx_msg void 	OnDisplayFloorZones();
	afx_msg void 	OnDisplayFloorTimeStamp();
	afx_msg void 	OnRecord();
	afx_msg int  	OnCreate();
	afx_msg void 	OnInitTracker();
	afx_msg void 	OnDataSaveVideoView();
	afx_msg void 	OnDataSaveSceneView();
	afx_msg void 	OnDataSaveActivityView();
	afx_msg void 	OnDataSaveFloorView();
	afx_msg void 	LoadTrackingData();
	afx_msg void 	LoadHeatmapData();	
	afx_msg void 	LoadActivityData();
	afx_msg void 	SaveTrackingData();
	afx_msg void 	SaveHeatmapData();
	afx_msg void 	SaveActivityData();
	afx_msg void 	OnBnClickedTrackingMode();
	afx_msg void 	OnBnClickedHeatmapMode();
	afx_msg void 	OnBnClickedActivityMode();
	afx_msg void 	OnBnClickedPlaybackMode();
	afx_msg void 	OnViewTracks();
	afx_msg void 	OnBodyEditInfoID();
	afx_msg void 	OnBodyMergeID();
	afx_msg void 	OnBodyAppendID();
	afx_msg void 	OnBodyChangeID();
	afx_msg void 	OnBodyDeleteID();
	afx_msg void 	OnBodyTerminateID();
	afx_msg void 	OnBodySetAsDefault();
	afx_msg void 	OnChooseMergeID();
	afx_msg void	OnVisualizeDataTracks();
	afx_msg void	OnVisualizeDataTimeTags();
	afx_msg void	OnVisualizeDataHeatmap();
	afx_msg void	OnVisualizeDataDwellHeatmap();
	afx_msg void	OnVisualizeDataMotion();
 	afx_msg void	OnBodyAddToGroup();

	afx_msg void	OnActivityDetectEvents();
	afx_msg void	OnActivityDetectActivities();
	afx_msg void	OnActivityFilterActivities();
	afx_msg void	OnActivityFilterActivities2();

};

#endif // !defined( NEWVISIONDOC_H )