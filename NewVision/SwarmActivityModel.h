/* ======================================================================================
SWARMACTIVITYMODEL - contains group activity recognition parameters and stores SWARMHISTORY

SWARMHISTORY - is a record of all swarming events (e.g. instantaneously forming groups) 
in the tracked video. 

SWARMEVENT - Is described by: 
	- type of event (GROUPING, LEADER FOLLOWING, LINING UP, CO-DWELLING, etc)
    - list of unique event participant IDs (bodies)
	- frame it occurred at
	- event likelihood

SWARMACTIVITY - Lasts for more than one frame and 
can be deduced from multiple SwarmEvent objects
======================================================================================== */

#if !defined( SWARMACTIVITYMODEL_H )
#define SWARMACTIVITYMODEL_H
#pragma once
#include "cv.h"    
#include "Model.h"
#include "Globals.h"
#include "SwarmActivityDetectors.h"
#include "SmartPtrArray.h"
class BodyPathCluster;
class CNewVisionDoc;

// --------------------------------------------------------------------------
class SwarmActivityModel : public Model
{
	DECLARE_SERIAL( SwarmActivityModel )

	public:
		bool m_initialized;
		CvSparseMat* sActorAdjacency;
		CvSparseMat* sEventAdjacency;

		// size constants
		static const N_EVENT_TYPES = 5;
		// checkboxes IDs
		int m_eventCheckIDC[N_EVENT_TYPES];
		// detectors
		(SwarmEventDetector* m_eventDetector)[N_EVENT_TYPES];
		
		// commmon activity recognition parameters
		int    m_eventType[N_EVENT_TYPES];                     // events to detect
		
		//// Methods
		SwarmActivityModel(void);
		bool Initialize();
		void DeInitialize();
		bool IsInitialized() { return m_initialized; };
		virtual ~SwarmActivityModel(void);
		void Empty();
		

		// TODO : move those to BodyPathCluster class or  BodyPath class
		void GetClusterDistances(CArray<BodyPathCluster*> &c, CvMat* d);
		void GetPathDistances(CvMat* d);
		BodyPathCluster* FindCluster(CArray<BodyPathCluster*> group, int groupid);
		
		void DetectEvents(int nFrame);
		void ExportEvents(int nFrame);
		void DrawEventsFrame(IplImage* frame, CvScalar color, int nFrame=-1);
		void DrawActivitiesFrame(IplImage* frame, CvScalar color, int nFrame=-1);

		void GetFirstEvent(int nFrame);
		bool GetNextEvent(int nFrame, SwarmEvent* &evOut);
		void GetFirstActivity(int nFrame);
		bool GetNextActivity(int nFrame, SwarmActivity* &acOut);
		

};
#endif // !defined( SWARMACTIVITYMODEL_H )