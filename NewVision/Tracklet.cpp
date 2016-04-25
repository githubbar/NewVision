
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
#include "stdafx.h"
#include "Globals.h"
#include "Tracklet.h"
#include "NewVisionDoc.h"

#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::IO;
using namespace System::Diagnostics;

int Tracklet::last_id = 0;
// --------------------------------------------------------------------------
Tracklet::Tracklet(void)
{
	id = last_id++; 
}
// --------------------------------------------------------------------------	
Tracklet::Tracklet(const Tracklet& that) {
	Copy(that);
	this->id = that.id;
	this->startFrame = that.startFrame;
}
// --------------------------------------------------------------------------	
Tracklet& Tracklet::operator=(const Tracklet& that) {
	Copy(that);
	this->id = that.id;
	this->startFrame = that.startFrame;
	return *this;
}
// --------------------------------------------------------------------------
Tracklet::~Tracklet(void)
{
}
// --------------------- INode overrides ------------------------------------
// --------------------------------------------------------------------------
double Tracklet::DistanceSQR(INode *node) {
	Tracklet* that = dynamic_cast< Tracklet* > (node);

	if (!that)
		return CLUSTERING_DIST_MAX;

	if (that == this)
		return CLUSTERING_DIST_MIN;

	// find first and last common frame
	int b = max(this->startFrame, that->startFrame);
	int e = min(this->startFrame+this->GetCount(), that->GetCount()+that->startFrame);
	
	// if no common subsequence found, return large number
	if (b >= e)
		return Double::MaxValue;

	// compute distance ( shift = =0)
	double dist = 0;
	for (int i = b; i < e; i++) {
		dist += d(this->GetAt(i-this->startFrame), that->GetAt(i-that->startFrame));
	}
	return dist/(e-b+1);
}
// --------------------------------------------------------------------------
System::String^ Tracklet::ToString() {
	System::String ^s 
		= System::String::Format("[Tracklet],  ID: {0}", id) 
		+ System::String::Format(", Start: {0}", startFrame);
	return s;
};
