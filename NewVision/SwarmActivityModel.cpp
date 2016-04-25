#include "StdAfx.h"
#include "SwarmActivityModel.h"
#include "BodyPathCluster.h"
#include "NewVisionDoc.h"
#using <mscorlib.dll>
#using <System.dll>
#include "SwarmEvent.h"
#include "SwarmActivity.h"
using namespace System;
using namespace System::Diagnostics;

int eventsExtracted = 0;
int activitiesExtracted = 0;
// --------------------------------------------------------------------------
IMPLEMENT_SERIAL(SwarmActivityModel, Model, 1)
// --------------------------------------------------------------------------
void SwarmActivityModel::Empty() {
	doc->m_ActivityData.sEvent.RemoveAll();
	doc->m_ActivityData.sActivity.RemoveAll();
}
// --------------------------------------------------------------------------
SwarmActivityModel::SwarmActivityModel(void) {
	m_eventDetector[0] = new ProximityEventDetector("Spatial Proximity", IDDS_PROXIMITYEVENTDETECTOR);
	m_eventDetector[1] = new GroupingEventDetector("Grouping", IDDS_GROUPINGEVENTDETECTOR);
	m_eventDetector[2] = new LeaderFollowingEventDetector("Leader Following", IDDS_LEADERFOLLOWINGEVENTDETECTOR);
	m_eventDetector[3] = new LiningUpEventDetector("Lining Up", IDDS_LININGUPEVENTDETECTOR);
	m_eventDetector[4] = new CoDwellingEventDetector("Co-dwelling", IDDS_CODWELLINGEVENTDETECTOR);

	m_initialized = false;
}
// --------------------------------------------------------------------------
SwarmActivityModel::~SwarmActivityModel(void) {
	for (int i=0;i<N_EVENT_TYPES;i++)
		delete m_eventDetector[i];
}
// --------------------------------------------------------------------------
bool SwarmActivityModel::Initialize() {
	if (m_initialized)
		DeInitialize();
	int sizes[2] = {50, 50};
	sActorAdjacency = cvCreateSparseMat(2, sizes, CV_32FC1);
	//SwarmEvent::last_id = 0;
	//SwarmActivity::last_id = 0;
	m_initialized = true;
	return true;
}
// --------------------------------------------------------------------------
void SwarmActivityModel::DeInitialize() {
	if (!m_initialized)
		return;
	m_initialized = false;
	Empty();
	cvReleaseSparseMat(&sActorAdjacency);
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
BodyPathCluster* SwarmActivityModel::FindCluster(CArray<BodyPathCluster*> group, int groupid) {
	for (int i=0;i<group.GetCount();i++) 
		if (group[i]->id == groupid)
			return group[i];
	return NULL;
}
// --------------------------------------------------------------------------
void SwarmActivityModel::GetClusterDistances(CArray<BodyPathCluster*> &c, CvMat* d) {
	//cvSet(d, cvScalar(Double::MaxValue));
	//for (int i=0;i<c.GetCount();i++) {
	//	for (int j=0;j<c.GetCount();j++) {
	//		if (i != j) {
	//			double dist = c[i]->Distance(
	//				c[j], 
	//				doc->trackermodel.Seconds2Frames(m_maturePath), 
	//				doc->trackermodel.Seconds2Frames(m_timeDelta), 
	//				doc->trackermodel.Seconds2Frames(m_everyNframe) 
	//				);
	//			//echo(__box(dist));
	//			cvSetReal2D(d, i, j, dist);
	//		}
	//	}
	//}
}
// --------------------------------------------------------------------------
void SwarmActivityModel::GetPathDistances(CvMat* d) {
	//cvSet(d, cvScalar(Double::MaxValue));
	//for (int i=0;i<GetCount();i++) {
	//	for (int j=0;j<GetCount();j++) {
	//		if (i != j) {
	//			double dist = GetAt(i)->Compare(GetAt(j));
	//			cvSetReal2D(d, i, j, dist);
	//		}
	//	}
	//}
}

// --------------------------------------------------------------------------
void SwarmActivityModel::DetectEvents(int nFrame) {
	int ecount = 0;
	for (int i=0;i<N_EVENT_TYPES;i++)
		if (m_eventType[i]) {
			ecount += m_eventDetector[i]->DetectEvents(nFrame, doc);
		}

	int acount = 0;
	for (int i=0;i<N_EVENT_TYPES;i++)
		if (m_eventType[i]) {
			acount += m_eventDetector[i]->DetectActivities(nFrame, doc);
		}

}
// --------------------------------------------------------------------------
void SwarmActivityModel::ExportEvents(int nFrame) {

}
// --------------------------------------------------------------------------
void SwarmActivityModel::DrawEventsFrame(IplImage* frame, CvScalar color, int nFrame) {
	if (nFrame == -1)
		nFrame = doc->trackermodel.m_frameNumber;
	int RADIUS = 5;
	SwarmEvent* ev = NULL;
	GetFirstEvent(nFrame);
	while (GetNextEvent(nFrame, ev)) {
		POSITION pos = ev->actors.GetStartPosition(); 
		while (pos) {
			int id; double c;
			ev->actors.GetNextAssoc(pos, id, c);
			Body* b;
			if (doc->bodymodel.body.Lookup(id, b))
				cvCircle(frame, cvPointFrom32f(b->GetImageCenter()), RADIUS, colorFromID(ev->id), CV_FILLED, CV_AA);
		}
	}
}
// --------------------------------------------------------------------------
void SwarmActivityModel::DrawActivitiesFrame(IplImage* frame, CvScalar color, int nFrame) {
	if (nFrame == -1)
		nFrame = doc->trackermodel.m_frameNumber;
	int RADIUS = 10;
	SwarmEvent* ev = NULL;
	GetFirstActivity(nFrame);
	while (GetNextEvent(nFrame, ev)) {
		for (int i=0; i < ev->actors.GetCount(); i++) {
			Body* b;
			if (doc->bodymodel.body.Lookup(ev->actors[i], b))
				cvCircle(frame, cvPointFrom32f(b->GetImageCenter()), RADIUS, colorFromID(ev->id), CV_FILLED, CV_AA);
		}
	}
}
// --------------------------------------------------------------------------
void SwarmActivityModel::GetFirstEvent(int nFrame) {
	eventsExtracted = 0;
}
// --------------------------------------------------------------------------
bool SwarmActivityModel::GetNextEvent(int nFrame, SwarmEvent* &evOut) {
	int nFound = 0;
	POSITION posE = doc->m_ActivityData.sEvent.GetStartPosition();
	while (posE) {
		int id; SwarmEvent *ev;
		doc->m_ActivityData.sEvent.GetNextAssoc(posE, id, ev);
			if (ev->start <= nFrame && ev->stop >= nFrame) {
			nFound++;
			if (nFound > eventsExtracted) {
				evOut = ev;
				eventsExtracted++;
				return true;
			}
		}
	}
	return false;
}
// --------------------------------------------------------------------------
void SwarmActivityModel::GetFirstActivity(int nFrame) {
	activitiesExtracted = 0;
}
// --------------------------------------------------------------------------
bool SwarmActivityModel::GetNextActivity(int nFrame, SwarmActivity* &acOut) {
	int nFound = 0;
	POSITION posA = doc->m_ActivityData.sActivity.GetStartPosition();
	while (posA) {
		int id; SwarmActivity *ac;
		doc->m_ActivityData.sActivity.GetNextAssoc(posA, id, ac);
		if (ac->start <= nFrame && ac->stop >= nFrame) {
			nFound++;
			if (nFound > activitiesExtracted) {
				acOut = ac;
				activitiesExtracted++;
				return true;
			}
		}
	}
	return false;
}