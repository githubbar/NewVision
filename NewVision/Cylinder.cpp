
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
#include "NewVision.h"
#include "NewVisionDoc.h"
#include "Cylinder.h"
#include "CameraModel.h"
#include "FloorModel.h"
#include "Globals.h"
#using <System.dll>
using namespace System;
using namespace System::Diagnostics;
IMPLEMENT_DYNAMIC(Cylinder, Obstacle)

// --------------------------------------------------------------------------
Cylinder::Cylinder(void) {
}
// --------------------------------------------------------------------------
Cylinder::Cylinder(CvPoint2D32f p, CNewVisionDoc* doc) {
	this->doc = doc;
	this->p1 = doc->cameramodel.coordsImage2Real(p);
	this->p2 = p1;
	this->R = 1;
}
// --------------------------------------------------------------------------
Cylinder::Cylinder(const Cylinder& that) {
	this->doc = that.doc;
	this->p1 = that.p1;
	this->p2 = that.p2;
	this->R = that.R;
}
// --------------------------------------------------------------------------
Cylinder::~Cylinder(void) {
}
// --------------------------------------------------------------------------
Cylinder& Cylinder::operator=(const Cylinder& that) {
	this->doc = that.doc;
	this->p1 = that.p1;
	this->p2 = that.p2;
	this->R = that.R;
	return *this;
}
// --------------------------------------------------------------------------
bool Cylinder::Move(CvPoint3D32f p) {
	p1.x += p.x; p1.y += p.y; p1.z += p.z;
	p2.x += p.x; p2.y += p.y; p2.z += p.z;
	return true;
}
// --------------------------------------------------------------------------
bool Cylinder::Move1(CvPoint2D32f p) {
	// move base center
	p1 = doc->cameramodel.coordsImage2Real(p);
	p2 = p1;
	return true;
}
// --------------------------------------------------------------------------
bool Cylinder::Move2(CvPoint2D32f p) {
	// move radius
	CvPoint3D32f rp = doc->cameramodel.coordsImage2Real(p);
	R = d(rp, p1);
	return true;
}
// --------------------------------------------------------------------------
bool Cylinder::Move3(CvPoint2D32f p) {
	// change height
	p2.z = (float)doc->cameramodel.computeZ(p, p1);
	return true;
}
// --------------------------------------------------------------------------
bool Cylinder::Move4(CvPoint2D32f p) {
	return false;
}
// --------------------------------------------------------------------------
void Cylinder::DrawFloor(IplImage* frame, CvScalar color) {
}
// --------------------------------------------------------------------------
void Cylinder::DrawFloor(CDC* pDC, int ox, int oy, CPen* pen) {
	CPen* pOldPen = pDC->SelectObject(pen);
	CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(HOLLOW_BRUSH);
	CvPoint im = doc->floormodel.coordsReal2Floor(p1);
	CvSize r = doc->floormodel.sizeReal2Floor(cvSize2D32f(R, R));
	pDC->Ellipse(im.x-DOT-ox, im.y-DOT-oy, im.x+DOT-ox, im.y+DOT-oy);

	// draw base 
	CPen base(PS_SOLID, 0, RGB(0, 255, 255)); pDC->SelectObject(base);
	pDC->Ellipse(im.x-r.width-ox, im.y-r.height-oy, im.x+r.width-ox, im.y+r.height-oy);

	// draw label
	int TEXT_HEIGHT = 16;
	CRect rToolTip(im.x-r.width-ox,im.y-TEXT_HEIGHT/2-oy, im.x+r.width-ox, im.y+TEXT_HEIGHT/2-oy);
	drawStaticToolTip(pDC, label, rToolTip);

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	return;	
}
// --------------------------------------------------------------------------
void Cylinder::DrawFrame(IplImage* frame, CvScalar color) {

}
// --------------------------------------------------------------------------
void Cylinder::DrawFrame(CDC* pDC , int ox, int oy, CPen* pen) {
	CPen* pOldPen = pDC->SelectObject(pen);
	CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(HOLLOW_BRUSH);
	// draw vertices
	CvPoint im1 = cvPointFrom32f(doc->cameramodel.coordsReal2Image(p1));
	CvPoint im2 = cvPointFrom32f(doc->cameramodel.coordsReal2Image(p2));
	pDC->Ellipse(im1.x-DOT-ox, im1.y-DOT-oy, im1.x+DOT-ox, im1.y+DOT-oy);
	pDC->Ellipse(im2.x-DOT-ox, im2.y-DOT-oy, im2.x+DOT-ox, im2.y+DOT-oy);

	// find bounding rectangle for the bottom & top pancake
	double D = sqrt(p1.x*p1.x+p1.y*p1.y);
	double co = p1.x/D;
	double si = p1.y/D;
	if (Double::IsNaN(co) || Double::IsNaN(si))
		return;
	CvPoint3D32f lt3B = cvPoint3D32f(p1.x+R*co-R*si, p1.y+R*si-R*co, p1.z);
	CvPoint3D32f rb3B = cvPoint3D32f(p1.x-R*co+R*si, p1.y-R*si+R*co, p1.z);
	CvPoint3D32f lt3U = cvPoint3D32f(p1.x+R*co-R*si, p1.y+R*si-R*co, p2.z);
	CvPoint3D32f rb3U = cvPoint3D32f(p1.x-R*co+R*si, p1.y-R*si+R*co, p2.z);
	CvPoint ltB  = cvPointFrom32f(doc->cameramodel.coordsReal2Image(lt3B));
	CvPoint rbB  = cvPointFrom32f(doc->cameramodel.coordsReal2Image(rb3B));
	CvPoint ltU  = cvPointFrom32f(doc->cameramodel.coordsReal2Image(lt3U));
	CvPoint rbU  = cvPointFrom32f(doc->cameramodel.coordsReal2Image(rb3U));

	// draw base and top
	CPen base(PS_SOLID, 0, RGB(0, 255, 255)); pDC->SelectObject(base);
	pDC->Ellipse(ltB.x-ox, ltB.y-oy, rbB.x-ox, rbB.y-oy);
	pDC->Ellipse(ltU.x-ox, ltU.y-oy, rbU.x-ox, rbU.y-oy);

	// draw vertical lines
	CPen vert(PS_SOLID, 0, RGB(128, 255, 128));pDC->SelectObject(vert);
	//pDC->MoveTo(cPoint(im[0])+off);pDC->LineTo(cPoint(im[4])+off);
	

	// draw label
	int TEXT_HEIGHT = 16;
	CRect rToolTip(ltU.x-ox,(ltB.y+rbU.y)/2-TEXT_HEIGHT/2-oy, rbB.x-ox, (ltB.y+rbU.y)/2+TEXT_HEIGHT/2-oy);
	drawStaticToolTip(pDC, label, rToolTip);

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	return;	
}
// --------------------------------------------------------------------------
void Cylinder::RasterizeFrame(IplImage* frame, CvScalar color) {
}
// --------------------------------------------------------------------------
void Cylinder::RasterizeFloor(IplImage* frame, CvScalar color) {
}
// --------------------------------------------------------------------------
void Cylinder::GetOccludedFloorArea(CvSeq* points) {
}
// --------------------------------------------------------------------------
CvPoint3D32f Cylinder::GetCenter() {
	CvPoint3D32f center = cvPoint3D32f(p1.x, p1.y, (p1.z + p2.z)/2);
	return center;
}
// --------------------------------------------------------------------------
double Cylinder::GetDistanceFromCamera() {
	return doc->cameramodel.coordsDistanceFromCamera(GetCenter());
}
// --------------------------------------------------------------------------
void Cylinder::Serialize( CArchive& archive ) {
	
	if( archive.IsStoring() ) {
		archive << label;
		archive << p1.x << p1.y << p1.z;
		archive << p2.x << p2.y << p2.z;
		archive << R;
	}
	else {
		archive >> label;
		archive >> p1.x >> p1.y >> p1.z;
		archive >> p2.x >> p2.y >> p2.z;
		archive >> R;
	}
}
// --------------------------------------------------------------------------
bool Cylinder::IsFramePointIn(CvPoint point, CvPoint3D32f& pcurrent) {
	CvPoint v1 = cvPointFrom32f(doc->cameramodel.coordsReal2Image(p1));
	CvPoint v2 = cvPointFrom32f(doc->cameramodel.coordsReal2Image(p2));
	if(d(point, v1) <= DOT) {
		pcurrent = p1;
		return true;
	}
	if(d(point, v2) <= DOT) {
		pcurrent = p2;
		return true;
	}
	return false;
}
// --------------------------------------------------------------------------
bool Cylinder::IsFloorPointIn(CvPoint point, CvPoint3D32f& pcurrent) {
	CvPoint v1 = doc->floormodel.coordsReal2Floor(p1);
	CvPoint v2 = doc->floormodel.coordsReal2Floor(p2);
	if(d(point, v1) <= DOT) {
		pcurrent = p1;
		return true;
	}
	if(d(point, v2) <= DOT) {
		pcurrent = p2;
		return true;
	}
	return false;
}