
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
#include "FloorRegion.h"
#include "NewVision.h"
#include "Globals.h"
#include "NewVisionDoc.h"

// --------------------------------------------------------------------------
FloorRegion::FloorRegion(void) {
	m_regionType = REGION_UNDIRECTIONAL;
}
// --------------------------------------------------------------------------
FloorRegion::FloorRegion(int id, CNewVisionDoc* doc) {
	this->doc = doc;
	this->id = id;
}
// --------------------------------------------------------------------------
FloorRegion::FloorRegion(CvPoint2D32f point, CNewVisionDoc* doc) {
	this->doc = doc;
	m_regionType = REGION_UNDIRECTIONAL;
	p.Add(point);
}
// --------------------------------------------------------------------------
FloorRegion& FloorRegion::operator=(const FloorRegion& that) {
	this->doc = that.doc;
	return *this;
}
// --------------------------------------------------------------------------
FloorRegion::FloorRegion(const FloorRegion& that) {
	this->doc = that.doc;
	m_regionType = REGION_UNDIRECTIONAL;
	//this->x = that.x;
	//this->y = that.y;
}
// --------------------------------------------------------------------------
FloorRegion::~FloorRegion(void) {
}
// --------------------------------------------------------------------------
CvPoint3D32f FloorRegion::GetCenter() {
	CvPoint3D32f c = cvPoint3D32f(0,0,0);
	for (int i=0;i<p.GetCount();i++) {
		c.x += p[i].x;
		c.y += p[i].y;
	}
	c.x /= p.GetCount();
	c.y /= p.GetCount();
	return c;
}
// --------------------------------------------------------------------------
bool FloorRegion::IsRealPointIn(CvPoint2D32f point) {
	return cvPointInPoly(point, p);
}
// --------------------------------------------------------------------------
void FloorRegion::AddBorder(int w) {
	// Add border of w (cm); Parallel shift each edge by w - this will create N new vertices
	// Solve system of two equations: (1) - line v1 to v2 and (2) - distance from |v3-v1|=w
	// to find point [x3, y3]
	CArray<CvPoint2D32f> arrNew;
	int N = p.GetCount();
	CvPoint3D32f c3 = GetCenter();
	CvPoint2D32f c = cvPoint2D32f(c3.x, c3.y);

	for (int i=0;i<N;i++) {
		CvPoint2D32f a1, b1, a2, b2;
		pointAtD2line(w, p[i], p[(i+1)%N], a1, b1);
		pointAtD2line(w, p[(i+1)%N], p[i], a2, b2);

		// which one: b1 or b2? must be outside the polygon
		CvPoint2D32f v1 = cvPoint2D32f(a1.x-p[i].x, a1.y-p[i].y);
		CvPoint2D32f vc1 = cvPoint2D32f(p[i].x-c.x, p[i].y-c.y);
		CvPoint2D32f v2 = cvPoint2D32f(a2.x-p[(i+1)%N].x, a2.y-p[(i+1)%N].y);
		CvPoint2D32f vc2 = cvPoint2D32f(p[(i+1)%N].x-c.x, p[(i+1)%N].y-c.y);
		double d1 = dotProduct(v1, vc1);
		double d2 = dotProduct(v2, vc2);
		double d3 = d(p[i], c);
		double d4 = d(a1, p[i]);
		double d5 = d(b1, p[i]);
		// add only vertices that are on outward pointing rays
		if (dotProduct(v1, vc1) > 0) {
			arrNew.Add(a1);  arrNew.Add(b2);   }
		else {
			arrNew.Add(b1); arrNew.Add(a2);  }
		//if (dotProduct(v2, vc2))
		//	arrNew.Add(a2);  
		//else
		//	arrNew.Add(b2); 

	}
	p.RemoveAll();
	p.Copy(arrNew);
}
// --------------------------------------------------------------------------
void FloorRegion::DrawFrame(IplImage* frame, CvScalar color) {
	//cvCircle(frame, cvPoint(x, y), 3, color, -1);
	int nPts = p.GetSize();
	CvPoint *pts = new CvPoint[nPts];
	for (int i=0;i<nPts;i++) {
		pts[i] = cvPointFrom32f(doc->cameramodel.coordsReal2Image(cvPoint3D32f(p[i].x, p[i].y, 0)));
	}
	// Draw a smoothed poly-line
	cvPolyLine(frame, &pts, &nPts, 1, 1, color, 1, CV_AA);
	delete pts;

	return;
}
// --------------------------------------------------------------------------
void FloorRegion::DrawFrame(CDC* pDC , int ox, int oy, CPen* pen) {
	CPen pNew(PS_SOLID, 1, RGB(0, 0, 255));
	CPen* pOldPen = pDC->SelectObject(&pNew);
	int nPts = p.GetSize();
	CPoint *pts = new CPoint[nPts];
	for (int i=0;i<nPts;i++) {
		CvPoint2D32f pp = doc->cameramodel.coordsReal2Image(cvPoint3D32f(p[i].x, p[i].y, 0));
		pts[i] = CPoint((int)(pp.x - ox), (int)(pp.y - oy));
	}
	pDC->Polyline(pts, nPts);
	pDC->MoveTo(pts[nPts-1]);
	pDC->LineTo(pts[0]);
	pDC->SelectObject(pOldPen);
	CBrush bNew(RGB(0, 255, 0));
	CBrush* pOldBrush = pDC->SelectObject(&bNew);
	for (int i=0;i<nPts;i++) {
		pDC->Ellipse(pts[i].x-DOT, pts[i].y-DOT, pts[i].x+DOT, pts[i].y+DOT);
	}

	//pDC->SelectObject(new CPen(PS_SOLID, 1, RGB(255, 255, 50)));
	//CvPoint3D32f c3 = GetCenter();
	//CvPoint2D32f c = doc->cameramodel.coordsReal2Image(c3);
	//pDC->Ellipse(c.x-DOT, c.y-DOT, c.x+DOT, c.y+DOT);

	// TEMP
	//CRect sz;
	//pDC->GetWindow()->GetWindowRect(&sz);
	//for (int x=0;x<sz.Width();x++) {
	//	for (int y=0;y<sz.Height();y++) {
	//		CvPoint3D32f pR = doc->cameramodel.coordsImage2Real(cvPoint2D32f(x+ox, y+oy));
	//		if (IsRealPointIn(cvPoint2D32f(pR.x, pR.y)))
	//			pDC->SetPixel(x, y, RGB(200, 200, 55));
	//	}
	//}
	// END TEMP
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	delete pts;
	
	// find limits
	int leftMost = INT_MAX, rightMost = INT_MIN, center = 0;
	CvPoint im;
	for (int i=0;i<nPts;i++) {
		im = cvPointFrom32f(doc->cameramodel.coordsReal2Image(cvPoint3D32f(p[i].x, p[i].y, 0)));
		if (im.x < leftMost)
			leftMost = im.x;
		if (im.x > rightMost)
			rightMost = im.x;
		center += im.y;
	}
	center /= nPts;


	// draw label
	int TEXT_HEIGHT = 16;
	CRect rToolTip(leftMost-ox,center-TEXT_HEIGHT/2-oy, rightMost-ox, center+TEXT_HEIGHT/2-oy);
	drawStaticToolTip(pDC, label, rToolTip);
	return;
}
// --------------------------------------------------------------------------
void FloorRegion::DrawFloor(CDC* pDC, int ox, int oy, CPen* pen) {	
	CPen pNew(PS_SOLID, 1, RGB(0, 0, 255));
	CPen* pOldPen = pDC->SelectObject(&pNew);
	CBrush bNew(RGB(0, 255, 0));
	CBrush* pOldBrush = pDC->SelectObject(&bNew);
	int nPts = p.GetSize();
	CPoint *pts = new CPoint[nPts];
	for (int i=0;i<nPts;i++) {
		CvPoint pp = doc->floormodel.coordsReal2Floor(cvPoint3D32f(p[i].x, p[i].y, 0));
		pts[i] = CPoint((int)(pp.x - ox), (int)(pp.y - oy));
	}
	// Draw a smoothed poly-line
	pDC->Polyline(pts, nPts);
	pDC->MoveTo(pts[nPts-1]);
	pDC->LineTo(pts[0]);
	pDC->SelectObject(pOldPen);
	for (int i=0;i<nPts;i++) {
		pDC->Ellipse(pts[i].x-DOT, pts[i].y-DOT, pts[i].x+DOT, pts[i].y+DOT);
	}

	// Cleanup
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	delete pts;

	//// find limits
	//int leftMost = INT_MAX, rightMost = INT_MIN, center = 0;
	//CvPoint im;
	//for (int i=0;i<nPts;i++) {
	//	im = cvPointFrom32f(doc->cameramodel.coordsReal2Image(cvPoint3D32f(p[i].x, p[i].y, 0)));
	//	if (im.x < leftMost)
	//		leftMost = im.x;
	//	if (im.x > rightMost)
	//		rightMost = im.x;
	//	center += im.y;
	//}
	//center /= nPts;


	//// draw label
	//int TEXT_HEIGHT = 16;
	//CRect rToolTip(leftMost-ox,center-TEXT_HEIGHT/2-oy, rightMost-ox, center+TEXT_HEIGHT/2-oy);
	//drawStaticToolTip(pDC, label, rToolTip);
}
// --------------------------------------------------------------------------
void FloorRegion::DrawFloor(IplImage* frame, CvScalar color) {	
	int nPts = p.GetSize();
	CvPoint *pts = new CvPoint[nPts];
	for (int i=0;i<nPts;i++) {
		pts[i] = doc->floormodel.coordsReal2Floor(cvPoint3D32f(p[i].x, p[i].y, 0));
	}
	// Draw a smoothed poly-line
	cvPolyLine(frame, &pts, &nPts, 1, 1, color, 4, CV_AA);
	delete pts;
}
// --------------------------------------------------------------------------
void FloorRegion::RasterizeFrame(IplImage* frame, CvScalar color) {
}
// --------------------------------------------------------------------------
void FloorRegion::RasterizeFloor(IplImage* frame, CvScalar color) {
	int nPts = p.GetSize();
	CvPoint *pts = new CvPoint[nPts];
	for (int i=0;i<nPts;i++) {
		pts[i] = doc->floormodel.coordsReal2Floor(cvPoint3D32f(p[i].x, p[i].y, 0));
	}
	cvFillPoly(frame, &pts, &nPts, 1, color); // fill frame with color 
	delete pts;
}
// --------------------------------------------------------------------------
void FloorRegion::Serialize( CArchive& archive ) {
	if( archive.IsStoring() ) {
		archive << id << label << m_regionType << nInVertices;
	}
	else {
		archive >> id >> label >> m_regionType >> nInVertices;
	}
	p.Serialize(archive);
}
// --------------------------------------------------------------------------
int FloorRegion::FramePointIn(CvPoint point) {
	for (int j=0;j<p.GetCount();j++) {
		CvPoint2D32f p2 = doc->cameramodel.coordsReal2Image(cvPoint3D32f(p[j].x, p[j].y, 0));
		if (d(cvPointFrom32f(p2), point) <= DOT) {
			return j; 
		}
	}
	return -1;
}
// --------------------------------------------------------------------------
int FloorRegion::FloorPointIn(CvPoint point) {
	for (int j=0;j<p.GetCount();j++) {
		CvPoint p2 = doc->floormodel.coordsReal2Floor(cvPoint3D32f(p[j].x, p[j].y, 0));
		if (d(p2, point) <= DOT) {
			return j; 
		}
	}
	return -1;
}