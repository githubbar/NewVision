#include "StdAfx.h"
#include "BodyActivityModel.h"
#include "BodyPathCluster.h"
#include "BodyModel.h"
#include "NewVisionDoc.h"
#include "Body.h"
#include "Blob.h"
#include "CVML.h"


#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::Diagnostics;
using namespace System::IO;
using namespace System::Globalization;

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(BodyActivityModel, Model, 1)
// --------------------------------------------------------------------------
BodyActivityModel::BodyActivityModel(void) {
	m_initialized = false;
}
// --------------------------------------------------------------------------
BodyActivityModel::~BodyActivityModel(void) {
}
// --------------------------------------------------------------------------
bool BodyActivityModel::Initialize() {
	if (m_initialized)
		DeInitialize();
	m_initialized = true;
	return true;
}
// --------------------------------------------------------------------------
void BodyActivityModel::DeInitialize() {
	if (!m_initialized)
		return;
	m_initialized = false;
	Empty();
}
// --------------------------------------------------------------------------
void BodyActivityModel::UpdateSalesRepHist()  {
	POSITION pos = doc->bodymodel.body.GetStartPosition();
	while (pos) {
		int id; Body *b;
		doc->bodymodel.body.GetNextAssoc(pos, id, b);
		if (::In(m_salesRepID, id))
			b->HistogramCompute(imgFrame, m_salesRepHist.h, 1);	
	}
}
// --------------------------------------------------------------------------
void BodyActivityModel::DrawSalesReps(IplImage* frame, CvScalar color) {
	POSITION pos = doc->bodymodel.body.GetStartPosition();
	while (pos) {
		int id; Body *b;
		doc->bodymodel.body.GetNextAssoc(pos, id, b);
		double d = b->color.Compare(&m_salesRepHist);
		if (d > m_SalesRepHistThreshold)
			b->DrawSalesRepMark(frame, color);
		if (::In(m_salesRepID, id))
			b->DrawSalesRepTempMark(frame, color);
	}
}
// --------------------------------------------------------------------------
void BodyActivityModel::DrawDwellingBodies(IplImage* frame, CvScalar color) {
	//// for each body
	//for (int i=0;i<doc->bodymodel.body.GetCount();i++) {
	//	// find corresponding path
	//	BodyPath* p = NULL;
	//	for (int j=0;j<GetCount();j++) {
	//		if (GetAt(j)->id == doc->bodymodel.body[i]->id) {
	//			p = GetAt(j); break;
	//		}
	//	}
	//	// if the path is dwelling display a mark on the body
	//	if (p && p->IsDwelling((int)doc->trackermodel.m_frameNumber))
	//		doc->bodymodel.body[i]->DrawDwellMark(frame, color);
	//}
}
// --------------------------------------------------------------------------
void BodyActivityModel::DrawPath(IplImage* frame, CvScalar color) {
	//CvPoint2D32f floormin, floormax;
	//doc->floormodel.GetMinMaxXY(floormin, floormax);
	//for (int i=0;i<GetCount();i++) {
	//	GetAt(i)->Draw(frame, COLOR[i], doc, m_pathSmoothSigma);
	//}
}
// --------------------------------------------------------------------------
void BodyActivityModel::DetectEvents(int nFrame) {
	//for (int i = 0; i < doc->bodymodel.body.GetCount()+doc->bodymodel.body_deleted.GetCount(); i++) {
	//	Body *b = NULL;
	//	if (i < doc->bodymodel.body.GetCount())
	//		b = doc->bodymodel.body[i];
	//	else 
	//		b = doc->bodymodel.body_deleted[i-doc->bodymodel.body.GetCount()];

	//	BodyPath *bp = FindBodyMatch(b);
	//	if (bp) {
	//		bp->Add(cvPoint2D32f(b->bot.x, b->bot.y), b->GetHeight(), b->GetWidth(), b->orientation, b->stopFrame < nFrame ? false : true);
	//	}
	//	else {
	//		bp = new BodyPath(b->id, cvPoint2D32f(b->bot.x, b->bot.y), nFrame, b->GetHeight(), b->GetWidth(), b->orientation, b->stopFrame < nFrame ? false : true);
	//		Add(bp);
	//	}
	//	// if the path is dwelling display a mark on the body
	//	b->dwell = bp->ComputeDwellingState(nFrame, m_pathSmoothSigma, m_dwellTime, m_dwellAreaRadius);
	//	bp->GetAt(bp->GetCount()-1).dwell = b->dwell;
	//}
}
// --------------------------------------------------------------------------
void BodyActivityModel::ExportEvents(int nFrame) {
	//doc->bodymodel.body.RemoveAll();
	//for (int i=0;i<GetCount();i++) {
	//	BodyPath* p = GetAt(i);
	//	if (nFrame >= p->startFrame && nFrame < p->startFrame + p->GetCount()) {		// paths occurs at this frame
	//		Step s = p->GetAt(nFrame-GetAt(i)->startFrame);
	//		if (s.visible) {
	//			doc->bodymodel.body.Add(new Body(s.p, s.height, s.width, doc, p->id, p->startFrame));
	//			doc->bodymodel.body[doc->bodymodel.body.GetCount()-1]->orientation = s.orientation;
	//		}
	//	}
	//}
}
// --------------------------------------------------------------------------
void BodyActivityModel::ExportToCSV(CString fname) {
	//StreamWriter^ sr = File::CreateText(gcnew String(fname));
	//SetProgressDialogRange(0, 2*GetCount());
	//sr->WriteLine("TrackID, GroupID, GroupSize, StartDateTime, StopDateTime, Height, Width"); 
	//for (int i=0;i<GetCount();i++)  {
	//	SetProgressDialogPos(i);
	//	sr->Write("{0}, ", GetAt(i)->id);
	//	//BodyPathCluster* group = FindCluster(GetAt(i)->groupid);
	//	//if (group)
	//	//	sr->Write("{0}, {1}, ", __box(group->id), __box(group->path.GetCount()));
	//	//else
	//		sr->Write("N/A, N/A, ");
	//	String ^start = doc->trackermodel.GetDateTime(GetAt(i)->startFrame).ToString("G", DateTimeFormatInfo::InvariantInfo);
	//	String ^stop  = doc->trackermodel.GetDateTime(GetAt(i)->GetLastVisibleFrame()).ToString("G", DateTimeFormatInfo::InvariantInfo); 
	//	sr->Write("{0}, {1}, ", start, stop);
	//	sr->Write("{0}, {1}, ", GetAt(i)->GetHeight(), GetAt(i)->GetWidth()); 
	//	sr->WriteLine(); 
	//}
	//sr->WriteLine("TrackID, FixtureID, DwellStartTime, DwellStopTime"); 	
	//for (int i=0;i<GetCount();i++)  {
	//	SetProgressDialogPos(GetCount()+i);
	//	CArray<CvPoint2D32f> dwellLocation;
	//	CArray<int> dwellFrame;
	//	CArray<int> dwellDuration;
	//	GetAt(i)->GetDwellLocations(dwellLocation, dwellFrame, dwellDuration);
	//	
	//	/* || dwellFrame[dwellFrame.GetCount()-1] > GetAt(i)->GetLastVisibleFrame()*/
	//	// ignore deleted dwellers
	//	if (dwellFrame.GetCount() == 0)
	//		continue;

	//	for (int j=0;j<dwellLocation.GetCount();j++)  {
	//		Object3D *obst = doc->obstaclemodel.GetClosestObstacle(dwellLocation[j]);
	//		String^ label = gcnew String(obst ? obst->label : "N/A");
	//		sr->Write("{0}, {1}, ", GetAt(i)->id, label);
	//		String ^start = doc->trackermodel.GetDateTime(dwellFrame[j]-m_dwellTime).ToString("G", DateTimeFormatInfo::InvariantInfo);
	//		String ^stop= doc->trackermodel.GetDateTime(dwellFrame[j]+dwellDuration[j]).ToString("G", DateTimeFormatInfo::InvariantInfo);
	//		sr->Write("{0}, {1}, ", start, stop);
	//		sr->WriteLine(); 
	//	}
	//}
 //   sr->Close();
}
// --------------------------------------------------------------------------
void BodyActivityModel::Empty() {
	//this->RemoveAll();
}

