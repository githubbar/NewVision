
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
#include "Vexel.h"
#include "NewVisionDoc.h"

#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::IO;
using namespace System::Diagnostics;

int Vexel::last_id = 0;
IMPLEMENT_DYNAMIC( Vexel, INode)
// --------------------------------------------------------------------------
Vexel::Vexel(void)
{
	id = last_id++; 
}
// --------------------------------------------------------------------------	
Vexel::Vexel(const Vexel& that) {
	this->p = that.p;
	this->v = that.v;
	this->id = that.id;
}
// --------------------------------------------------------------------------	
Vexel& Vexel::operator=(const Vexel& that) {
	this->p = that.p;
	this->v = that.v;
	this->id = that.id;
	return *this;
}
// --------------------------------------------------------------------------
Vexel::~Vexel(void)
{
}
// --------------------- INode overrides ------------------------------------
// --------------------------------------------------------------------------
double Vexel::DistanceSQR(INode *node) {
	Vexel* that = dynamic_cast< Vexel* > (node);

	if (!that)
		return CLUSTERING_DIST_MAX;

	if (that == this)
		return CLUSTERING_DIST_MIN;

	// compute distance: locations + angle of velocities
	// 
	//double wPos = 0.8;
	//return wPos*d(this->p, that->p) + (1-wPos)*abs(angle(this->v, that->v))/(PI/2);
	return abs(angle(this->v, that->v))/(PI/2);
}
// --------------------------------------------------------------------------
System::String^ Vexel::ToString() {
	System::String ^s 
		= System::String::Format("[Vexel],  ID: {0}", id) 
		+ System::String::Format("x={0} y={1} vx={2} xy={3}", p.x, p.y, v.x, v.y);
	return s;
};
