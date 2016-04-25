#if !defined( Cluster2DPoint_H )
#define Cluster2DPoint_H
#pragma once

#include "ClusteringMachine.h"
class Node2DPoint;
class Cluster2DPoint : public IFuzzyCluster {
	public:
		static int last_id;
		// Attributes
		double p;		// cluster likelihood/confidence

		Cluster2DPoint();
		Cluster2DPoint(CArray<INode*> &node);
		System::String^ ToString();

		// IFuzzyCluster overrides
		virtual void Serialize(CArchive& ar) {};

};
#endif // !defined( Cluster2DPoint_H )