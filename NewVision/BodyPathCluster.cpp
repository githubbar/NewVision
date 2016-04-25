#include "stdafx.h"
#include "Globals.h"
#include "Body.h"
#include "BodyActivityModel.h"
#include "BodyPath.h"
#include "BodyPathCluster.h"
#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::Diagnostics;

// --------------------------------------------------------------------------
BodyPathCluster::BodyPathCluster(void)
{
	first = INT_MAX;
	last = 0;
}
// --------------------------------------------------------------------------
BodyPathCluster::~BodyPathCluster(void)
{
}
// --------------------------------------------------------------------------
BodyPathCluster::BodyPathCluster(BodyPath* path) {
	this->id = path->id;
	first = path->startFrame;
	last = path->startFrame + path->GetCount()-1;
	this->path.Add(path);
}
// --------------------------------------------------------------------------
void BodyPathCluster::AddCluster(BodyPathCluster* that) {
	// find first and last common frame
	int b = max(this->first, that->first);
	int e = min(this->last, that->last);

	// if no common subsequence found, return 
	if (b >= e)
		return;

	// update first/last to match longest common subsequence
	first = b; last = e;

	// add paths from 'that' cluster
	for (int i=0; i < that->path.GetCount(); i++) {
		this->path.Add(that->path[i]);
	}
}
// --------------------------------------------------------------------------
void BodyPathCluster::Add(BodyPath* that) {
	// find first and last common frame
	first = min(this->first, that->startFrame);
	last  = max(this->last, that->startFrame+that->GetCount()-1);
	this->path.Add(that);
}
// --------------------------------------------------------------------------
void BodyPathCluster::GetCenter(CArray<Step>& center) {

	for (int i = first; i <= last; i++) {
		CvPoint2D32f p = cvPoint2D32f(0,0);
		double height = 0, width = 0;
		for (int n = 0; n < path.GetCount(); n++) {
			p.x += path[n]->GetAt(i-path[n]->startFrame).p.x;
			p.y += path[n]->GetAt(i-path[n]->startFrame).p.y;
		}
		p.x /= path.GetCount();
		p.y /= path.GetCount();
		center.Add(Step(p, 0, 0, 0, 0, 0));
	}
}
// --------------------------------------------------------------------------
double BodyPathCluster::Compare(BodyPathCluster* that, int mature_path, int delta, int step) {
	// find first and last common frame
	int b = max(this->first, that->first);
	int e = min(this->last, that->last);

	// if longest common subsequence is too short, return large number
	if (e-b <= 2*delta/step)
		return Double::MaxValue;

	// computer centers of each cluster
	CArray<Step> center1, center2;
	this->GetCenter(center1);
	that->GetCenter(center2);

	// compute distance
	double distance_all = 0;
	int shift_count = 0;
	for (int shift=-delta; shift<=delta; shift+=step) {
		double distance = 0;
		double denom_all = 0;
		double denom = exp(-1.0*shift/delta);
		for (int i = b; i < e; i++) {
			if (i-that->first+shift >=0 && i+shift <= that->last) {
				distance += d(center1[i-this->first].p, center2[i-that->first+shift].p)*denom;
				denom_all += denom;
			}
		}
		if (denom_all == 0)
			continue;
		distance_all += distance/denom_all;
		shift_count++;
	}
	return distance_all/shift_count;
}
// --------------------------------------------------------------------------
bool BodyPathCluster::IsInCluster(int id) {
	for (int i=0; i < this->path.GetCount(); i++)
		if (this->path[i]->id == id)
			return true;
	return false;

}
// --------------------------------------------------------------------------