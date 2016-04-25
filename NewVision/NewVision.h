// NewVision.h : main header file for the NewVision application
#if !defined( NEWVISION_H )
#define NEWVISION_H 
#pragma once
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "prsht.h"
#include "ShortcutMap.h"

#ifdef _DEBUG
	#define CRTDBG_MAP_ALLOC
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

namespace NewVision {
	ref class AppSettings;
	ref class PedestrianTags;
};


// CNewVisionApp: See NewVision.cpp for the implementation of this class
class CNewVisionApp : public CWinApp
{
	ShortCutMap tagShortcutsMap;			// stores manually added information tag shortcut mappings
public:
	CNewVisionApp();
	gcroot<NewVision::AppSettings ^> appSettingsForm;
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();
	void			ReadConfiguration();
	void			WriteConfiguration();
	BOOL			PreTranslateMessage(MSG* pMsg);
	CNewVisionDoc*	GetDocument();

// Implementation
	afx_msg void OnAppAbout();
	afx_msg void OnAppSettings();
	DECLARE_MESSAGE_MAP()
};
extern CNewVisionApp theApp;

// ---------------- Global Menu Initialization Routines
extern bool displayTrackInfo;
extern bool displayVideoInputImage;
extern bool displayVideoBodies;
extern bool displayVideoOrientations;
extern bool displayVideoBlobs;
extern bool displayVideoBodyHeights;
extern bool displayVideoBlobHeights;
extern bool displayVideoTimeStamp;
extern bool displayVideoDoors;
extern bool displayVideoObstacles;
extern bool displaySceneBG;
extern bool displaySceneZ;
extern bool displaySceneBodies;
extern bool displaySceneBlobs;
extern bool displaySceneGroups;
extern bool displayScenePaths;
extern bool displaySceneDoors;
extern bool displaySceneCamera;
extern bool displaySceneHistograms;
extern bool displaySceneDistancePlane;
extern bool displayActivityBodies;
extern bool displayActivityGroups;
extern bool displayActivitySalesReps;
extern bool displayActivityDwellingBodies;
extern bool displayFloorFloorMap;
extern bool displayFloorBodyMaps;
extern bool displayFloorBodyPaths;
extern bool displayFloorBodyTracks;
extern bool displayFloorHeatMap;
extern bool displayFloorObstacles;
extern bool displayFloorZones;
extern bool displayFloorTimeStamp;
extern void ResetMenus();

#endif // !defined( NEWVISION_H )