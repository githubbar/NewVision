#if !defined( CLUSTERINGMACHINE_H )
#define CLUSTERINGMACHINE_H
#pragma once
#include "Globals.h"
#include <afxtempl.h>
#include <list>

extern double CLUSTERING_DIST_MAX;
extern double CLUSTERING_DIST_MIN;

class CvMat;
// ------------------------------------------------------------------------
class INode : public CObject {
	DECLARE_DYNAMIC( INode )
public:
	static int last_id;
	int id;
	INode&				operator=(const INode& that) 
		{return *this;};
	virtual double			DistanceSQR(INode* that) = 0;	// computes a distance between to another element
	virtual System::String^ ToString() = 0;
	virtual void			Serialize(CArchive& ar) = 0;
};
// ------------------------------------------------------------------------
struct Record { 
	Record() {;};
	virtual void Serialize(CArchive& ar) {
		if (ar.IsStoring())	{
			ar << d2 << rho << psi << u;
		}
		else {
			ar >> d2 >> rho >> psi >> u;
		}

		//node->Serialize(ar);
	};
	INode* node;	// points to the node
	bool   crispIn; // indicated crisp membership of node N in cluster C
	double d2;		// square distance from cluster C to node N
	double rho;		// M-estimator
	double psi;		// W-estimator
	double u;		// membership in cluster C of node N
	double u_RR;    // sharing part of u
	double u_Bias;  // bias part of u
	};
// ------------------------------------------------------------------------
class IFuzzyCluster : public INode {
public:
	static int last_id;
	CMap<int, int&, Record*, Record*>			rec;			// array of membership parameters
	int						nNodes;								// number of nodes (assuming crisp clustering)
	double					T;									// median of cluster distances
	double					S;									// median of cluster absolute deviations 
	double					K;									// integration constant (makes all rho's througout the clusters reach the same maximum)
	double					N;									// robust cardinality
	
	IFuzzyCluster();
	IFuzzyCluster(CArray<INode*> &node);
	~IFuzzyCluster();
	IFuzzyCluster&				operator=(const IFuzzyCluster& that) 
		{return *this;};
	virtual void			Add(INode* that);				// add element
	virtual double			DistanceSQR(INode* that);			// computes a distance from INode to cluster
	virtual System::String^ ToString();
	virtual void			Serialize(CArchive& ar);
	void					UpdateMedian();
	void					UpdateMAD();
	void					UpdateRho(double cTuning);
	void					UpdatePsi(double cTuning);
	void					UpdateN();
};
//
//template<> void AFXAPI CopyElements <IFuzzyCluster*> (IFuzzyCluster** pDest, const IFuzzyCluster** pSrc, INT_PTR nCount) {
//}
// ------------------------------------------------------------------------
struct ClusteringParams {
	int 			maxIterations;		// maximum number of iterations
	int				minNodeCardinality; // minimum number of times the events with the same actors repeats (used while creating initial clusters)
	double			maxObjectiveFunctionDelta; // used as a stop criteria (convergence)
	int 			maxClusters;		// initial number of clusters
	double 			minNodeDistance;	// a threshold to remove nodes that are too far away from any other nodes (i.e outliers)
	double 			maxNodeDistance; 	// a threshold to group nodes to form initial clusters
	double			eta0;				// multiplier: controls the speed of clustering
	double			tau;				// exponent: controls the speed of clustering
	double			nIteration0;		// after this iteration clustering will slow down
	double			cTuningMin;			// a tuning constant
	double			cTuning0;			// a tuning constant
	double			cTuningDelta;		// a tuning constant
	double			minCardinality;		// minimum cardinality to keep cluster alive
	bool			silent;				// true=do not display messages

};
// ------------------------------------------------------------------------
class ClusteringMachine
{
	ClusteringParams param;
	//CMap<IntPair, IntPair&, Record, Record&> rec;  // store pair-wise cluster distances (size N^2)
	double goal; 
	bool	isRunning;
	int		nIteration;
	double	eta;
	double	alpha;
	int		C, Cmax;
	double	cTuning;			
	CMap<int, int&, double, double>	Nnode;			// weighted average of cardinalities for each node
	CMap<int, int&, double, double>	NnodeN;			// weighted average of cardinalities for each node /times N

	gcroot<System::IO::StreamWriter^> log;

public:
	ClusteringMachine(ClusteringParams &param);
	void	printD(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster);
	void 	RunFuzzy(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster);
	void 	InitializeClusters(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster);
	void 	UpdateDistanceMatrix(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster);
	void 	UpdateClusterMedians(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster);
	void 	UpdateEstimators(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster);
	void 	UpdateAlpha(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster);
	void 	UpdatePartitionMatrix(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster);
	void	RemoveEmptyClusters(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster);
	void 	DiscardWeakClusters(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster);
	double	GetObjective(CArray<INode*> &node, CArray<IFuzzyCluster*> &cluster);

private:

};

// ------------------------------------------------------------------------
template<> UINT AFXAPI HashKey<IntPair&> (IntPair& key);
// ------------------------------------------------------------------------
template<> BOOL AFXAPI CompareElements<IntPair , IntPair> (const IntPair* p1, const IntPair* p2);
template <> void AFXAPI SerializeElements <Record*> ( CArchive& ar, Record** p, INT_PTR nCount );

#endif // CLUSTERINGMACHINE_H