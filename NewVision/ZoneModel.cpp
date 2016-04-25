#include "StdAfx.h"
#include "ZoneModel.h"
#include "FloorRegion.h"
#include "NewVisionDoc.h"
#include "Globals.h"

IMPLEMENT_SERIAL(ZoneModel, Model, 1)
// ------------------------------------------------------------------------
ZoneModel::ZoneModel(void)
{
	last_id = 0;
}
// ------------------------------------------------------------------------
ZoneModel::~ZoneModel(void)
{
}
// ------------------------------------------------------------------------
void ZoneModel::Serialize( CArchive& ar )
{
	zone.Serialize( ar );
	if (ar.IsLoading()) {
		POSITION pos = zone.GetStartPosition();
		while (pos) {
			int i; FloorRegion *z;
			zone.GetNextAssoc(pos, i, z);
			z->doc = doc;
			if (z->id > last_id)
				last_id = z->id;
		}
	}
}
// ------------------------------------------------------------------------
bool ZoneModel::IsRealPointIn(CvPoint2D32f p, int zoneID) {
	int i; FloorRegion *z;
	if (zone.Lookup(zoneID, z)) {
		z->IsRealPointIn(p);
	}
	return false;
}
// ------------------------------------------------------------------------
int ZoneModel::GetZone(CvPoint2D32f p) {
	for (POSITION pos = zone.GetStartPosition();pos;) {
		int i; FloorRegion *z;
		zone.GetNextAssoc(pos, i, z);
		if (z->IsRealPointIn(p))
			return z->id;
	}
	return -1; 
}
// ------------------------------------------------------------------------
int ZoneModel::GetClosestZone(CvPoint2D32f p) {
	double minDist = DBL_MAX;
	int minI = -1;
	for (POSITION pos = zone.GetStartPosition();pos;) {
		int i; FloorRegion *z;
		zone.GetNextAssoc(pos, i, z);
		CvPoint2D32f point = cvPoint2D32f(p.x, p.y);
		double dist = d(point, z);
		if (dist < minDist) {
			minDist = dist;
			minI = z->id;
		}
	}
	return minI; 
}


// ------------------------------------------------------------------------
void ZoneModel::DrawFrame(CDC* pDC, CPoint off) {
	CPen blue(PS_SOLID, 2, RGB(0, 0, 255));
	POSITION pos = zone.GetStartPosition();
	while (pos) {
		int i; FloorRegion *z;
		zone.GetNextAssoc(pos, i, z);
		z->DrawFrame(pDC, off.x, off.y, &blue);
	}
}
// ------------------------------------------------------------------------
void ZoneModel::DrawFloor(CDC* pDC, CPoint off) {
	CPen blue(PS_SOLID, 2, RGB(255, 128, 0));
	POSITION pos = zone.GetStartPosition();
	while (pos) {
		int i; FloorRegion *z;
		zone.GetNextAssoc(pos, i, z);
		z->DrawFloor(pDC, off.x, off.y, &blue);
	}
}
// ------------------------------------------------------------------------
void ZoneModel::DrawFrame(IplImage* frame, CvScalar color) {
	POSITION pos = zone.GetStartPosition();
	while (pos) {
		int i; FloorRegion *z;
		zone.GetNextAssoc(pos, i, z);
		z->DrawFrame(frame, color);
	}
}
// ------------------------------------------------------------------------
void ZoneModel::DrawFloor(IplImage* frame, CvScalar color) {
	POSITION pos = zone.GetStartPosition();
	while (pos) {
		int i; FloorRegion *z;
		zone.GetNextAssoc(pos, i, z);
		z->DrawFloor(frame, color);
	}
}
// ------------------------------------------------------------------------
void ZoneModel::RasterizeFloor(IplImage* frame, CvScalar color) {
	POSITION pos = zone.GetStartPosition();
	while (pos) {
		int i; FloorRegion *z;
		zone.GetNextAssoc(pos, i, z);
		z->RasterizeFloor(frame, color);
	}
}