/*!
 * 
 * Copyright : Alex Leykin 2007
 *
 *	Email : alexleykin@gmail.com
 *
 *	This code may be used in compiled form in any way you desire. This
 *	file may be redistributed unmodified by any means PROVIDING it is 
 *	not sold for profit without the authors written consent, and 
 *	providing that this notice and the authors name is included.
 *
 *	Description  
 *  ----------- 
*/

#include "StdAfx.h"
#include "SwarmEvent.h"
#include "SwarmActivity.h"
#include "Globals.h"
#include "PersistantFormats.h"
#include "BodyPath.h"

#include <float.h>
#include <math.h>
#include <typeinfo.h>
// ============================================================================================================
// ====================== SwarmActivity Class =================================================================
int SwarmActivity::last_id = 0;
// --------------------------------------------------------------------------
SwarmActivity::SwarmActivity() : IFuzzyCluster () {
	p = 0;
}
// --------------------------------------------------------------------------
SwarmActivity::SwarmActivity(CArray<INode*> &node) : IFuzzyCluster (node) {
	p = 0;
}
// --------------------------------------------------------------------------
System::String^ SwarmActivity::ToString() {
	System::String ^s 
		= System::String::Format("{0}", id) 
		//+ System::String::Format(", Type: {0}", gcnew System::String(type))
		+ System::String::Format(", {0} ", p)
		+ System::String::Format(", {0}, {1}", start, stop);

	double EPSILON = 10e-3;
	//for (POSITION pos = rec.GetStartPosition(); pos != NULL;) {
	//	int nid; Record *r;
	//	rec.GetNextAssoc(pos, nid, r);
	//	if (r->u > EPSILON)
	//		s += System::String::Format(", {0}, {1:f3}, {2:f3} ", nid, r->d, r->u);
	//}
	//double pAll = 0;
	//for (POSITION pos = actors.GetStartPosition(); pos != NULL;) {
	//	int aid; double p;
	//	actors.GetNextAssoc(pos, aid, p);
	//	pAll += p;
	//}
	//for (POSITION pos = actors.GetStartPosition(); pos != NULL;) {
	//	int aid; double p;
	//	actors.GetNextAssoc(pos, aid, p);
	//	s += System::String::Format(", {0}, {1:f3}", aid, p);
	//}
	for (POSITION pos = events.GetStartPosition(); pos != NULL;) {
		int eid; double p;
		events.GetNextAssoc(pos, eid, p);
		s += System::String::Format(", {0}, {1:f3}", eid, p);
	}
	return s;
};
// --------------------------------------------------------------------------
/**
 * Test for actor IDs equality
 * \param *that 
 * \return number of matching actors
 */
int SwarmActivity::EqualActors( SwarmActivity *that )
{
	// match all actors from 'this'
	int nEqual = 0;
	for (POSITION pos1 = this->actors.GetStartPosition(); pos1 != NULL;) {
		int aid; double p;
		this->actors.GetNextAssoc(pos1, aid, p);
		if (that->actors.Lookup(aid, p))
			nEqual++;
	}

	// no mismatches found, return true
	return nEqual;
}
// --------------------------------------------------------------------------
/**
 * Adds new actors and updates the weights of already existing ones
 * \param *that 
 */
void SwarmActivity::Merge( SwarmActivity *that )
{
	for (POSITION pos = that->actors.GetStartPosition(); pos != NULL;) {
		int aid; double pThis = 0,  pThat = 0;
		that->actors.GetNextAssoc(pos, aid, pThat);
		if (this->actors.Lookup(aid, pThis))
			this->actors.SetAt(aid, (this->p*pThis+that->p*pThat)/(this->p + that->p));
		else
			this->actors.SetAt(aid, (that->p*pThat)/(this->p + that->p));
	}
	for (POSITION pos = that->events.GetStartPosition(); pos != NULL;) {
		int eid; double pThis = 0,  pThat = 0;
		that->events.GetNextAssoc(pos, eid, pThat);
		if (this->events.Lookup(eid, pThis))
			this->events.SetAt(eid, (this->p*pThis+that->p*pThat)/(this->p + that->p));
		else
			this->events.SetAt(eid, (that->p*pThat)/(this->p + that->p));
	}
	this->N += that->N;
	this->p += that->p;
	//Normalize();
}
// --------------------------------------------------------------------------
/**
* Makes all actor's "p" sum up to one
*/
void SwarmActivity::Normalize()
{
	double pAll = 0;
	for (POSITION pos = actors.GetStartPosition(); pos != NULL;) {
		int aid; double p;
		actors.GetNextAssoc(pos, aid, p);
		pAll += p;
	}

	for (POSITION pos = actors.GetStartPosition(); pos != NULL;) {
		int aid; double p;
		actors.GetNextAssoc(pos, aid, p);
		actors.SetAt(aid, p/pAll);
	}
}
// ------------------------------------------------------------------------
void SwarmActivity::UpdateStartStop(TrackingData *tr) {
	start = INT_MAX, stop = INT_MIN;
	for (POSITION posA = actors.GetStartPosition(); posA;) {
		int aid; double p;
		actors.GetNextAssoc(posA, aid, p);
		BodyPath *bp;
		if (tr->data.Lookup(aid, bp)) {
			int first, last;
			bp->GetFirstAndLastVisible(first, last);
			if (first < start)
				start = first;
			if (last > stop)
				stop = last;
		}
	}
}
// --------------------------------------------------------------------------
/**
 * Removes all actors with p < m_minActorWeight
 * \param m_minActorWeight 
 */
void SwarmActivity::FilterActors( double m_minActorWeight )
{
	for (POSITION pos = actors.GetStartPosition(); pos != NULL;) {
		int aid; double p;
		actors.GetNextAssoc(pos, aid, p);
		if (p < m_minActorWeight)
			actors.RemoveKey(aid);
	}
}
// --------------------- IFuzzyCluster overrides ---------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
void SwarmActivity::Serialize(CArchive& ar) {
	IFuzzyCluster::Serialize(ar);
	if (ar.IsStoring())	{
		ar << id << type << start << stop << p;
	}
	else {
		ar >> id >> type >> start >> stop >> p;
	}
	rec.Serialize(ar);
	actors.Serialize(ar);
	events.Serialize(ar);
}
// --------------------------------------------------------------------------
void SwarmActivity::Add( INode* that )
{
	IFuzzyCluster::Add(that);
	if (!that->IsKindOf(RUNTIME_CLASS( SwarmEvent ))) 
		return;

	this->events.SetAt(that->id, ((SwarmEvent*)that)->p);
}
