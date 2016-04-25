/* ======================================================================================
BODYACTIVITYMODEL - contains individual activity recognition parameters and stores BODYHISTORY

BODYPATH - Is a list of body coordinates and other parameters recorded at each frame
of the tracking process.
======================================================================================== */

#if !defined( BODYACTIVITYMODEL_H )
#define BODYACTIVITYMODEL_H
#pragma once

#include <afxtempl.h>
#include "cv.h"
#include "Model.h"
#include "SmartPtrArray.h"
#include "LABHistogram2D.h"

class SwarmActivityModel;
class Body;
class CNewVisionDoc;
class BodyPath;
class Step;
class FloorModel;
class TrackerModel;

#define DWELL_POINT_SCALE 1	
#define DWELL_POINT_MAX_R 15
#define DWELL_POINT_COLOR CV_RGB(200, 170, 10)


class BodyActivityModel : public Model
{
DECLARE_SERIAL(BodyActivityModel)
public:
	// --------------- Persistant variables -----------------------
	bool m_initialized;

	double m_SalesRepHistThreshold;
	int m_pathSmoothSigma; 
	int m_dwellTime;       
	double m_dwellAreaRadius; // in centimeters
	LABHistogram2D m_salesRepHist;
	CArray<int> m_salesRepID; // the ID's used in learning only; current sales reps are estimated at a run time


	BodyActivityModel(void);
	bool Initialize();
	void DeInitialize();
	bool IsInitialized() { return m_initialized; };
	virtual ~BodyActivityModel(void);

	BodyPath* FindBodyMatch(int id);
	BodyPath* FindBodyMatch(Body* b);

	void UpdateSalesRepHist();
	void DrawSalesReps(IplImage* frame, CvScalar color);
	void DrawDwellingBodies(IplImage* frame, CvScalar color);
	void DrawPath(IplImage* frame, CvScalar color=CV_RGB(0,0,0));

	void DetectEvents(int nFrame);
	void ExportEvents(int nFrame);
	void ExportToDAT(CString filename);
	void ImportFromDAT(CString filename);
	void ExportToCVML(CString filename);
	void ImportFromCVML(CString filename);
	void ExportToCSV(CString filename);

	void Empty();
};

template <> void AFXAPI SerializeElements <BodyPath*> ( CArchive& ar, BodyPath** p, INT_PTR nCount );
template <> void AFXAPI SerializeElements <Step> ( CArchive& ar, Step* p, INT_PTR nCount );
template <> void AFXAPI SerializeElements <CvPoint2D32f> ( CArchive& ar, CvPoint2D32f* p, INT_PTR nCount );

#endif // !defined( BODYACTIVITYMODEL_H )
