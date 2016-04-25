
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
#include "Cuboid.h"
#include "CameraModel.h"
#include "FloorModel.h"
#include "Globals.h"
#using <System.dll>
using namespace System;
using namespace System::Diagnostics;

IMPLEMENT_DYNAMIC(Cuboid, Obstacle)

// --------------------------------------------------------------------------
Cuboid::Cuboid(void) {
}
// --------------------------------------------------------------------------
Cuboid::Cuboid(CvPoint2D32f p, CNewVisionDoc* doc) {
	this->doc = doc;
	for (int i=0;i<NPTS;i++)
		this->v[i] = doc->cameramodel.coordsImage2Real(p);
}
// --------------------------------------------------------------------------
Cuboid::Cuboid(const Cuboid& that) {
	this->doc = that.doc;
	for (int i=0;i<NPTS;i++)
		this->v[i] = that.v[i];
}
// --------------------------------------------------------------------------
Cuboid::~Cuboid(void) {
}
// --------------------------------------------------------------------------
Cuboid& Cuboid::operator=(const Cuboid& that) {
	this->doc = that.doc;
	for (int i=0;i<NPTS;i++)
		this->v[i] = that.v[i];
	return *this;
}
// --------------------------------------------------------------------------
bool Cuboid::Move(CvPoint3D32f p) {
	for (int i=0;i<NPTS;i++) {
		v[i].x += p.x;
		v[i].y += p.y;
		v[i].z += p.z;
	}
	return true;
}
// --------------------------------------------------------------------------
bool Cuboid::Move1(CvPoint2D32f p) {
	// v[0] can be anywhere 
	for (int i=0;i<NPTS;i++)
		v[i] = doc->cameramodel.coordsImage2Real(p);
	return true;
}
// --------------------------------------------------------------------------
bool Cuboid::Move2(CvPoint2D32f p) {
	// v[1] can be anywhere 
	v[1] = doc->cameramodel.coordsImage2Real(p);
	return true;
}
// --------------------------------------------------------------------------
bool Cuboid::Move3(CvPoint2D32f p) {
	// v[2] must be on a line perpendicular to p1p2 and intersecting v[1]
	// (1) convert all to the floor plane coordinates
	CvPoint3D32f pp = doc->cameramodel.coordsImage2Real(p);
	CvPoint3D32f pp1 = v[0];
	CvPoint3D32f pp2 = v[1];
	CvPoint2D32f P = cvPoint2D32f(pp.x, pp.y), P1 = cvPoint2D32f(pp1.x, pp1.y), P2 = cvPoint2D32f(pp2.x, pp2.y);
	// (2) p_ = p proj p1p2
	double u = ((P.x-P1.x)*(P2.x-P1.x)+(P.y-P1.y)*(P2.y-P1.y))/d_2(P1, P2);
	CvPoint P_ = cvPoint((int)(P1.x+u*(P2.x-P1.x)), (int)(P1.y+u*(P2.y-P1.y)));
	// (3) v[2] = v[1] + (p-p_)
	CvPoint3D32f P3 = cvPoint3D32f(0,0,0), P4 = cvPoint3D32f(0,0,0);
	P3.x = P2.x + (P.x-P_.x);
	P3.y = P2.y + (P.y-P_.y);
	P4.x = P1.x + P3.x - P2.x;
	P4.y = P1.y + P3.y - P2.y;

	v[2] = P3;
	v[3] = P4;
	v[4] = v[0]; v[5] = v[1]; v[6] = v[2]; v[7] = v[3];
	return true;
}
// --------------------------------------------------------------------------
bool Cuboid::Move4(CvPoint2D32f p) {
	// must be on a line perpendicular to plane v0v1v2v3 and intersecting v[2]
	// essentially v[6].z-v[2].z = h
	double h = doc->cameramodel.computeZ(p, v[2]);
	v[4] = cvPoint3D32f(v[0].x, v[0].y, h);
	v[5] = cvPoint3D32f(v[1].x, v[1].y, h);
	v[6] = cvPoint3D32f(v[2].x, v[2].y, h);
	v[7] = cvPoint3D32f(v[3].x, v[3].y, h);
	return true;
}
// --------------------------------------------------------------------------
void Cuboid::DrawFloor(IplImage* frame, CvScalar color) {
	CvPoint pf1 = doc->floormodel.coordsReal2Floor(v[0]);
	CvPoint pf2 = doc->floormodel.coordsReal2Floor(v[1]);
	CvPoint pf3 = doc->floormodel.coordsReal2Floor(v[2]);
	CvPoint pf4 = doc->floormodel.coordsReal2Floor(v[3]);

	cvLine(frame, pf1, pf2, color, 2, CV_AA);
	cvLine(frame, pf2, pf3, color, 2, CV_AA);
	cvLine(frame, pf3, pf4, color, 2, CV_AA);
	cvLine(frame, pf4, pf1, color, 2, CV_AA);

	// Draw label text
	int leftMost = min(min(pf1.x, pf2.x), min(pf3.x, pf4.x));
	//int rightMost = max(max(pf1.x, pf2.x), max(pf3.x, pf4.x));
	int center = (pf1.y+pf2.y+pf3.y+pf4.y)/4;
	drawTextInRectangle(label, cvPoint(leftMost, center), frame, CV_RGB(0,0,0), CV_RGB(255,255,150), (float)1);
}
// --------------------------------------------------------------------------
void Cuboid::DrawFloor(CDC* pDC, int ox, int oy, CPen* pen) {
	// draw vertices
	CPen* pOldPen = pDC->SelectObject(pen);
	int leftMost = INT_MAX, rightMost = INT_MIN, center = 0;
	CvPoint im[NPTS/2];
	for (int i=0;i<NPTS/2;i++) {
		im[i] = doc->floormodel.coordsReal2Floor(v[i]);
		pDC->Ellipse(im[i].x-DOT-ox, im[i].y-DOT-oy, im[i].x+DOT-ox, im[i].y+DOT-oy);
		if (im[i].x < leftMost)
			leftMost = im[i].x;
		if (im[i].x > rightMost)
			rightMost = im[i].x;
		center += im[i].y;
	}
	center /= NPTS/2;

	// draw edges
	CPoint off = CPoint(-ox, -oy);
	CPen base(PS_SOLID, 0, RGB(0, 255, 255)); pDC->SelectObject(base);
	pDC->MoveTo(cPoint(im[0])+off);pDC->LineTo(cPoint(im[1])+off);
	pDC->MoveTo(cPoint(im[1])+off);pDC->LineTo(cPoint(im[2])+off);
	pDC->MoveTo(cPoint(im[2])+off);pDC->LineTo(cPoint(im[3])+off);
	pDC->MoveTo(cPoint(im[3])+off);pDC->LineTo(cPoint(im[0])+off);

	// draw label
	int TEXT_HEIGHT = 16;
	CRect rToolTip(leftMost-ox,center-TEXT_HEIGHT/2-oy, rightMost-ox, center+TEXT_HEIGHT/2-oy);
	drawStaticToolTip(pDC, label, rToolTip);

	pDC->SelectObject(pOldPen);
	return;	
}
// --------------------------------------------------------------------------
void Cuboid::DrawFrame(IplImage* frame, CvScalar color) {
}
// --------------------------------------------------------------------------
void Cuboid::DrawFrame(CDC* pDC , int ox, int oy, CPen* pen) {
	// draw vertices
	CPen* pOldPen = pDC->SelectObject(pen);
	int leftMost = INT_MAX, rightMost = INT_MIN, center = 0;
	CvPoint im[NPTS];
	for (int i=0;i<NPTS;i++) {
		im[i] = cvPointFrom32f(doc->cameramodel.coordsReal2Image(v[i]));
//		echo(String::Concat(__box(v[i].x), S" ", __box(v[i].y),  S" ",__box(v[i].z)));
		pDC->Ellipse(im[i].x-DOT-ox, im[i].y-DOT-oy, im[i].x+DOT-ox, im[i].y+DOT-oy);
		if (im[i].x < leftMost)
			leftMost = im[i].x;
		if (im[i].x > rightMost)
			rightMost = im[i].x;
		center += im[i].y;
	}
	center /= NPTS;

	// draw edges
	CPoint off = CPoint(-ox, -oy);
	CPen base(PS_SOLID, 0, RGB(0, 255, 255)); pDC->SelectObject(base);
	CPoint pts[2*NPTS];
	DWORD cnts[] = {4, 4, 4, 4};

	if (doc->cameramodel.m_cameraType == SPHERICAL && d(im[0], im[1]) > doc->cameramodel.w/2) {
		rightMost = leftMost*2; leftMost = 0;
		pts[1] = cPoint(im[0])+off;
		pts[2] = cPoint(im[3])+off;
		pts[5] = cPoint(im[2])+off;
		pts[6] = cPoint(im[1])+off;
		pts[9] = cPoint(im[4])+off;
		pts[10] = cPoint(im[7])+off;
		pts[13] = cPoint(im[6])+off;
		pts[14] = cPoint(im[5])+off;
		if (v[0].y > v[1].y) {
			pts[0] = cPoint(0, (im[1].y+im[0].y)/2)+off;
			pts[3] = cPoint(0, (im[3].y+im[2].y)/2)+off;
			pts[4] = cPoint(doc->cameramodel.w-1, (im[3].y+im[2].y)/2)+off;
			pts[7] = cPoint(doc->cameramodel.w-1, (im[1].y+im[0].y)/2)+off;
			pts[8] = cPoint(0, (im[5].y+im[4].y)/2)+off;
			pts[11] = cPoint(0, (im[7].y+im[6].y)/2)+off;
			pts[12] = cPoint(doc->cameramodel.w-1, (im[7].y+im[6].y)/2)+off;
			pts[15] = cPoint(doc->cameramodel.w-1, (im[5].y+im[4].y)/2)+off;
		}
		else {
			pts[0] = cPoint(doc->cameramodel.w-1, (im[1].y+im[0].y)/2)+off;
			pts[3] = cPoint(doc->cameramodel.w-1, (im[3].y+im[2].y)/2)+off;
			pts[4] = cPoint(0, (im[3].y+im[2].y)/2)+off;
			pts[7] = cPoint(0, (im[1].y+im[0].y)/2)+off;
			pts[8] = cPoint(doc->cameramodel.w-1, (im[5].y+im[4].y)/2)+off;
			pts[11] = cPoint(doc->cameramodel.w-1, (im[7].y+im[6].y)/2)+off;
			pts[12] = cPoint(0, (im[7].y+im[6].y)/2)+off;
			pts[15] = cPoint(0, (im[5].y+im[4].y)/2)+off;
		}
		pDC->PolyPolyline(pts, cnts, 4);
	}
	else if (doc->cameramodel.m_cameraType == SPHERICAL && d(im[1], im[2]) > doc->cameramodel.w/2) {
		rightMost = leftMost*2; leftMost = 0;
		pts[1] = cPoint(im[0])+off;
		pts[2] = cPoint(im[1])+off;
		pts[5] = cPoint(im[2])+off;
		pts[6] = cPoint(im[3])+off;
		pts[9] = cPoint(im[4])+off;
		pts[10] = cPoint(im[5])+off;
		pts[13] = cPoint(im[6])+off;
		pts[14] = cPoint(im[7])+off;
		if (v[1].y > v[2].y) {
			pts[0] = cPoint(0, (im[3].y+im[0].y)/2)+off;
			pts[3] = cPoint(0, (im[1].y+im[2].y)/2)+off;
			pts[4] = cPoint(doc->cameramodel.w-1, (im[1].y+im[2].y)/2)+off;
			pts[7] = cPoint(doc->cameramodel.w-1, (im[3].y+im[0].y)/2)+off;
			pts[8] = cPoint(0, (im[7].y+im[4].y)/2)+off;
			pts[11] = cPoint(0, (im[5].y+im[6].y)/2)+off;
			pts[12] = cPoint(doc->cameramodel.w-1, (im[5].y+im[6].y)/2)+off;
			pts[15] = cPoint(doc->cameramodel.w-1, (im[7].y+im[4].y)/2)+off;
		}
		else {
			pts[0] = cPoint(doc->cameramodel.w-1, (im[3].y+im[0].y)/2)+off;
			pts[3] = cPoint(doc->cameramodel.w-1, (im[1].y+im[2].y)/2)+off;
			pts[4] = cPoint(0, (im[1].y+im[2].y)/2)+off;
			pts[7] = cPoint(0, (im[3].y+im[0].y)/2)+off;
			pts[8] = cPoint(doc->cameramodel.w-1, (im[7].y+im[4].y)/2)+off;
			pts[11] = cPoint(doc->cameramodel.w-1, (im[5].y+im[6].y)/2)+off;
			pts[12] = cPoint(0, (im[5].y+im[6].y)/2)+off;
			pts[15] = cPoint(0, (im[7].y+im[4].y)/2)+off;
		}
		pDC->PolyPolyline(pts, cnts, 4);
	}
	else  {
		CPoint pts[NPTS+2];
		DWORD cnts[] = {5, 5};
		pts[0] = cPoint(im[0])+off;
		pts[1] = cPoint(im[1])+off;
		pts[2] = cPoint(im[2])+off;
		pts[3] = cPoint(im[3])+off;
		pts[4] = cPoint(im[0])+off;
		pts[5] = cPoint(im[4])+off;
		pts[6] = cPoint(im[5])+off;
		pts[7] = cPoint(im[6])+off;
		pts[8] = cPoint(im[7])+off;
		pts[9] = cPoint(im[4])+off;
		pDC->PolyPolyline(pts, cnts, 2);
	}

	CPen vert(PS_SOLID, 0, RGB(128, 255, 128));pDC->SelectObject(vert);
	pDC->MoveTo(cPoint(im[0])+off);pDC->LineTo(cPoint(im[4])+off);
	pDC->MoveTo(cPoint(im[1])+off);pDC->LineTo(cPoint(im[5])+off);
	pDC->MoveTo(cPoint(im[2])+off);pDC->LineTo(cPoint(im[6])+off);
	pDC->MoveTo(cPoint(im[3])+off);pDC->LineTo(cPoint(im[7])+off);

	int TEXT_HEIGHT = 16;
	CRect rToolTip(leftMost-ox,center-TEXT_HEIGHT/2-oy, rightMost-ox, center+TEXT_HEIGHT/2-oy);
	drawStaticToolTip(pDC, label, rToolTip);

	pDC->SelectObject(pOldPen);
	return;	
}
// --------------------------------------------------------------------------
void Cuboid::RasterizeFrame(IplImage* frame, CvScalar color) {
	CvPoint im[NPTS];
	for (int i=0;i<NPTS;i++)
		im[i] = cvPointFrom32f(doc->cameramodel.coordsReal2Image(v[i]));

	CvPoint pts[2*NPTS];

	if (doc->cameramodel.m_cameraType == SPHERICAL && d(im[0], im[1]) > doc->cameramodel.w/2) {
		pts[1] = im[0];
		pts[2] = im[3];
		pts[5] = im[2];
		pts[6] = im[1];
		pts[9] = im[4];
		pts[10] = im[7];
		pts[13] = im[6];
		pts[14] = im[5];
		if (v[0].y > v[1].y) {
			pts[0] = cvPoint(0, (im[1].y+im[0].y)/2);
			pts[3] = cvPoint(0, (im[3].y+im[2].y)/2);
			pts[4] = cvPoint(doc->cameramodel.w-1, (im[3].y+im[2].y)/2);
			pts[7] = cvPoint(doc->cameramodel.w-1, (im[1].y+im[0].y)/2);
			pts[8] = cvPoint(0, (im[5].y+im[4].y)/2);
			pts[11] = cvPoint(0, (im[7].y+im[6].y)/2);
			pts[12] = cvPoint(doc->cameramodel.w-1, (im[7].y+im[6].y)/2);
			pts[15] = cvPoint(doc->cameramodel.w-1, (im[5].y+im[4].y)/2);
		}
		else {
			pts[0] = cvPoint(doc->cameramodel.w-1, (im[1].y+im[0].y)/2);
			pts[3] = cvPoint(doc->cameramodel.w-1, (im[3].y+im[2].y)/2);
			pts[4] = cvPoint(0, (im[3].y+im[2].y)/2);
			pts[7] = cvPoint(0, (im[1].y+im[0].y)/2);
			pts[8] = cvPoint(doc->cameramodel.w-1, (im[5].y+im[4].y)/2);
			pts[11] = cvPoint(doc->cameramodel.w-1, (im[7].y+im[6].y)/2);
			pts[12] = cvPoint(0, (im[7].y+im[6].y)/2);
			pts[15] = cvPoint(0, (im[5].y+im[4].y)/2);
		}
		cvFillConvexPoly(frame, &pts[0], NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, &pts[4], NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, &pts[8], NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, &pts[12], NPTS/2, color, CV_AA);
		CvPoint v1[] = {pts[0], pts[1], pts[9], pts[8]};
		CvPoint v2[] = {pts[2], pts[3], pts[11], pts[10]};
		CvPoint v3[] = {pts[4], pts[5], pts[13], pts[12]};
		CvPoint v4[] = {pts[6], pts[7], pts[15], pts[14]};
		cvFillConvexPoly(frame, v1, NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, v2, NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, v3, NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, v4, NPTS/2, color, CV_AA);
	}
	else if (doc->cameramodel.m_cameraType == SPHERICAL && d(im[1], im[2]) > doc->cameramodel.w/2) {
		pts[1] = im[0];
		pts[2] = im[1];
		pts[5] = im[2];
		pts[6] = im[3];
		pts[9] = im[4];
		pts[10] = im[5];
		pts[13] = im[6];
		pts[14] = im[7];
		if (v[1].y > v[2].y) {
			pts[0] = cvPoint(0, (im[3].y+im[0].y)/2);
			pts[3] = cvPoint(0, (im[1].y+im[2].y)/2);
			pts[4] = cvPoint(doc->cameramodel.w-1, (im[1].y+im[2].y)/2);
			pts[7] = cvPoint(doc->cameramodel.w-1, (im[3].y+im[0].y)/2);
			pts[8] = cvPoint(0, (im[7].y+im[4].y)/2);
			pts[11] = cvPoint(0, (im[5].y+im[6].y)/2);
			pts[12] = cvPoint(doc->cameramodel.w-1, (im[5].y+im[6].y)/2);
			pts[15] = cvPoint(doc->cameramodel.w-1, (im[7].y+im[4].y)/2);
		}
		else {
			pts[0] = cvPoint(doc->cameramodel.w-1, (im[3].y+im[0].y)/2);
			pts[3] = cvPoint(doc->cameramodel.w-1, (im[1].y+im[2].y)/2);
			pts[4] = cvPoint(0, (im[1].y+im[2].y)/2);
			pts[7] = cvPoint(0, (im[3].y+im[0].y)/2);
			pts[8] = cvPoint(doc->cameramodel.w-1, (im[7].y+im[4].y)/2);
			pts[11] = cvPoint(doc->cameramodel.w-1, (im[5].y+im[6].y)/2);
			pts[12] = cvPoint(0, (im[5].y+im[6].y)/2);
			pts[15] = cvPoint(0, (im[7].y+im[4].y)/2);
		}
		cvFillConvexPoly(frame, &pts[0], NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, &pts[4], NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, &pts[8], NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, &pts[12], NPTS/2, color, CV_AA);
		CvPoint v1[] = {pts[0], pts[1], pts[9], pts[8]};
		CvPoint v2[] = {pts[2], pts[3], pts[11], pts[10]};
		CvPoint v3[] = {pts[4], pts[5], pts[13], pts[12]};
		CvPoint v4[] = {pts[6], pts[7], pts[15], pts[14]};
		cvFillConvexPoly(frame, v1, NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, v2, NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, v3, NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, v4, NPTS/2, color, CV_AA);
	}
	else  {
		CvPoint poly1[] = {im[0], im[1], im[2], im[3]};
		CvPoint poly2[] = {im[4], im[5], im[6], im[7]};
		CvPoint poly3[] = {im[0], im[1], im[5], im[4]};
		CvPoint poly4[] = {im[1], im[2], im[6], im[5]};
		CvPoint poly5[] = {im[2], im[3], im[7], im[6]};
		CvPoint poly6[] = {im[3], im[0], im[4], im[7]};
		cvFillConvexPoly(frame, poly1, NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, poly2, NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, poly3, NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, poly4, NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, poly5, NPTS/2, color, CV_AA);
		cvFillConvexPoly(frame, poly6, NPTS/2, color, CV_AA);
	}
}
// --------------------------------------------------------------------------
void Cuboid::RasterizeFloor(IplImage* frame, CvScalar color) {
	CvPoint pf1 = doc->floormodel.coordsReal2Floor(v[0]);
	CvPoint pf2 = doc->floormodel.coordsReal2Floor(v[1]);
	CvPoint pf3 = doc->floormodel.coordsReal2Floor(v[2]);
	CvPoint pf4 = doc->floormodel.coordsReal2Floor(v[3]);
	CvPoint poly[] = {pf1, pf2, pf3, pf4};
	cvFillConvexPoly(frame, poly, 4, color, CV_AA);
}
// --------------------------------------------------------------------------
void Cuboid::GetOccludedFloorArea(CvSeq* points) {
	// find a projection of each vertex on the floor
	for (int i=0;i<NPTS;i++) {

	}
}
// --------------------------------------------------------------------------
CvPoint3D32f Cuboid::GetCenter() {
	CvPoint3D32f center = cvPoint3D32f(0,0,0);
	for (int i=0;i<NPTS;i++) {
		center.x += v[i].x;
		center.y += v[i].y;
		center.z += v[i].z;
	}
	center.x /= NPTS;
	center.y /= NPTS;
	center.z /= NPTS;
	
	return center;
}
// --------------------------------------------------------------------------
double Cuboid::GetDistanceFromCamera() {
	return doc->cameramodel.coordsDistanceFromCamera(GetCenter());
	// get the depth of the obstacle
	//double max_dist = 0;
	//for (int i=0;i<NPTS;i++) {
	//	CvPoint3D32f pp = doc->cameramodel.coordsImage2Real(cvPointTo32f(v[i]));
	//	double dist = doc->cameramodel.coordsDistanceFromCamera(pp);
	//	if (dist < min_dist)
	//		minax_dist = dist;
	//}

	//return max_dist;
}
// --------------------------------------------------------------------------
//int Cuboid::Compare(Object3D* that) {
//	int diff = this->GetDistanceFromCamera() > that->GetDistanceFromCamera();
//	if (diff > 0)
//		return 1;
//	else if (diff == 0)
//		return 0;
//	else 
//		return -1;
//}
// --------------------------------------------------------------------------
void Cuboid::Serialize( CArchive& archive ) {
	
	if( archive.IsStoring() ) {
		archive << label << id;
		for (int i=0;i<NPTS;i++)
			archive << v[i].x << v[i].y << v[i].z;
	}
	else {
		archive >> label >> id;
		for (int i=0;i<NPTS;i++)
			archive >> v[i].x >> v[i].y >> v[i].z;
	}
}
// --------------------------------------------------------------------------
template <> void AFXAPI SerializeElements <Object3D*> ( CArchive& ar, Object3D** p, INT_PTR nCount )
{
    int i; 
    if (ar.IsStoring()) 
		for (i=0; i<nCount; i++)
			p[i]->Serialize(ar); 
    else 
		for (i=0; i<nCount; i++) { 
			p[i] = new Cuboid();
            p[i]->Serialize(ar); 
		}

}
// --------------------------------------------------------------------------
bool Cuboid::IsFramePointIn(CvPoint point, CvPoint3D32f& pcurrent) {
	for (int j=0;j<Cuboid::NPTS;j++) {
		CvPoint vertex = cvPointFrom32f(doc->cameramodel.coordsReal2Image(v[j]));
		if(d(point, vertex) <= DOT) {
			pcurrent = v[j];
			return true;
		}
	}
	return false;
}
// --------------------------------------------------------------------------
bool Cuboid::IsFloorPointIn(CvPoint point, CvPoint3D32f& pcurrent) {
	for (int j=0;j<Cuboid::NPTS;j++) {
		CvPoint vertex = doc->floormodel.coordsReal2Floor(v[j]);
		if(d(point, vertex) <= DOT) {
			pcurrent = v[j];
			return true;
		}
	}
	return false;
}