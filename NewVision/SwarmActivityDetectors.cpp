#include "StdAfx.h"
#include "SwarmActivityDetectors.h"
#include "NewVisionDoc.h"
#include "BodyPath.h"
#include "BodyCluster.h"
#include "SwarmEvent.h"
#include "SwarmActivity.h"
#include "Node2DPoint.h"
#include "Cluster2DPoint.h"
#using <mscorlib.dll>
#using <System.dll>

using namespace System;
using namespace System::Diagnostics;
using namespace System::IO;

// ============================================================================================================
BEGIN_MESSAGE_MAP(ProximityEventDetector, CTabPageSSL)
END_MESSAGE_MAP()
// ============================================================================================================
int ProximityEventDetector::DetectEvents(int nFrame, CNewVisionDoc* doc) {
	//double min_val=0, max_val=0;
	//CvPoint min_loc, max_loc;
	//CArray<BodyPathCluster*> group;
	//for (int i=0;i<output.GetCount();i++)
	//	group.Add(new BodyPathCluster(doc->bodyactivitymodel.GetAt(i)));

	//if (group.GetCount() == 0)
	//	return 0;

	//do {
	//	CvMat* d = cvCreateMat(group.GetCount(), group.GetCount(), CV_64FC1); 
	//	GetClusterDistances(group, d);
	//	cvMinMaxLoc(d, &min_val, &max_val, &min_loc, &max_loc);
	//	// merge
	//	if (min_val < m_distanceThreshold) {
	//		group[min_loc.x]->AddCluster(group[min_loc.y]);
	//		group.RemoveAt(min_loc.y);
	//	}
	//	cvReleaseMat(&d);
	//} while (min_val < m_distanceThreshold);
	//// convert to doc->bodymodel.group
	return 0; //number of events of this type detected
}
// --------------------------------------------------------------------------
void ProximityEventDetector::DoDataExchange (CDataExchange* pDX) {
	CTabPageSSL::DoDataExchange (pDX);
	DDX_Text(pDX, IDC_PROXIMITYDISTANCETHRESHOLD, m_distanceThreshold);
	DDX_Text(pDX, IDC_PROXIMITYMATUREPATH, m_maturePath);
	DDX_Text(pDX, IDC_PROXIMITYTIMEDELTA, m_timeDelta);
	DDX_Text(pDX, IDC_PROXIMITYEVERYNFRAME, m_everyNframe);
}
// --------------------------------------------------------------------------
BOOL ProximityEventDetector::OnInitDialog () {
	CTabPageSSL::OnInitDialog ();
	m_distanceThreshold = 300;
	m_maturePath = 15;
	m_timeDelta = 30;
	m_everyNframe = 5;
	UpdateData(0);
	return TRUE; 
}
// --------------------------------------------------------------------------
void ProximityEventDetector::Serialize( CArchive& ar ) {
	SwarmEventDetector::Serialize(ar);
	if( ar.IsStoring())
		ar << m_distanceThreshold << m_maturePath << m_timeDelta; 
	else
		ar >> m_distanceThreshold >> m_maturePath >> m_timeDelta; 
}
// ============================================================================================================
BEGIN_MESSAGE_MAP(GroupingEventDetector, CTabPageSSL)
END_MESSAGE_MAP()
// --------------------------------------------------------------------------
GroupingEventDetector::GroupingEventDetector(CString name, int idd) {
	this->name = name; IDD = idd;
	m_dwellWeight = 0.3;             
	m_alignmentWeight = 0.1;         
	m_cohesionWeight = 0.6;
	m_maxGroupDiameter = 200;
	m_everyNframe = 30;
	m_EventEndBeforeFrame = 3000;
	m_minEventWeight = 0.01;
	m_minActorWeight = 0.2;

	// Initialize clustering parameters
	param.maxIterations				= 15;	// maximum number of iterations
	param.maxClusters				= 20;	// initial number of clusters
	param.minNodeCardinality		= 20;	// minimum number of times the events with the same actors repeats (used while creating initial clusters)
	param.maxObjectiveFunctionDelta = 1;	// used as a stop criteria (convergence
	param.minNodeDistance			= 0.1;	// a threshold to group SwarmEvents to form initial clusters
	param.maxNodeDistance			= 0.7;	// a threshold to remove SwarmEvents that are too far away from any other events (i.e outliers)
	param.eta0						= 0.01;	// multiplier: controls the speed of clustering
	param.minCardinality			= 0.3;	// minimum cardinality to keep cluster alive
	param.tau						= 10;	// exponent: controls the speed of clustering
	param.nIteration0				= 5;	// after this iteration clustering will slow down
	param.cTuningMin				= 4;	// a tuning constant
	param.cTuning0					= 12;	// a tuning constant
	param.cTuningDelta				= 1;	// a tuning constant
}
// --------------------------------------------------------------------------
void GroupingEventDetector::DoDataExchange (CDataExchange* pDX) {
	CTabPageSSL::DoDataExchange (pDX);
	DDX_Text(pDX, IDC_GROUPINGDWELLW,m_dwellWeight);
	DDX_Text(pDX, IDC_GROUPINGALIGNMENTW,m_alignmentWeight);
	DDX_Text(pDX, IDC_GROUPINGCOHESIONW,m_cohesionWeight);
	DDX_Text(pDX, IDC_GROUPINGGROUPRADIUS,m_maxGroupDiameter);
	DDX_Text(pDX, IDC_GROUPINGEVERYNFRAME, m_everyNframe);
	DDX_Text(pDX, IDC_GROUPINGEVENTSBEFOREFAME, m_EventEndBeforeFrame);
	DDX_Text(pDX, IDC_GROUPINGMINEVENTWEIGHT, m_minEventWeight);
	DDX_Text(pDX, IDC_GROUPINGMINACTORWEIGHT, m_minActorWeight);
	DDX_Text(pDX, IDC_GROUPINGMINNODECARDINALITY, param.minNodeCardinality);
	DDX_Text(pDX, IDC_GROUPINGMAXOBJECTIVEFUNCTIONDELTA, param.maxObjectiveFunctionDelta);
	DDX_Text(pDX, IDC_GROUPINGMAXITERATIONS, param.maxIterations);
	DDX_Text(pDX, IDC_GROUPINGMAXCLUSTERS, param.maxClusters);
	DDX_Text(pDX, IDC_GROUPINGITERATION0, param.nIteration0);
	DDX_Text(pDX, IDC_GROUPINMINCARDINALITY, param.minCardinality);
	DDX_Text(pDX, IDC_GROUPINAGGLOMERATIONSPEED, param.eta0);


	//param.minNodeDistance	= 0.1;	// a threshold to group SwarmEvents to form initial clusters
	//param.maxNodeDistance	= 0.7;	// a threshold to remove SwarmEvents that are too far away from any other events (i.e outliers)
	//param.minCardinality	= 0.3;	// minimum cardinality to keep cluster alive
	//param.tau				= 10;	// exponent: controls the speed of clustering
	//param.cTuningMin		= 4;	// a tuning constant
	//param.cTuning0			= 12;	// a tuning constant
	//param.cTuningDelta		= 1;	// a tuning constant
	DDV_MinMaxInt(pDX, m_everyNframe, 1, 10000);
	DDV_MinMaxDouble(pDX, param.minCardinality, 0, 1);
}
// --------------------------------------------------------------------------
BOOL GroupingEventDetector::OnInitDialog () {
	CTabPageSSL::OnInitDialog ();
	UpdateData(0);
	return TRUE; 
}
// --------------------------------------------------------------------------
double GroupingEventDetector::ComputeDistance(BodyClusterElement* bi, BodyClusterElement* bj) {
	double d1 = m_dwellWeight*(abs(bi->dwell - bj->dwell));
	double d2 = m_alignmentWeight * d_angle(bi->orientation, bj->orientation)/(2*PI);
	double d3 = m_cohesionWeight*d(bi->bot, bj->bot)/(m_maxGroupDiameter);
	double dist = d1 + d2 + d3;
	return dist;
}

// --------------------------------------------------------------------------
double GroupingEventDetector::ComputeSpread(BodyCluster* bi) {
	double dist = 0;
	if (bi->sub.GetCount() <= 1)
		return 0;
	for (int i=0;i<bi->sub.GetCount();i++)
		dist += ComputeDistance(bi, bi->sub[i]);
	return dist/bi->sub.GetCount();
}
// --------------------------------------------------------------------------
int FindNeighbors(CNewVisionDoc* doc, BodyCluster* all, SmartPtrArray<BodyClusterElement>& dst, CvMat* D_elements, int n, int N=1) {
		double min_val, max_val;
		CvPoint min_loc, max_loc;
		CvMat* D_cpy = (CvMat*)cvClone(D_elements);
		CvMat col; cvGetCol(D_cpy, &col, n);
		int i;
		for (i=0; i<N; i++) { // find N nearest neighbors
			cvMinMaxLoc(&col, &min_val, &max_val, &min_loc, &max_loc); // find first closest 
			if (min_val == FLT_MAX)
				break;
			//doc->Message(String::Format("i= {0:d1} x={1:d1} y={2:d1} dst={3:f3}  ", i, all->sub[n]->id, all->sub[min_loc.y]->id, min_val));
			dst.Add(all->sub[min_loc.y]);  
			cvSetReal2D(&col, min_loc.y, 0, FLT_MAX);
		}
		cvReleaseMat(&D_cpy);
		return i;
}
double isolation, compactness;
// --------------------------------------------------------------------------
double GroupingEventDetector::ComputeGroupClusteringValidity(CNewVisionDoc* doc, CArray<BodyCluster*>& cl, BodyCluster* all, CvMat* D_elements) {
	double all_spread = ComputeSpread(all);

	// ==== Validity index = isolation index + compactness index
	
	// Isolation - how isolated the clusters are
	isolation = 0;
	const int N=2;

	if (all->sub.GetCount() == 1)
		isolation = 1;

	double norm_i = 0;
	
	for (int i=0;i<all->sub.GetCount();i++) { // for each node
		SmartPtrArray<BodyClusterElement> nearest;
		int n_found = FindNeighbors(doc, all, nearest, D_elements, i, N);
		int cl_found = -1;
		// get cluster id for target body
		for (int i_cl=0;i_cl<cl.GetCount();i_cl++)
			if (cl[i_cl]->In(all->sub[i]->id)) {
				cl_found = i_cl;
				break;
			}
		
		// compare to cluster ids of nearest bodies
		int n_matches = 0;
		for (int i_near=0;i_near<n_found;i_near++) {
			int cl_same = 0;				
			for (int i_cl=0;i_cl<cl.GetCount();i_cl++)
				if (cl[i_cl]->In(nearest[i_near]->id)) {
					if (cl[cl_found]->id == cl[i_cl]->id)
						n_matches++;
				}
		}
		isolation += 1.0*n_matches/n_found;
	}
	isolation /= all->sub.GetCount();

	// Compactness - how compact the clusters are ( 1 - mean(Diameter_cluster)/Diameter_all )
	compactness = 0;
	double norm_c = 0;
	for (int i=0;i<cl.GetCount();i++)
		if (cl[i]->sub.GetCount()) {
			compactness += ComputeSpread(cl[i]);
			norm_c ++; 
		}
	
	compactness = 1 - compactness /(norm_c * all_spread);
	//doc->Message(String::Format("i={0:f3} c={1:f3} v={2:f3}  ", isolation, compactness, isolation+compactness));
	double wCompactness = 5;
	return isolation+wCompactness*compactness;
}
// --------------------------------------------------------------------------
int GroupingEventDetector::DetectEvents(int nFrame, CNewVisionDoc* doc) {
	/*
	The fitness of body Bi in group Gj can be determined as 
	f(Bi, Gj) = W_dwell      * f_dwell +
				W_alignment  * f_alignment  +
				W_cohesion   * f_cohesion
				  
	Where f_* are functions that compute (	Co-dwelling: dwell near the same or nearby fixture as swarm-mates
											Alignment: align towards the average heading of the local swarm-mates
											Cohesion: position itself close to the average position of local swarm-mates )
	and W_* are respective weights

	f_dwell(Bi, Gj)     = || Bi_dwell_fixture_xy && mean(Gj_dwell_fixture_xy) ||
	f_alignment(Bi, Gj) = (Bi_velocity, mean(Gj_velocity))
	f_cohesion(Bi, Gj)  = 1 / || Bi_xy, mean(Gj_xy) ||
	*/

	if (nFrame % m_everyNframe)
		return -1;

	// --------- At each frame perfom a mapping of Bi (bodies) to Gi (groups)------------

	// Create a temporary queue of recent events
	CList<SwarmEvent*, SwarmEvent*> recentEvents;

	SmartPtrArray<BodyCluster> cl;
	SmartPtrArray<BodyCluster> cl_copy;
	BodyCluster all;	
	for (POSITION pos = doc->m_TrackingData.data.GetStartPosition();pos;) {
		int id; BodyPath *bp;
		doc->m_TrackingData.data.GetNextAssoc(pos, id, bp);
		if (nFrame < bp->startFrame || nFrame > bp->startFrame+bp->GetUpperBound())
			continue;
		Step s = bp->GetAt(nFrame-bp->startFrame);
		if (!s.visible)
			continue;
		bool dwell = bp->ComputeDwellingState(nFrame,
			doc->bodyactivitymodel.m_pathSmoothSigma, 
			doc->bodyactivitymodel.m_dwellTime, 
			doc->bodyactivitymodel.m_dwellAreaRadius);
		double orientation = bp->GetMotionAngle(nFrame, doc->bodyactivitymodel.m_pathSmoothSigma);
		BodyClusterElement *bce = new BodyClusterElement(id, cvPoint3D32f(s.p.x, s.p.y, 0), s.orientation, dwell);
		cl.Add(new BodyCluster(bce));
		all.Add(bce);
	}

	// (1.0) Prepare a set of N singleton clusters and NxN distance matrix 
	int N = cl.GetCount();
	if (N < 2)
		return 0;
	CvMat* D = cvCreateMat(N, N, CV_64FC1); // lower-left diagonal matrix
	cvSet(D, cvScalar(FLT_MAX));
	for (int i = 0;i < N;i++)
		for (int j = 0;j < i;j++) {
			double d = ComputeDistance(cl[i], cl[j]);
			cvSetReal2D(D, i, j, d);
			cvSetReal2D(D, j, i, d);
		}
	CvMat* D_elements = (CvMat*)cvClone(D);

	// (2.0) Perform agglomerative clustering 
	double kValidityNow = ComputeGroupClusteringValidity(doc, cl, &all, D_elements); // to determine the optimal number of clusters compute clustering validity index
	double kValidityBefore = 0; 
	int step=0;
	String ^msg = gcnew String("");
	do {
			double min_val, max_val;
			CvPoint min_loc, max_loc;
			cl_copy.Copy(cl);
			cvMinMaxLoc(D, &min_val, &max_val, &min_loc, &max_loc);
			if (min_val == FLT_MAX)
				break;
			BodyCluster *b1 = cl[min_loc.x], *b2= cl[min_loc.y];
			// break if no more close bodies
			if (d(b1->bot, b2->bot) > m_maxGroupDiameter )
				break;
			
			// merge
			//doc->Message(String::Concat("Add ", b2->id, " at [", b2->bot.x, ", ", b2->bot.y, "] to ", b1->id, " at [", b1->bot.x, ", ", b1->bot.y, "]"));
			b1->Add(b2);
			//doc->Message(String::Concat("Result at [", b1->bot.x, ", ", b1->bot.y, "] "));
			b2->sub.RemoveAllPointers();

			// change row and col 'min_loc.x'
			for (int i=0;i<N;i++) {
				if (i == min_loc.x)
					continue;
				double dist = ComputeDistance(cl[i], b1);
				cvSetReal2D(D, i, min_loc.x, dist);
				cvSetReal2D(D, min_loc.x, i, dist);
			}

			// erase row and col 'min_loc.y'
			for (int i=0;i<N;i++) {
				cvSetReal2D(D, i, min_loc.y, FLT_MAX);
				cvSetReal2D(D, min_loc.y, i, FLT_MAX);
			}
			msg += String::Format("{0:f2}:{1:f2}  ", isolation, compactness);

			kValidityBefore = kValidityNow;
			kValidityNow = ComputeGroupClusteringValidity(doc, cl, &all, D_elements);
			
			step++;
		} while (kValidityNow > kValidityBefore);
		msg += String::Format("{0:f2}:{1:f2}  ", isolation, compactness);
		//doc->Message(msg);
		
		//doc->Message(String::Format("step={3:d1} {0:f2}+{1:f2}={2:f2}  ", isolation, compactness, kValidityNow, step));




	// (3.0) populate group events array
	int EVENT_FRAME_CONFIDENCE = 100;
	double MAX_EVENT_DISTANCE_TO_MERGE = 0.01; // if some actors are different the distance will usually be > 0.02
	int count = cl_copy.GetCount();
	for (int i=0;i<count;i++) {
		if (cl_copy[i]->sub.GetCount() < 2)
			continue;
		SwarmEvent* ev1 = new SwarmEvent(nFrame, name);
		for (int k=0;k<cl_copy[i]->sub.GetCount();k++)
			ev1->AddActor(cl_copy[i]->sub[k]->id, kValidityNow);

		//// Update a temporary queue of recent events: find the closest existing event with all actors matching, then merge
		//double maxDistance = FLT_MAX; int eid = -1;
		//SwarmEvent* ev2 = NULL, *evMatch = NULL;
		//for (POSITION posE = recentEvents.GetHeadPosition();posE;)
		//{
		//	 ev2 = recentEvents.GetNext(posE);
		//	 // do not merge if too far apart in time (treat as separate events)
		//	 if (ev1->GetAverageFrameDistance(ev2) > EVENT_FRAME_CONFIDENCE)
		//		 break;
		//	double d = ev1->DistanceSQR(ev2);
		//	if (d < maxDistance ) {  // ev2 is the closest to ev1 
		//		maxDistance = d;
		//		evMatch = ev2;
		//	}
		//	//echo(String::Format("dist {0}",ev1->GetAverageFrameDistance(ev2)));
		//}

		//// if found an almost identical event match - merge
		//if (evMatch && maxDistance < MAX_EVENT_DISTANCE_TO_MERGE) {
		//	ev2->Add(ev1);
		//	SwarmEvent::last_id--;
		//}
		//// otherwise, create a new event map entry
		//else
			recentEvents.AddHead(ev1);
	}

	// (4.0) Move events from temporary queue to the events map
	for (POSITION posE = recentEvents.GetHeadPosition();posE;)
	{
		SwarmEvent *ev = recentEvents.GetNext(posE);
		doc->m_ActivityData.sEvent.SetAt(ev->id, ev);
	}
	// (5.0) Cleanup
	cvReleaseMat(&D);
	cvReleaseMat(&D_elements);
	recentEvents.RemoveAll();
	cl.RemoveAll();
	cl_copy.RemoveAll();
	return count;
}
// -------------------------------------------------------------------------
// Detects activities in on step after all events are collected 
// -------------------------------------------------------------------------
int GroupingEventDetector::PostProcessActivities(CNewVisionDoc* doc) {
	CArray<INode*> node;
	CArray<IFuzzyCluster*> cluster;
	doc->m_ActivityData.sActivity.RemoveAll();
	SwarmActivity::last_id = 0;
	// TEMP: WRITE OUT ALL DISTANCES
	//System::IO::StreamWriter^ log = File::CreateText("F:\\My Documents\\My Research\\Results\\Data\\Activity Data\\distances.csv");
	//log->Write("ID, ");
	//for (POSITION posE = doc->m_ActivityData.sEvent.GetStartPosition(); posE != NULL;) {
	//	int id; SwarmEvent *ev;
	//	doc->m_ActivityData.sEvent.GetNextAssoc(posE, id, ev);
	//	if (m_EventEndBeforeFrame == -1 || ev->stop < m_EventEndBeforeFrame)
	//		log->Write(String::Format("{0}, ", id));
	//}
	//log->WriteLine();
	//for (POSITION posE = doc->m_ActivityData.sEvent.GetStartPosition(); posE != NULL;) {
	//	int id; SwarmEvent *ev;
	//	doc->m_ActivityData.sEvent.GetNextAssoc(posE, id, ev);
	//	if (m_EventEndBeforeFrame == -1 || ev->stop < m_EventEndBeforeFrame) {
	//		//log->Write(String::Format("{0}, ", id));
	//		for (POSITION posE2 = doc->m_ActivityData.sEvent.GetStartPosition(); posE2 != NULL;) {
	//			int id2; SwarmEvent *ev2;
	//			doc->m_ActivityData.sEvent.GetNextAssoc(posE2, id2, ev2);
	//			if (m_EventEndBeforeFrame == -1 || ev2->stop < m_EventEndBeforeFrame)
	//				log->Write(String::Format("{0:f4}, ", ev->DistanceSQR(ev2)));
	//		}
	//		log->WriteLine();
	//	}
	//}
	//log->Close();
	//return 0;
	//// ------------------- TEMP: TEST WITH 2D POINTS

	////// Uniform points
	////for (int i=0;i<100;i++) {
	////	node.Add(new Node2DPoint());
	////}

	//rnd_seed = cvRNG(1);
	//Cluster2DPoint::last_id = 0;
	//IFuzzyCluster::last_id = 0;
	//Node2DPoint::last_id = 0;
	//// Several clusters with noise
	//for (int c=0;c<8;c++) {
	//	double x1 = 20*cvRandReal(&rnd_seed), y1 = 20*cvRandReal(&rnd_seed);
	//	int N = (int)(10+0*cvRandReal(&rnd_seed));
	//	//int N = 10;
	//	for (int i=0;i<N;i++) {
	//		Node2DPoint *p1 = new Node2DPoint(x1+cvRandReal(&rnd_seed), y1+cvRandReal(&rnd_seed));
	//		p1->id = c*N+i;
	//		node.Add(p1);
	//	}
	//}

	//ClusteringMachine cm(param);
	//cm.RunFuzzy(node, cluster);

	//return 0;
	//// -------------------- END TEMP

	for (POSITION posE = doc->m_ActivityData.sEvent.GetStartPosition(); posE != NULL;) {
		int id; SwarmEvent *ev;
		doc->m_ActivityData.sEvent.GetNextAssoc(posE, id, ev);
		//TEMP
		//if (ev->id == 1 || ev->id == 2 || ev->id == 6 || ev->id == 7 || ev->id == 8)
		if (m_EventEndBeforeFrame == -1 || ev->stop < m_EventEndBeforeFrame)
			node.Add(ev);
	}

	ClusteringMachine cm(param);
	cm.RunFuzzy(node, cluster);

	// export to activity map
	for (int i=0;i<cluster.GetCount();i++) {
		((SwarmActivity*)(cluster[i]))->actors.RemoveAll();
		((SwarmActivity*)(cluster[i]))->start = INT_MAX;
		((SwarmActivity*)(cluster[i]))->stop = INT_MIN;
		for (POSITION pos = cluster[i]->rec.GetStartPosition(); pos != NULL;) {
			int nid; Record* r;
			cluster[i]->rec.GetNextAssoc(pos, nid, r);
			// for each significant event in the cluster
			if (r->u > m_minEventWeight) {
				// copy all actors from significant events
				for (POSITION posA = ((SwarmEvent*)r->node)->actors.GetStartPosition(); posA != NULL;) {
					int aid; double p;
					((SwarmEvent*)r->node)->actors.GetNextAssoc(posA, aid, p);
					p = 0;
					((SwarmActivity*)(cluster[i]))->actors.Lookup(aid, p);
					// accumulate weight from all events
					((SwarmActivity*)(cluster[i]))->actors.SetAt(aid, p+r->u*pow(r->psi,2));
					((SwarmActivity*)(cluster[i]))->events.SetAt(r->node->id, 1);
				}
			}
		}
		//// normalize actor weights
		//((SwarmActivity*)(cluster[i]))->Normalize();
		//// remove insignificant actors
		//((SwarmActivity*)(cluster[i]))->FilterActors(m_minActorWeight);
		//// normalize actor weights again
		//((SwarmActivity*)(cluster[i]))->Normalize();

		// populate swarming activity data
		if (((SwarmActivity*)(cluster[i]))->actors.GetCount()) {
			SwarmActivity *cl = (SwarmActivity*)cluster[i];
			//cl->p = cl->N;
			doc->m_ActivityData.sActivity.SetAt(cluster[i]->id, cl);
		}
	}

	return 0;
}
// --------------------------------------------------------------------------
int GroupingEventDetector::DetectActivities(int nFrame, CNewVisionDoc* doc) {
	if (nFrame == -1)
		nFrame = doc->trackermodel.m_frameNumber;

	if (nFrame % m_everyNframe)
		return -1;

	return 0;
	//echo(String::Format("---------------------------- Frame = {0} ---------------------------", nFrame));
	// for each event
	//for (int ei=0; ei < ev.GetCount(); ei++) {
	//	for (int ai=0; ai < ac.GetCount(); ai++) {
	//		POSITION pos = ev[ei]->actors.GetStartPosition(); 
	//		while (pos) {
	//			int id; double c;
	//			ev[ei]->actors.GetNextAssoc(pos, id, c);
	//			double c;
	//			match = true;
	//			}
	//			else {
	//				double c  = ac[ai]->Compare(ev[ei]);
	//				if (c > m_actT1) {
	//					ac[ai]->Add(ev[ei], c);
	//					match = true;
	//				}
	//			}
	//		}
	//		if (!match)
	//			ac.Add(new SwarmActivity(ev[ei]));
	//	}
	//}
	//return ac.GetCount();

	////echo(String::Format("---------------------------- Frame = {0} ---------------------------", nFrame));
	//// for each event
	//for (int ei=0; ei < ev.GetCount(); ei++) {
	//	// skip event, if it is not current
	//	if (!ev[ei]->OccurredAt(nFrame))
	//		continue;
	//	// no activities? create the first one!
	//	if (ac.GetCount() == 0) {
	//		ac.Add(new SwarmActivity(ev[ei]));
	//		continue;
	//	}

	//	// otherwise add to all matching activities
	//	bool match = false;
	//	for (int ai=0; ai < ac.GetCount(); ai++) {
	//		// has this event been classified already? if yes, add to known activity
	//		if (ac[ai]->LookupEvent(ev[ei])) {
	//			ac[ai]->Add(ev[ei], 1);
	//			match = true;
	//		}
	//		else {
	//			double c  = ac[ai]->DistanceSQR(ev[ei]);
	//			if (c > m_actT1) {
	//				ac[ai]->Add(ev[ei], c);
	//				match = true;
	//			}
	//		}
	//	}
	//	if (!match)
	//		ac.Add(new SwarmActivity(ev[ei]));
	//}
	//return ac.GetCount();
}
// --------------------------------------------------------------------------
void GroupingEventDetector::Serialize( CArchive& ar ) {
	SwarmEventDetector::Serialize(ar);
	if( ar.IsStoring()) {
		ar << m_dwellWeight << m_alignmentWeight << m_cohesionWeight << m_maxGroupDiameter;
	
		ar << m_EventEndBeforeFrame << m_minEventWeight << m_minActorWeight;
		ar << param.maxIterations << param.maxClusters << param.minNodeDistance	<< param.maxNodeDistance	
			<< param.minCardinality	<< param.eta0 << param.tau	<< param.nIteration0 <<	param.cTuningMin 
			<< param.cTuning0 << param.cTuningDelta << param.minCardinality << param.eta0
			<< param.minNodeCardinality << param.maxObjectiveFunctionDelta; 
	}
	else {
		int temp;
		ar >> m_dwellWeight >> m_alignmentWeight >> m_cohesionWeight >> m_maxGroupDiameter;
		ar >> m_EventEndBeforeFrame >> m_minEventWeight >> m_minActorWeight;
		ar >> param.maxIterations >> param.maxClusters >> param.minNodeDistance	>> param.maxNodeDistance	
			>> param.minCardinality	>> param.eta0 >> param.tau	>> param.nIteration0 >>	param.cTuningMin 
			>> param.cTuning0 >> param.cTuningDelta >> param.minCardinality >> param.eta0
			>> param.minNodeCardinality >> param.maxObjectiveFunctionDelta; 
	}
}
// ============================================================================================================
BEGIN_MESSAGE_MAP(LeaderFollowingEventDetector, CTabPageSSL)
END_MESSAGE_MAP()
// --------------------------------------------------------------------------
int LeaderFollowingEventDetector::DetectEvents(int nFrame, CNewVisionDoc* doc) {
	return 0;
}
// ============================================================================================================
BEGIN_MESSAGE_MAP(LiningUpEventDetector, CTabPageSSL)
END_MESSAGE_MAP()
// --------------------------------------------------------------------------
int LiningUpEventDetector::DetectEvents(int nFrame, CNewVisionDoc* doc) {
	return 0;
}
// ============================================================================================================
BEGIN_MESSAGE_MAP(CoDwellingEventDetector, CTabPageSSL)
END_MESSAGE_MAP()
// --------------------------------------------------------------------------
int CoDwellingEventDetector::DetectEvents(int nFrame, CNewVisionDoc* doc) {
	return 0;
}