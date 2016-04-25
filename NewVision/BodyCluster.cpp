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
#include "BodyCluster.h"
#include "NewVisionDoc.h"

// --------------------------------------------------------------------------
BodyClusterElement::~BodyClusterElement() {
}
// --------------------------------------------------------------------------
BodyClusterElement::BodyClusterElement() {
	bot = cvPoint3D32f(0,0,0);
	orientation = 0;
	dwell = 0;
}
// --------------------------------------------------------------------------
BodyClusterElement::BodyClusterElement(Body *b) {
	id = b->id;
	bot = b->bot;
	orientation = b->orientation;
	dwell = b->dwell ? 1 : 0;
}
// --------------------------------------------------------------------------
BodyClusterElement::BodyClusterElement(int id, CvPoint3D32f bot, double orientation, double dwell) {
	this->id = id;
	this->bot = bot;
	this->orientation = orientation;
	this->dwell = dwell ? 1 : 0;
}
// --------------------------------------------------------------------------
BodyClusterElement::BodyClusterElement(const BodyClusterElement& that) {
	this->id = that.id;
	this->bot = that.bot;
	this->orientation = that.orientation;
	this->dwell = that.dwell;
}
// --------------------------------------------------------------------------
BodyClusterElement& BodyClusterElement::operator=(const BodyClusterElement& that) {
	this->id = that.id;
	this->bot = that.bot;
	this->orientation = that.orientation;
	this->dwell = that.dwell;
	return *this;
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
BodyCluster::BodyCluster() {
}
// --------------------------------------------------------------------------
BodyCluster::~BodyCluster() {
	sub.RemoveAll();
}
// --------------------------------------------------------------------------
BodyCluster::BodyCluster(int id, CvPoint3D32f bot, double orientation, double dwell) {
	this->id = id;
	this->bot = bot;
	this->orientation = orientation;
	this->dwell = dwell ? 1 : 0;
	sub.Add(new BodyClusterElement(id, bot, orientation, dwell ? 1 : 0));
}
// --------------------------------------------------------------------------
BodyCluster::BodyCluster(const BodyCluster& that) {
	this->id = that.id;
	this->bot = that.bot;
	this->orientation = that.orientation;
	this->dwell = that.dwell;
	this->sub.Copy(that.sub);
}
// --------------------------------------------------------------------------
BodyCluster::BodyCluster(BodyClusterElement *that) {
	this->id = that->id;
	this->bot = that->bot;
	this->orientation = that->orientation;
	this->dwell = that->dwell;
	this->sub.Add(that);
}
// --------------------------------------------------------------------------
BodyCluster& BodyCluster::operator=(const BodyCluster& that) {
	this->id = that.id;
	this->bot = that.bot;
	this->orientation = that.orientation;
	this->dwell = that.dwell;
	this->sub.Copy(that.sub);
	return *this;
}
// --------------------------------------------------------------------------
BodyCluster::BodyCluster(Body *b) {
	id = b->id;
	bot = b->bot;
	orientation = b->orientation;
	dwell = b->dwell ? 1 : 0;
	sub.Add(new BodyClusterElement(b->id, b->bot, b->orientation, b->dwell ? 1 : 0));
}
// --------------------------------------------------------------------------
void BodyCluster::Add(Body *b) {
	int N = sub.GetCount();
	if (N) {
		bot.x = (N*bot.x + b->bot.x)/(N+1);
		bot.y = (N*bot.y + b->bot.y)/(N+1);
		bot.z = (N*bot.z + b->bot.z)/(N+1);
		dwell = (N*dwell + b->dwell)/(N+1);
		double cc = (N*cos(orientation) + cos(b->orientation))/(N+1);
		double ss = (N*sin(orientation) + sin(b->orientation))/(N+1);
		orientation = atan2(ss, cc);
	}
	else {
		id = b->id;
		bot = b->bot;
		orientation = b->orientation;
		dwell = b->dwell ? 1 : 0;
	}
	sub.Add(new BodyClusterElement(b->id, b->bot, b->orientation, b->dwell ? 1 : 0));
}
// --------------------------------------------------------------------------
bool BodyCluster::In(int id) {
	for (int i=0;i<sub.GetCount();i++)
		if (sub[i]->id == id)
			return true;
	return false;
}
// --------------------------------------------------------------------------
BodyClusterElement* BodyCluster::Find(int id) {
	for (int i=0;i<sub.GetCount();i++)
		if (sub[i]->id == id)
			return sub[i];
	return NULL;
}
// --------------------------------------------------------------------------
void BodyCluster::Add(BodyClusterElement *b) {
	int N = sub.GetCount();
	if (N) {
		bot.x = (N*bot.x + b->bot.x)/(N+1);
		bot.y = (N*bot.y + b->bot.y)/(N+1);
		bot.z = (N*bot.z + b->bot.z)/(N+1);
		dwell = (N*dwell + b->dwell)/(N+1);
		double cc = (N*cos(orientation) + cos(b->orientation))/(N+1);
		double ss = (N*sin(orientation) + sin(b->orientation))/(N+1);
		orientation = atan2(ss, cc);
	}
	else {
		id = b->id;
		bot = b->bot;
		orientation = b->orientation;
		dwell = b->dwell ? 1 : 0;
	}
	sub.Add(new BodyClusterElement(b->id, b->bot, b->orientation, b->dwell ? 1 : 0));
}
// --------------------------------------------------------------------------
void BodyCluster::Add(BodyCluster *b) {
	// TODO: N == 0 case
	int N = sub.GetCount();
	int M = b->sub.GetCount();
	bot.x = (N*bot.x + M*b->bot.x)/(N+M);
	bot.y = (N*bot.y + M*b->bot.y)/(N+M);
	bot.z = (N*bot.z + M*b->bot.z)/(N+M);
	dwell = (N*dwell + M*(b->dwell ? 1 : 0))/(N+M);
	double cc = (N*cos(orientation) + M*cos(b->orientation))/(N+M);
	double ss = (N*sin(orientation) + M*sin(b->orientation))/(N+M);
	orientation = atan2(ss, cc);
	sub.Append(b->sub);
}
// --------------------------------------------------------------------------
void BodyCluster::Clear() {
	sub.RemoveAll();
}
// --------------------------------------------------------------------------
double ComputeInterDistance(SmartPtrArray<BodyCluster>& cl, SwarmEventDetector* comp) {
	double min_dist = DBL_MAX;
	for (int i=0;i<cl.GetCount();i++)
		for (int j=0;j<i;j++)
			min_dist = min(comp->ComputeDistance(cl[i], cl[j]), min_dist);
	return min_dist;
}