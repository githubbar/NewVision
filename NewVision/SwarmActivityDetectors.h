#if !defined( SWARMACTIVITYDETECTORS_H )
#define SWARMACTIVITYDETECTORS_H
#pragma once

#include "cv.h"
#include "SmartPtrArray.h"
#include "Resource.h"
#include "TabPageSSL.h"

#include "ClusteringMachine.h"

class CNewVisionDoc;
class Body;
class BodyCluster;
class BodyClusterElement;
class SwarmEvent;
class SwarmActivity;

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
enum ActivityType {AC_PROXIMITY=0, AC_GROUPING=1, AC_FOLLOW_LEADER=2, AC_LINE_UP=3, AC_CO_DWELL=4};
// --------------------------------------------------------------------------
class SwarmEventDetector : public CTabPageSSL {
	public:
	int IDD;
	CString name;
	int m_everyNframe;             // perform event detection every N-th frame
	SwarmEventDetector() {};
	SwarmEventDetector(CString name, int idd) {CTabPageSSL (idd, NULL); this->name = name; IDD = idd;};
	virtual int    DetectEvents(int nFrame, CNewVisionDoc* doc) {return 0;};
	virtual int    DetectActivities(int nFrame, CNewVisionDoc* doc) {return 0;};
	virtual int	   PostProcessActivities(CNewVisionDoc* doc) {return 0;};
	virtual double ComputeDistance(BodyCluster* bi, BodyCluster* bj) {return 0;};	
	virtual double ComputeSpread(BodyCluster* bi) {return 0;};	
	virtual double ComputeGroupClusteringValidity(CArray<BodyCluster*>& cl) {return 0;};
	virtual void   Serialize(CArchive& ar) { ar.IsStoring() ? ar << m_everyNframe : ar >> m_everyNframe; };
};
// ==========================================================================
class ProximityEventDetector : public SwarmEventDetector {
	public:
	ProximityEventDetector(CString name, int idd) {this->name = name; IDD = idd;};
	double m_distanceThreshold; // max distance in (cm) to be a part of the same group
	int    m_maturePath;        // min number of (seconds) in a path to consider it for grouping 
	int    m_timeDelta;         // time interval +/- to consider the distances at      
	virtual int DetectEvents(int nFrame, CNewVisionDoc* doc);
	virtual double ComputeDistance(BodyCluster* bi, BodyCluster* bj) {return 0;};	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX); 
	virtual BOOL OnInitDialog();
	virtual void Serialize(CArchive& ar);
	DECLARE_MESSAGE_MAP()
};
// --------------------------------------------------------------------------
class GroupingEventDetector: public SwarmEventDetector {
	public:
	GroupingEventDetector(CString name, int idd);
	double m_dwellWeight;             // Co-dwelling: co-dwell with swarm mates
	double m_alignmentWeight;         // Alignment: steer towards the average heading of the local flock-mates
	double m_cohesionWeight;          // Cohesion: steer to move toward the average position of local flock-mates

	double m_maxGroupDiameter;          // Maximum distance (cm) from the center of the group to be considered a part of the group
	int m_EventEndBeforeFrame;
	double m_minEventWeight;		// Only use cluster members that have a weight more than this (sum of weights = 1)
	double m_minActorWeight;		// Only include actor members that have a weight more than this (sum of weights in activity = 1)
	ClusteringParams param;


	virtual void DoDataExchange(CDataExchange* pDX); 
	virtual BOOL OnInitDialog();
	virtual double ComputeDistance(BodyClusterElement* bi, BodyClusterElement* bj);	
	virtual double ComputeSpread(BodyCluster* bi);	
	virtual double ComputeGroupClusteringValidity(CNewVisionDoc* doc, CArray<BodyCluster*>& cl, BodyCluster* all, CvMat* D_elements);
	virtual int DetectEvents(int nFrame, CNewVisionDoc* doc);
	
	virtual int	PostProcessActivities(CNewVisionDoc* doc);
	virtual int DetectActivities(int nFrame, CNewVisionDoc* doc);
	
	virtual void Serialize(CArchive& ar);
	DECLARE_MESSAGE_MAP()
};
// --------------------------------------------------------------------------
class LeaderFollowingEventDetector: public SwarmEventDetector {
	public:
	LeaderFollowingEventDetector(CString name, int idd) {this->name = name; IDD = idd;};
	virtual void DoDataExchange(CDataExchange* pDX) {}; 
	virtual int DetectEvents(int nFrame, CNewVisionDoc* doc);
	virtual void Serialize(CArchive& ar) {};
	DECLARE_MESSAGE_MAP()
};
// --------------------------------------------------------------------------
class LiningUpEventDetector: public SwarmEventDetector {
	public:
	LiningUpEventDetector(CString name, int idd) {this->name = name; IDD = idd;};
	virtual void DoDataExchange(CDataExchange* pDX) {}; 
	virtual int DetectEvents(int nFrame, CNewVisionDoc* doc);
	virtual void Serialize(CArchive& ar) {};
	DECLARE_MESSAGE_MAP()
};
// --------------------------------------------------------------------------
class CoDwellingEventDetector: public SwarmEventDetector {
	public:
	CoDwellingEventDetector(CString name, int idd) {this->name = name; IDD = idd;};
	virtual void DoDataExchange(CDataExchange* pDX) {}; 
	virtual int DetectEvents(int nFrame, CNewVisionDoc* doc);
	virtual void Serialize(CArchive& ar) {};
	DECLARE_MESSAGE_MAP()
};

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

#endif // !defined( SWARMACTIVITYDETECTORS_H )