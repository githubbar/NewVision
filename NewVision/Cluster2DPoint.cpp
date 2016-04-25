#include "StdAfx.h"
#include "Node2DPoint.h"
#include "Cluster2DPoint.h"
#include "Globals.h"

#include <float.h>
#include <math.h>
#include <typeinfo.h>
// ============================================================================================================
// ====================== Cluster2DPoint Class =================================================================
int Cluster2DPoint::last_id = 0;
// --------------------------------------------------------------------------
Cluster2DPoint::Cluster2DPoint() : IFuzzyCluster ()
{
}
// --------------------------------------------------------------------------
Cluster2DPoint::Cluster2DPoint( CArray<INode*> &node ) : IFuzzyCluster (node)
{

}
// --------------------------------------------------------------------------
System::String^ Cluster2DPoint::ToString() {
	System::String ^s = System::String::Format("{0:d}, ", this->id);
	double EPSILON = 10e-3;
	for (POSITION pos = rec.GetStartPosition(); pos != NULL;) {
		int nid; Record *r;
		rec.GetNextAssoc(pos, nid, r);
		Node2DPoint* node = (Node2DPoint*)(r->node);
		s += System::String::Format("{0:d}, {1:f3}, ", nid, r->u);
	}
	return s;
};
// --------------------- IFuzzyCluster overrides ---------------------------------
