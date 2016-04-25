#include "StdAfx.h"
#include "SwarmEvent.h"
#include <float.h>
#include <typeinfo.h>
// ============================================================================================================
// ====================== SwarmEvent Class ====================================================================
int SwarmEvent::last_id = 0;
IMPLEMENT_DYNAMIC( SwarmEvent, INode)
// --------------------------------------------------------------------------
SwarmEvent::SwarmEvent () {
	id = 0; 
	start = 0; 
	stop = 0; 
	type = ""; 
	p = 0;
};
// --------------------------------------------------------------------------
SwarmEvent::SwarmEvent (int f, CString t) {
	id = last_id++; 
	start = f; 
	stop = f; 
	type = t; 
	p = 0;
};
// --------------------------------------------------------------------------
void SwarmEvent::AddActor(int aid, double p) {
	double pNow;
	if (!actors.Lookup(aid, pNow))
		pNow = 0;
	actors.SetAt(aid, pNow+p);
	this->p += pNow+p;
}
// --------------------------------------------------------------------------
bool SwarmEvent::OccurredAt(int nFrame) {
	if (nFrame >= start && nFrame <= stop) 
		return true; 
	else
		return false;
}
// --------------------------------------------------------------------------
void SwarmEvent::Serialize(CArchive& ar) {
	if (ar.IsStoring())	{
		ar << id << type << start << stop << p;
	}
	else {
		ar >> id >> type >> start >> stop >> p;
	}
	actors.Serialize(ar);
}
// --------------------------------------------------------------------------
int SwarmEvent::GetAverageFrameDistance(SwarmEvent *that) {
	return abs( (this->stop + this->start)/2 - (that->stop + that->start)/2 );
}
// --------------------------------------------------------------------------
double SwarmEvent::Spread() {
	//double distance = 0;
	//for (int i1 = 0; i1 < sub.GetCount(); i1++)
	//	for (int i2 = 0; i2 < i1; i2++) {
	//		distance += sub[i1]->DistanceSQR(sub[i2]);
	//	}
	//	return distance/sub.GetCount();
	return 0;
}
// --------------------------------------------------------------------------
void SwarmEvent::Add(SwarmEvent *t) {
	SwarmEvent* that = dynamic_cast< SwarmEvent* > (t);
	if (that == this || that->type.Compare(this->type) != 0 || that->actors.GetCount() == 0)
		return;
	this->start = min(this->start, that->start);
	this->stop = max(this->stop, that->stop);
	POSITION pos = that->actors.GetStartPosition();
	while (pos) {
		int id; double p;
		that->actors.GetNextAssoc(pos, id, p);
		this->AddActor(id, p);
	}
}
// --------------------------------------------------------------------------
bool SwarmEvent::AreActorsUnique( SwarmEvent *that )
{
	// count matching actors and corresponding likelihoods
	int nCommonActors = 0, nAllActors = max(this->actors.GetCount(), that->actors.GetCount());
	for (POSITION pos = actors.GetStartPosition();pos;) {
		int id; double pNow, pIncoming;
		actors.GetNextAssoc(pos, id, pNow);	
		if (that->actors.Lookup(id, pIncoming)) {
			nCommonActors++;
		}
	}
	return (nCommonActors == nAllActors);
}
// --------------------- INode overrides ------------------------------------
// --------------------------------------------------------------------------
double SwarmEvent::DistanceSQR(INode *node) {
	SwarmEvent* that = dynamic_cast< SwarmEvent* > (node);
	if (that == this)
		return CLUSTERING_DIST_MIN;

	if (that->type.Compare(this->type) != 0 || that->actors.GetCount() == 0)
		return CLUSTERING_DIST_MAX;

	// compute time distance
	int timeDelta =  max(0, max(that->start - this->stop, this->start - that->stop));
	int avgTimeDelta = abs((that->start + that->stop) - (this->start + this->stop) );

	// count matching actors and corresponding likelihoods
	int nCommonActors = 0, nAllActors = 0;
	double pAllIncoming = 0, pAllNow = 0;
	for (POSITION pos = actors.GetStartPosition();pos;) {
		int id; double pNow, pIncoming;
		actors.GetNextAssoc(pos, id, pNow);	
		if (that->actors.Lookup(id, pIncoming)) {
			pAllIncoming += pIncoming;
			pAllNow += pNow;
			nCommonActors++;
		}
		nAllActors++;
	}

	// count remaining actors in "that"
	for (POSITION pos = that->actors.GetStartPosition();pos;) {
		int id; double dummy;
		that->actors.GetNextAssoc(pos, id, dummy);
		if (!actors.Lookup(id, dummy))
			nAllActors++;
	}

	pAllIncoming /= (that->p);
	pAllNow /= (this->p);

	// the distance is shorter if more actors match and they are more separated in time
	double EPSILON = 10e-3, SHIFT = 100, SCALE = 1000, ASHIFT = 0.4, ASCALE = 0.07, W1 = 0.7, W2 = 1-W1;

	double distance = 
		   W1*(sigmoid(1-((double)nCommonActors)/nAllActors, ASHIFT, ASCALE))
		 + W2*tanh(avgTimeDelta/SCALE);
	return distance*distance;
}
// --------------------------------------------------------------------------
System::String^ SwarmEvent::ToString() {
	System::String ^s 
		= System::String::Format("[SwarmEvent],  ID: {0}", id) 
		+ System::String::Format(", Type: {0}", gcnew System::String(type))
		+ System::String::Format(", Frames: {0}-{1}", start, stop)
		+ System::String::Format(", Confidence: {0:F}", p);
	POSITION pos = actors.GetStartPosition();
	while (pos) {
		int id; double d;
		actors.GetNextAssoc(pos, id, d);
		s += System::String::Format(", ID={0}", id);
	}

	// output as id, x, y where y is a mapping of actors
	//System::String ^s = System::String::Format("{0:d}, {1:f3}, {2:f3}, {3:f3}", id, 0.5*(this->start + this->stop), y);
	return s;
};
