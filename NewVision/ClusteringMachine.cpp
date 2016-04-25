#include "StdAfx.h"
#include "ClusteringMachine.h"
#include "cv.h"
#include "cxcore.h"
#include "SwarmActivity.h"
#include "SwarmEvent.h"
#include "Node2DPoint.h"
#include "Cluster2DPoint.h"
#include "Vexel.h"
#include <typeinfo.h>
#include <algorithm>
using namespace System;
using namespace System::Diagnostics;
using namespace System::IO;

double CLUSTERING_DIST_MAX = 1;
double CLUSTERING_DIST_MIN = 1e-4;

gcroot<System::IO::StreamWriter^> logMean;
int IFuzzyCluster::last_id = 0;
IMPLEMENT_DYNAMIC( INode, CObject)
// --------------------------------------------------------------------------
ClusteringMachine::ClusteringMachine(ClusteringParams &param) {
	isRunning = false;
	this->param = param;
}

// --------------------------------------------------------------------------
double ClusteringMachine::GetObjective(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster) {
	double goal_RR = 0, goal_BI = 0;
	for (int ic = 0; ic < cluster.GetCount(); ic++) {
		for (int i = 0; i < node.GetCount(); i++) {	
			Record* r;
			cluster[ic]->rec.Lookup(node[i]->id, r);
			goal_RR += r->u*r->u*r->rho;
		}
		goal_BI += alpha*cluster[ic]->N*cluster[ic]->N;
	}
	return goal_RR-goal_BI;
}
// --------------------------------------------------------------------------
void ClusteringMachine::printD(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster) {
	// TEMP: DEBUG OUTPUT
	log->WriteLine(String::Format("iter={0}, alpha={1:f4}, clusters={2}", nIteration, alpha, C));
	//log->Write("nodeID, ");
	//for (int ic = 0; ic < cluster.GetCount(); ic++)
	//	log->Write(String::Format("{0}, ", cluster[ic]->id));
	//log->WriteLine("");
	double goal_old  = 0, goal_a = 0;
	goal = 0;
	//double sumU = 0;
	//log->Write(String::Format("{0} ,  ", node[i]->id));
	for (int ic = 0; ic < cluster.GetCount(); ic++) {
		log->Write(String::Format("{0:d} -- {1:f2} -- {2:f2} -- {3:f2},  ", cluster[ic]->id, cluster[ic]->N, cluster[ic]->T, cluster[ic]->S));
		for (int i = 0; i < node.GetCount(); i++) {	
			Record* r;
			cluster[ic]->rec.Lookup(node[i]->id, r);
			r->crispIn ? log->Write("in ") : log->Write("out ");
			log->Write(String::Format("{0:f2} : {1:f2} : {2:f2}  : {3:f2} : {4:f2} : {5:f2}, ", r->u, r->u_RR, r->u_Bias, r->d2, r->rho, r->psi));
			goal_old += r->u*r->u*r->d2;
			goal += r->u*r->u*r->rho;
			//log->Write(String::Format(" ,  "));
		}
		goal_a += alpha*cluster[ic]->N*cluster[ic]->N;
		log->WriteLine("");
	}
	log->WriteLine();
	log->WriteLine("GOAL = {0:f2} OLD = {1:f2}",goal-goal_a, goal_old);
	log->WriteLine("--------------------------------------------------");
	// END TEMP: DEBUG OUTPUT
}
// --------------------------------------------------------------------------
void ClusteringMachine::RunFuzzy(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster) {
	log = File::CreateText("F:\\My Documents\\My Research\\Results\\Data\\Activity Data\\ClusteringMachineLog.csv");
	System::IO::StreamWriter^ logN = File::CreateText("F:\\My Documents\\My Research\\Results\\Data\\Activity Data\\dataN.csv");
	System::IO::StreamWriter^ logCnumber = File::CreateText("F:\\My Documents\\My Research\\Results\\Data\\Activity Data\\dataCnumber.csv");
	for (int ic=0;ic<node.GetCount();ic++) {
		logN->WriteLine(node[ic]->ToString());
	}
	logN->Close();
	System::IO::StreamWriter^ logC = File::CreateText("F:\\My Documents\\My Research\\Results\\Data\\Activity Data\\dataC.csv");


	//// TEMP
	//double T = 2, S = 0.3, K = 5, cTuning = 10, rho;
	//for (double d2 = 0; d2 < 10; d2 += 0.1) {
	//	if (T == 0 || S == 0)
	//		rho = CLUSTERING_DIST_MIN;
	//	else {
	//		if (d2 <= T) 
	//			rho = d2 - ( d2*d2*d2 / (6*T*T) );
	//		else if (d2 > T+cTuning*S) 
	//			rho = (5*T+cTuning*S)/6+K;
	//		else
	//			rho = pow(d2-(T+cTuning*S), 3)/(6*cTuning*cTuning*S*S) + (5*T+cTuning*S)/6;
	//	}
	//	rho = max(rho, CLUSTERING_DIST_MIN);
	//	log->WriteLine(rho);
	//}
	//log->Close();
	//return;
	///// END TEMP
	// (1) If no events, return
	if (!node.GetCount()) {
		log->Close();
		return;
	}

	// (2) Create an initial set of clusters
	if (!param.silent)
		ShowProgressDialog("Initializing Clusters ...",4);
	InitializeClusters(node, cluster);

	if (!param.silent)
		HideProgressDialog();
	printD(node, cluster);	
	// (3) Run fuzzy agglomerative clustering
	double objective = DBL_MAX;
	double objectivePrev = 0;
	if (!param.silent)
		ShowProgressDialog("Clustering ...",param.maxIterations);
	// Begin Output CSV data
	for (int ic=0;ic<cluster.GetCount();ic++) {
		logC->WriteLine(cluster[ic]->ToString());
	}
	logCnumber->Write(String::Format("{0}, ", C));
	// End Output CSV data


	CArray<double> aGoal;

	// TEMP
	nIteration = param.maxIterations;
	// END TEMP
	while (C > 0 && nIteration < param.maxIterations /*&& fabs(objective - objectivePrev) > param.maxObjectiveFunctionDelta*/) {
		objectivePrev = objective;
		// (3.1) Update distance matrix
		UpdateDistanceMatrix(node, cluster);

		// (3.1.1) Remove empty clusters
		RemoveEmptyClusters(node, cluster);

		// (3.2) Update medians
		UpdateClusterMedians(node, cluster);

		// (3.3) Update robust estimators
		UpdateEstimators(node, cluster);

		// (3.4) Update stabilizing coefficient alpha
		UpdateAlpha(node, cluster);
		// (3.5) Update cluster partition matrix
		UpdatePartitionMatrix(node, cluster);

		// (3.6) Discard clusters with N < minCardinality
		RemoveEmptyClusters(node, cluster);
		DiscardWeakClusters(node, cluster);

		cTuning = max(param.cTuningMin, cTuning-param.cTuningDelta);
		objective = GetObjective(node, cluster);
		// Begin Output CSV data
		for (int ic=0;ic<cluster.GetCount();ic++) {
			logC->WriteLine(cluster[ic]->ToString());
		}
		logCnumber->Write(String::Format("{0}, ",C));
		// End Output CSV data
		nIteration++;
	
		printD(node, cluster);
		aGoal.Add(goal);
		if (!param.silent)
			StepProgressDialog((CString)String::Format("iteration = {0}", nIteration));
		
	}
	if (!param.silent)
		HideProgressDialog();
	for (int i=0;i<aGoal.GetCount();i++)
		log->WriteLine(aGoal[i]);
	// (4) Cleanup

	log->Close();
	logC->Close();
	logCnumber->Close();

	isRunning = false;
}
// --------------------------------------------------------------------------
void ClusteringMachine::InitializeClusters(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster) {
	// form initialize parameters 
	C = Cmax = param.maxClusters;
	nIteration = 0; eta = 0; alpha = 0;
	cTuning = param.cTuning0;

	// ============= Vexel =========================
	if (node[0]->IsKindOf(RUNTIME_CLASS( Vexel ))) {
		for (int i1 = 0; i1 < C; i1++)
			cluster.Add(new Cluster2DPoint(node));

		for (int i2 = 0; i2 < node.GetCount(); i2++) {
			int idx = (int)(cluster.GetCount()*cvRandReal(&rnd_seed));
			cluster[idx]->Add(node[i2]);
		}
	}

	// ============= Node2DPoint =========================
	if (node[0]->IsKindOf(RUNTIME_CLASS( Node2DPoint ))) {
		for (int i1 = 0; i1 < C; i1++)
			cluster.Add(new Cluster2DPoint(node));
		
		int Nrandomseed = 3;
		for (int i2 = 0; i2 < node.GetCount(); i2++) {
			if ((i2 % Nrandomseed) != 0)
				cluster[i2/10]->Add(node[i2]);
			else {
				int idx = (int)(cluster.GetCount()*cvRandReal(&rnd_seed));
				cluster[idx]->Add(node[i2]);
			}
		}
	}


	// ============= SwarmEvent =========================
	if (node[0]->IsKindOf(RUNTIME_CLASS( SwarmEvent ))) {
		if (!param.silent)
			StepProgressDialog("Determining compact groups of nodes...",0);	
		CArray<bool> nodeInCluster;   // boolean indicator if node is assigned to a cluster 
		for (int i = 0; i < node.GetCount(); i++)
			nodeInCluster.Add(false);
		int nInCluster = 0;			// count total number of clustered nodes

		for (int i = 0; i < node.GetCount(); i++) {
			CArray<int> nodePos;
			int nodeCardinality = 0;		  // number of times the event with the same actors repeats
			// find closely matching nodes
			for (int j = 0; j < node.GetCount(); j++) {
				// skip already assigned nodes
				if (nodeInCluster[j])
					continue;
				// group only nodes with exactly same actors
				if (((SwarmEvent*)(node[i]))->AreActorsUnique((SwarmEvent*)node[j])) {
					nodeCardinality++;
					nodePos.Add(j);
				}
			}
			
			// if enough found, create a new cluster
			if (nodeCardinality > param.minNodeCardinality) {
				SwarmActivity *ac = new SwarmActivity(node);
				cluster.Add(ac);
				for (int j = 0; j < nodePos.GetCount(); j++) {
					ac->Add(node[nodePos[j]]);
					ac->p += ((SwarmEvent*)(node[nodePos[j]]))->p;
					nodeInCluster[nodePos[j]] = true;	
					nInCluster++;
				}
			}
		}

		if (!param.silent)
			StepProgressDialog((CString)String::Format("Adding {0} orphaned nodes", node.GetCount()-nInCluster));

		//// For all non-clustered nodes add them to the nearest cluster
		//for (int i = 0; i < nodeInCluster.GetCount(); i++) {
		//	if (!nodeInCluster[i]) {
		//		// Find closest cluster
		//		double dMin = DBL_MAX;
		//		int jMin = 0;
		//		for (int j = 0; j < cluster.GetCount(); j++) {
		//			double dist = cluster[j]->DistanceSQR(node[i]);
		//			if (dist < dMin) {
		//				dMin = dist;
		//				jMin = j;
		//			}
		//		}
		//		cluster[jMin]->Add(node[i]);
		//	}
		//}
		if (!param.silent)
			StepProgressDialog();
		C = cluster.GetCount();
	}
}
// --------------------------------------------------------------------------
void ClusteringMachine::UpdateDistanceMatrix(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster) {
	int *min_id  = new int[node.GetCount()]; // ID of closest cluster
	double *min_d  = new double[node.GetCount()]; // distance to the closest cluster
	double *distances = new double[node.GetCount()]; // temp distances array
	for (int i = 0; i < node.GetCount(); i++)
			min_d[i] = DBL_MAX;

	// re-compute fuzzy distances
	for (int i1 = 0; i1 < cluster.GetCount(); i1++) {
		double T1, T2;
		int Tlt = 0, Tgt = 0; // less than, greater than
		IFuzzyCluster* c = cluster[i1];
		// first find closest cluster to each node (for crisp clustering)
		for (int i2 = 0; i2 < node.GetCount(); i2++) {
			INode*	n = node[i2];
			distances[i2] = c->DistanceSQR(n);
			if (distances[i2] < min_d[i2]) {
				min_d[i2] = distances[i2];
				min_id[i2] = c->id;
			}
		}
		// second, update distances
		for (int i2 = 0; i2 < node.GetCount(); i2++) {
			INode*	n = node[i2];
			c->rec[n->id]->d2 = distances[i2];
		}
		
	} 
	// BUG:: is last cluster still the closest?S !!!!!!!
	// YES: because u or psi are 0's for it

	// set new crisp clustering
	for (int i1 = 0; i1 < cluster.GetCount(); i1++) {
		double T1, T2;
		int Tlt = 0, Tgt = 0; // less than, greater than
		IFuzzyCluster* c = cluster[i1];
		for (int i2 = 0; i2 < node.GetCount(); i2++) {
			INode*	n = node[i2];
			if (c->id == min_id[i2])
				c->rec[n->id]->crispIn = true;
			else 
				c->rec[n->id]->crispIn = false;
		}
	}
		
	delete []min_id;
	delete []min_d;
	//c->rec[min_id]->crispIn = true;
}
// --------------------------------------------------------------------------
void ClusteringMachine::UpdateClusterMedians(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster) {

	double maxK = 0;
	for (int i = 0; i < cluster.GetCount(); i++) {
		IFuzzyCluster * c = cluster[i];
		c->UpdateMedian();
		c->UpdateMAD();
		if (c->S == 0)
			c->S = CLUSTERING_DIST_MIN;//cluster.RemoveAt(i);
		double K = (5*c->T+cTuning*c->S)/6;
		if (K > maxK)
			maxK = K;
	}
	
	for (int i = 0; i < cluster.GetCount(); i++) {
		IFuzzyCluster * c = cluster[i];
		c->K = maxK - (5*c->T+cTuning*c->S)/6;
	}
}
// --------------------------------------------------------------------------
void ClusteringMachine::UpdateEstimators(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster) {
	for (int i = 0; i < cluster.GetCount(); i++) {
		cluster[i]->UpdateRho(cTuning);
		cluster[i]->UpdatePsi(cTuning);
	}
}
// --------------------------------------------------------------------------
void ClusteringMachine::UpdateAlpha(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster) {
	double num = 0, denom = 0, denomC = 0;
	eta = param.eta0*exp(-abs(param.nIteration0-nIteration)/param.tau);
	for (int i = 0; i < cluster.GetCount(); i++) {
		denomC = 0;
		for (POSITION pos = cluster[i]->rec.GetStartPosition(); pos != NULL;) {
			int nid; Record* r;
			cluster[i]->rec.GetNextAssoc(pos, nid, r);
			num += r->u*r->u*r->rho;
			denomC += r->psi*r->u; 
		}
		denom += denomC*denomC;
	}
	ASSERT(!(Double::IsNaN(alpha) || Double::IsInfinity(alpha) || Double::IsNegativeInfinity(alpha)
		|| Double::IsNaN(num) || Double::IsInfinity(num) || Double::IsNegativeInfinity(num)
		|| Double::IsNaN(denom) || Double::IsInfinity(denom) || Double::IsNegativeInfinity(denom)));
 	alpha = eta*num/denom;
	//alpha = 0.0;
}
// --------------------------------------------------------------------------
void ClusteringMachine::UpdatePartitionMatrix(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster) {
	// update cardinality norms
	for (int i = 0; i < node.GetCount(); i++) {
		Nnode.SetAt(node[i]->id, 0);
		NnodeN.SetAt(node[i]->id, 0);
	}
	for (int i = 0; i < cluster.GetCount(); i++) {
		for (POSITION pos = cluster[i]->rec.GetStartPosition(); pos != NULL;) {
			int nid; Record* r;
			cluster[i]->rec.GetNextAssoc(pos, nid, r);
			ASSERT(r->rho > 0 && !Double::IsNaN(r->rho) && !Double::IsInfinity(r->rho) && !Double::IsNegativeInfinity(r->rho));
			double d = 0;
			if (NnodeN.Lookup(nid, d)) 
				NnodeN.SetAt(nid, d + cluster[i]->N/r->rho);

			if (Nnode.Lookup(nid, d))
				Nnode.SetAt(nid, d + 1/r->rho);
		}
	}
	// update cluster memberships
	double u_RRmin=DBL_MAX, u_RRmax=-DBL_MAX, u_Biasmin=DBL_MAX, u_Biasmax=-DBL_MAX;
	for (int i = 0; i < cluster.GetCount(); i++) {
		for (POSITION pos = cluster[i]->rec.GetStartPosition(); pos != NULL;) {
			int nid; Record* r;
			// !!!BUGS:::: check nid values
			cluster[i]->rec.GetNextAssoc(pos, nid, r);
			double nN, nn;
			if (!NnodeN.Lookup(nid, nN) || !Nnode.Lookup(nid, nn)) {
				ASSERT(false);continue;
			}
			//log->WriteLine(String::Format("id = {0:d} nn = {1:f2} nN = {2:f2}", nid, nn, nN));
			ASSERT(!(Double::IsNaN(nN) || Double::IsInfinity(nN) || Double::IsNegativeInfinity(nN)
				|| Double::IsNaN(nn) || Double::IsInfinity(nn) || Double::IsNegativeInfinity(nn)));

			r->u_RR = 1.0/r->rho/nn;
			r->u_Bias = alpha/r->rho*(cluster[i]->N - nN/nn);
			r->u = max(min(r->u_RR+r->u_Bias,1), 0);
			//r->u = r->u_RR+r->u_Bias;
		}
	}

	// update cluster cardinalities
	for (int i = 0; i < cluster.GetCount(); i++) {
		cluster[i]->UpdateN();
	}
	//log->WriteLine(String::Format("u_RRmin = {0:f2} u_RRmax = {1:f2} u_Biasmin = {2:f2} u_Biasmax = {3:f2}", u_RRmin, u_RRmax, u_Biasmin, u_Biasmax));
}
// --------------------------------------------------------------------------
void ClusteringMachine::RemoveEmptyClusters(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster) {
	for (int i1 = 0; i1 < cluster.GetCount(); i1++) {
		IFuzzyCluster* c = cluster[i1];
		bool foundCrisp = false;
		for (POSITION pos = c->rec.GetStartPosition(); pos != NULL;) {
			int nid; Record* r;
			c->rec.GetNextAssoc(pos, nid, r);
			if (r->crispIn == true) {
				foundCrisp = true;
				break;
			}
		}
		if (!foundCrisp)
			cluster.RemoveAt(i1);
	}
	C = cluster.GetCount();
}
// --------------------------------------------------------------------------
void ClusteringMachine::DiscardWeakClusters(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster) {
	// Compute average cluster cardinality
	double Naverage = 0;
	for (int i = 0; i < cluster.GetCount(); i++)
		Naverage += cluster[i]->N;
	Naverage /= C;

	// Kill the ones below average*minCardinality
	for (int i = 0; i < cluster.GetCount(); i++) {
		if (cluster[i]->N < Naverage*param.minCardinality)
			cluster.RemoveAt(i);
	}
	C = cluster.GetCount();
}
// --------------------------------------------------------------------------
void IFuzzyCluster::UpdateRho(double cTuning) {
	for (POSITION pos = rec.GetStartPosition(); pos != NULL;) {
		int nid; Record* r;
		rec.GetNextAssoc(pos, nid, r);
		//// TEMP
		//r->rho = r->d2;
		//continue;
		//// END TEMP
		if (T == 0 || S == 0)
			r->rho = CLUSTERING_DIST_MIN;
		else {
			if (r->d2 <= T) 
				r->rho = r->d2 - ( r->d2*r->d2*r->d2 / (6*T*T) );
			else if (r->d2 > T+cTuning*S) 
				r->rho = (5*T+cTuning*S)/6+K;
			else
				r->rho = pow(r->d2-(T+cTuning*S), 3)/(6*cTuning*cTuning*S*S) + (5*T+cTuning*S)/6;
		}
		r->rho = max(r->rho, CLUSTERING_DIST_MIN);
		ASSERT(!(Double::IsNaN(r->rho) || Double::IsInfinity(r->rho) || Double::IsNegativeInfinity(r->rho)));
	}
}
// --------------------------------------------------------------------------
void IFuzzyCluster::UpdatePsi(double cTuning) {
	for (POSITION pos = rec.GetStartPosition(); pos != NULL;) {
		int nid; Record* r;
		rec.GetNextAssoc(pos, nid, r);
		//// TEMP
		//r->psi = 0.5;
		//continue;
		//// END TEMP
		if (T == 0 || S == 0)
			r->psi = 1;
		else {
			if (r->d2 <= T)
				r->psi = 1-r->d2*r->d2/(2*T*T);
			else if (r->d2 > T+cTuning*S) 
				r->psi = 0;
			else
				r->psi = pow(r->d2-(T+cTuning*S), 2)/(2*cTuning*cTuning*S*S);
		}
		ASSERT(!(Double::IsNaN(r->psi) || Double::IsInfinity(r->psi) || Double::IsNegativeInfinity(r->psi)));
	}
}
// --------------------------------------------------------------------------
void IFuzzyCluster::UpdateN() {
	N = 0;
	for (POSITION pos = rec.GetStartPosition(); pos != NULL;) {
		int nid; Record* r;
		rec.GetNextAssoc(pos, nid, r);
		N += r->psi*r->u;
	}
}
// --------------------------------------------------------------------------
void IFuzzyCluster::UpdateMedian() {
	// create temp array
	int size = rec.GetCount();
	//if (size < 5) {
	//	AfxMessageBox("Less than 5 events! Can't you count them manually? ;-)");
	//	return;
	//}
	double *aTmp = new double[size];
	int n = 0;
	for (POSITION pos = rec.GetStartPosition(); pos;) {
		int nid; Record* r;
		rec.GetNextAssoc(pos, nid, r);
		if (r->crispIn)
			aTmp[n++] = r->d2;
	}
	if (n>0)
		T = median<double>(aTmp, n);
	delete[] aTmp;
}
// --------------------------------------------------------------------------
// compute median of absolute deviations
void IFuzzyCluster::UpdateMAD() {
	// create temp array
	int size = rec.GetCount();
	double *aTmp = new double[size];
	int n = 0;
	//if (size < 5) {
	//	AfxMessageBox("Less than 5 events! Can't you count them manually? ;-)");
	//	return;
	//}
	for (POSITION pos = rec.GetStartPosition(); pos;) {
		int nid; Record* r;
		rec.GetNextAssoc(pos, nid, r);
		if (r->crispIn)
			aTmp[n++] = fabs(r->d2-T);
	}
	if (n>0)
		S = median<double>(aTmp, n);
	ASSERT(!(Double::IsNaN(S) || Double::IsInfinity(S) || Double::IsNegativeInfinity(S)));
	delete[] aTmp;
}
// --------------------------------------------------------------------------
double IFuzzyCluster::DistanceSQR(INode *node) {
	double dist = 0, norm = 0;
	for (POSITION pos = rec.GetStartPosition(); pos != NULL; ) {
		int nid; Record* r;
		rec.GetNextAssoc(pos, nid, r);	
		double FUZZIFIER = 2;
		double dWeight = pow(r->u, FUZZIFIER) * r->psi;
		double d2 = r->node->DistanceSQR(node);
		dist += dWeight * d2;
		norm += dWeight;
	}
	//ASSERT(dist > 0);
	if (norm == 0)
		return CLUSTERING_DIST_MAX;
	else
		return max(CLUSTERING_DIST_MIN, dist/norm);
}
// --------------------------------------------------------------------------
void IFuzzyCluster::Add(INode *node) {
	nNodes++;
	Record *r;
	if (rec.Lookup(node->id, r)) {
		r->crispIn = true;
		r->u = 1;
		r->u_RR = 0;
		r->u_Bias = 0;
		r->d2 = 0;
	}
}
// --------------------------------------------------------------------------
System::String^ IFuzzyCluster::ToString() {
	System::String ^s 
		= System::String::Format("[IFuzzyCluster], ID: {0}", id);
	return s;
};
// --------------------------------------------------------------------------
IFuzzyCluster::IFuzzyCluster() {
	nNodes = 0; N = 1; T = 0.5; S = 0.5;
	this->id = last_id++;
}
// --------------------------------------------------------------------------
IFuzzyCluster::IFuzzyCluster(CArray<INode*> &node) {
	this->id = last_id++; nNodes = 0; N = 1; T = 0.5; S = 0.5;
	for (int i=0; i< node.GetCount();i++)
	{
		Record *r = new Record();
		r->node = node[i]; r->u = 0; r->u_RR = 0; r->u_Bias = 0; r->psi = 1; r->rho = 1; r->d2 = 0; r->crispIn = false;
		rec.SetAt(node[i]->id, r);
	}
}
// --------------------------------------------------------------------------
IFuzzyCluster::~IFuzzyCluster() {
	for (POSITION pos = rec.GetStartPosition(); pos;) {
		int nid; Record* r;
		rec.GetNextAssoc(pos, nid, r);
		delete r;
	}
}
// --------------------------------------------------------------------------
void IFuzzyCluster::Serialize( CArchive& ar )
{

}