#include "StdAfx.h"
#include "Node2DPoint.h"
#include <float.h>
#include <typeinfo.h>
// ============================================================================================================
// ====================== Node2DPoint Class ====================================================================
int Node2DPoint::last_id = 0;
IMPLEMENT_DYNAMIC( Node2DPoint, INode)
// --------------------------------------------------------------------------
Node2DPoint::Node2DPoint () {
	id = last_id++; 
	this->x = cvRandReal(&rnd_seed); this->y = cvRandReal(&rnd_seed);
};
// --------------------------------------------------------------------------
Node2DPoint::Node2DPoint (double x, double y) {
	id = last_id++; 
	this->x = x; this->y = y;
};
// --------------------- INode overrides ------------------------------------
// --------------------------------------------------------------------------
double Node2DPoint::DistanceSQR(INode *node) {
	// squared Euclidean distance
	Node2DPoint* that = dynamic_cast< Node2DPoint* > (node);
	if (that == this)
		return CLUSTERING_DIST_MIN;
	return (this->x-that->x)*(this->x-that->x)+(this->y-that->y)*(this->y-that->y);
}
// --------------------------------------------------------------------------
System::String^ Node2DPoint::ToString() {
	System::String ^s = System::String::Format("{0:d}, {1:f3}, {2:f3}", id, x, y);
	return s;
};
