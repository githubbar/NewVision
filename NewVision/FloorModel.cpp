#include "StdAfx.h"
#include "FloorModel.h"
#include "NewVisionDoc.h"
#include "Globals.h"
IMPLEMENT_SERIAL(FloorModel, Model, 1)
// ------------------------------------------------------------------------
FloorModel::FloorModel(void)
{
}
// ------------------------------------------------------------------------
FloorModel::~FloorModel(void)
{
}
// ------------------------------------------------------------------------
CvPoint FloorModel::coordsReal2Floor(CvPoint3D32f p) {
	CvPoint pf = cvPoint((int)(w*(p.x-floormin.x)/(floormax.x-floormin.x)),
				(int)(h*(p.y-floormin.y)/(floormax.y-floormin.y)));
	return pf;
}
// ------------------------------------------------------------------------
CvPoint FloorModel::coordsReal2Floor(CvPoint2D32f p) {
	CvPoint pf = cvPoint((int)(w*(p.x-floormin.x)/(floormax.x-floormin.x)),
				(int)(h*(p.y-floormin.y)/(floormax.y-floormin.y)));
	return pf;
}
// ------------------------------------------------------------------------
 CvPoint3D32f FloorModel::coordsFloor2Real(CvPoint p) {
	return cvPoint3D32f(
		floormin.x+p.x*(floormax.x-floormin.x)/w,
		floormin.y+p.y*(floormax.y-floormin.y)/h,
		0);
}
// ------------------------------------------------------------------------
CvSize FloorModel::sizeReal2Floor(CvSize2D32f s) {
	CvSize axes = cvSize((int)(w*s.width/(floormax.x-floormin.x)),
				(int)(h*s.height/(floormax.y-floormin.y)));
	return axes;
}
// ------------------------------------------------------------------------
bool FloorModel::InBounds(CvPoint3D32f p) {
	if (p.x > floormax.x || p.x < floormin.x 
		|| p.y > floormax.y || p.y < floormin.y )
		return false;
	return true;
}
// ------------------------------------------------------------------------
bool FloorModel::InBounds(CvPoint2D32f p) {
	if (p.x > floormax.x || p.x < floormin.x 
		|| p.y > floormax.y || p.y < floormin.y )
		return false;
	return true;
}
// ------------------------------------------------------------------------
void FloorModel::GetMinMaxXY(CvPoint2D32f& cammin, CvPoint2D32f& cammax) {
	if (m_extreme.GetCount() == 0)
		return;
	// find 3D coordinates of 4 image corners on the floor
	CvPoint3D32f p = doc->cameramodel.coordsImage2Real(cvPointTo32f(m_extreme[0]), 0);
	cammin.x = p.x; cammin.y = p.y;	cammax.x = p.x; cammax.y = p.y;
	for (int i=1;i<m_extreme.GetCount();i++) {
		p = doc->cameramodel.coordsImage2Real(cvPointTo32f(m_extreme[i]), 0);
		cammin.x = min(p.x, cammin.x); cammin.y = min(p.y, cammin.y);
		cammax.x = max(p.x, cammax.x); cammax.y = max(p.y, cammax.y);
	}
}
// ------------------------------------------------------------------------
void FloorModel::Serialize( CArchive& ar )
{
	if( ar.IsStoring())
		ar << m_sourcename << w << h;
	else
		ar >> m_sourcename >> w >> h;
	m_extreme.Serialize( ar );	
	// TEMP: scale 4
	//if( ar.IsLoading())
	//	for (int i=0;i< m_extreme.GetCount();i++) {
	//		m_extreme[i].x *= 2;
	//		m_extreme[i].y *= 2;
	//	}
}
// ------------------------------------------------------------------------
void FloorModel::Draw(IplImage* frame, CvScalar color) {
	if (m_type == 0) {
		if (floormap->width == frame->width && floormap->height == frame->height)
			cvCopy(floormap, frame);
	}
	else {
	}
}
// ------------------------------------------------------------------------
#if USE_X3DLIBRARY
#include "X3DDriver.h"
void FloorModel::ExportToX3D( X3DDriver &xd )
{
	// x <-> y to convert from left-coordinate system to right coordinate system
	CString minmaxExt;
	minmaxExt.Format("%.2f %.2f 0, %.2f %.2f 0, %.2f %.2f 0, %.2f %.2f 0", 
		doc->floormodel.floormin.y, 
		doc->floormodel.floormin.x,
		doc->floormodel.floormax.y, 
		doc->floormodel.floormin.x, 
		doc->floormodel.floormax.y, 
		doc->floormodel.floormax.x, 
		doc->floormodel.floormin.y, 
		doc->floormodel.floormax.x);
	xd.AddFloorImage(doc->floormodel.m_sourcename, minmaxExt);
}
#endif
