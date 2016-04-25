#include "StdAfx.h"
#include "DoorModel.h"
#include "NewVisionDoc.h"
#include "Globals.h"

IMPLEMENT_SERIAL(DoorModel, Model, 1)
// ------------------------------------------------------------------------
DoorModel::DoorModel(void)
{
}
// ------------------------------------------------------------------------
DoorModel::~DoorModel(void)
{
}
// ------------------------------------------------------------------------
void DoorModel::Serialize( CArchive& ar )
{
	if( ar.IsStoring()) {
		ar << m_doorType; // << nInVertices
	}
	else {
		ar >> m_doorType; // >> nInVertices
	}
	door.Serialize( ar );
}
// --------------------------------------------------------------------------
double DoorModel::GetDistanceFromClosestDoor(CvPoint3D32f p) {
	return GetDistanceFromClosestDoor(cvPoint2D32f(p.x, p.y));
}
// --------------------------------------------------------------------------
double DoorModel::GetDistanceFromClosestDoor(CvPoint2D32f p) {
	switch (m_doorType) {
		case DOORS_UNIFORM:
			return 1;
		case DOORS_POINTS: {
			// if no doors - the body is infinitely far from the doors
			if (door.GetSize() == 0)
				return DBL_MAX;
			double min_d = DBL_MAX;
			for (int i=0;i<door.GetSize();i++) {
				double dd = d(p, door[i]);
				if (dd < min_d)
					min_d = dd;
			}
			return min_d;
		}
		case DOORS_INTEREST_REGION: {
			if (IsInsideInterestRegion(doc->cameramodel.coordsReal2Image(cvPoint3D32f(p.x, p.y, 0))))
				return 1;
			else
				return DBL_MAX;
		}
		case DOORS_INTEREST_BOUNDARY: {
			// if no doors - the body is infinitely far from the doors
			if (door.GetSize() < 3)
				return DBL_MAX;
			
			double min_d = DBL_MAX;
			double ZERO_DISTANCE = 10;
			for (int i=0;i<door.GetSize()-1;i++) {
				// ignore if one of the doors is close to the 0,0 point (for panoramic model)
				if (d(door[i]) < ZERO_DISTANCE || d(door[i+1]) < ZERO_DISTANCE)
					continue;
				double dd = d2lineSegment(p, door[i], door[i+1]);
				if (dd < min_d)
					min_d = dd;
			}
			return min_d;
		}
		case DOORS_COUNTER: {
			if (IsInsideInterestRegion(doc->cameramodel.coordsReal2Image(cvPoint3D32f(p.x, p.y, 0))))
				return 1;
			else
				return DBL_MAX;
		}
		default:
			return 1;
	}
	return 1;
}
// ------------------------------------------------------------------------
bool DoorModel::IsInsideInterestRegion(CvPoint2D32f foot) {
	if (m_doorType == DOORS_INTEREST_REGION || m_doorType == DOORS_INTEREST_BOUNDARY || m_doorType == DOORS_COUNTER) {
		CvScalar p = cvGet2D(door_mask, (int)foot.y, (int)foot.x);
		if (p.val[0] != 0) 
			return true;
		else 
			return false;
	}
	else
		return true;
}
// ------------------------------------------------------------------------
void DoorModel::DrawFrame(IplImage* frame, CvScalar color) {
	switch (m_doorType) {
		case DOORS_UNIFORM:
			return;
		case DOORS_POINTS: {
			for (int i=0;i<door.GetSize();i++) {
				CvPoint point = cvPointFrom32f(doc->cameramodel.coordsReal2Image(cvPoint3D32f(door[i].x, door[i].y, 0)));
				cvCircle(frame, point, DOT, color, -1); 
			}
			return;
		}
		case DOORS_INTEREST_REGION: {
			int nPts = door.GetSize();
			CvPoint *pts = new CvPoint[nPts];
			for (int i=0;i<nPts;i++) {
				pts[i] = cvPointFrom32f(doc->cameramodel.coordsReal2Image(cvPoint3D32f(door[i].x, door[i].y, 0)));
			}
			// Draw a smoothed poly-line
			cvPolyLine(frame, &pts, &nPts, 1, 1, color, 1, CV_AA);
			delete pts;
			return;
		}
		case DOORS_INTEREST_BOUNDARY: {
			int nPts = door.GetSize();
			CvPoint *pts = new CvPoint[nPts];
			for (int i=0;i<nPts;i++) {
				pts[i] = cvPointFrom32f(doc->cameramodel.coordsReal2Image(cvPoint3D32f(door[i].x, door[i].y, 0)));
			}
			// Draw a smoothed poly-line
			cvPolyLine(frame, &pts, &nPts, 1, 1, color, 1, CV_AA);
			delete pts;
			return;
		}
		case DOORS_COUNTER: {
			// draw "in" vertices
			CvPoint *pts = new CvPoint[nInVertices];
			for (int i=0;i<nInVertices;i++) {
				pts[i] = cvPointFrom32f(doc->cameramodel.coordsReal2Image(cvPoint3D32f(door[i].x, door[i].y, 0)));
			}
			cvPolyLine(frame, &pts, &nInVertices, 1, 0, CV_RGB(0,255,0), 1, CV_AA);
			// draw "out" vertices
			delete pts;
			int nPts = door.GetSize()-nInVertices+1;
			pts = new CvPoint[nPts+1];
			for (int i=0;i<nPts;i++) {
				pts[i] = cvPointFrom32f(doc->cameramodel.coordsReal2Image(cvPoint3D32f(door[nInVertices+i-1].x, door[nInVertices+i-1].y, 0)));
			}
			pts[nPts] = cvPointFrom32f(doc->cameramodel.coordsReal2Image(cvPoint3D32f(door[0].x, door[0].y, 0)));
			cvPolyLine(frame, &pts, &nPts, 1, 0, color, 1, CV_AA);
			delete pts;
			return;
		}
		default:
			return;
	}
}
// ------------------------------------------------------------------------
void DoorModel::DrawFloor(IplImage* frame, CvScalar color) {
	switch (m_doorType) {
		case DOORS_UNIFORM:
			return;
		case DOORS_POINTS: {
			for (int i=0;i<door.GetSize();i++) {
				CvPoint point = doc->floormodel.coordsReal2Floor(cvPoint3D32f(door[i].x, door[i].y, 0));
				cvCircle(frame, point, 3, color, -1); 
			}
			return;
		}
		case DOORS_INTEREST_REGION: {
			int nPts = door.GetSize();
			CvPoint *pts = new CvPoint[nPts];
			for (int i=0;i<nPts;i++) {
				pts[i] = doc->floormodel.coordsReal2Floor(cvPoint3D32f(door[i].x, door[i].y, 0));
			}
			// Draw a smoothed poly-line
			cvPolyLine(frame, &pts, &nPts, 1, 1, color, 4, CV_AA);
			delete pts;
			return;
		}
		case DOORS_INTEREST_BOUNDARY: {
			int nPts = door.GetSize();
			CvPoint *pts = new CvPoint[nPts];
			for (int i=0;i<nPts;i++) {
				pts[i] = doc->floormodel.coordsReal2Floor(cvPoint3D32f(door[i].x, door[i].y, 0));
			}
			// Draw a smoothed poly-line
			cvPolyLine(frame, &pts, &nPts, 1, 1, color, 4, CV_AA);
			delete pts;
			return;
		}
		case DOORS_COUNTER: {
			// draw "in" vertices
			CvPoint *pts = new CvPoint[nInVertices];
			for (int i=0;i<nInVertices;i++) {
				pts[i] = doc->floormodel.coordsReal2Floor(cvPoint3D32f(door[i].x, door[i].y, 0));
			}
			cvPolyLine(frame, &pts, &nInVertices, 1, 0, CV_RGB(0,255,0), 1, CV_AA);
			// draw "out" vertices
			delete pts;
			int nPts = door.GetSize()-nInVertices+1;
			pts = new CvPoint[nPts+1];
			for (int i=0;i<nPts;i++) {
				pts[i] = doc->floormodel.coordsReal2Floor(cvPoint3D32f(door[nInVertices+i-1].x, door[nInVertices+i-1].y, 0));
			}
			pts[nPts] = doc->floormodel.coordsReal2Floor(cvPoint3D32f(door[0].x, door[0].y, 0));
			cvPolyLine(frame, &pts, &nPts, 1, 0, color, 1, CV_AA);
			delete pts;
			return;
		}
		default:
			return;
	}
}
// ------------------------------------------------------------------------
void DoorModel::DrawFrame(CDC* pDC, CPoint off) {
	switch (m_doorType) {
		case DOORS_UNIFORM:
			return;
		case DOORS_POINTS: {
			CBrush bNew(RGB(255, 255, 0));
			CBrush* pOldBrush = pDC->SelectObject(&bNew);
			for (int i=0;i<door.GetSize();i++) {
				CvPoint d = cvPointFrom32f(doc->cameramodel.coordsReal2Image(cvPoint3D32f(door[i].x, door[i].y, 0)));
				d.x -= off.x; d.y -= off.y;
				pDC->Ellipse(d.x-DOT, d.y-DOT, d.x+DOT, d.y+DOT);
			}
			pDC->SelectObject(pOldBrush);
			return;
		}
		case DOORS_INTEREST_REGION: {
			CPen pNew(PS_SOLID, 1, RGB(255, 0, 0));
			CPen* pOldPen = pDC->SelectObject(&pNew);
			int nPts = door.GetSize();
			CPoint *pts = new CPoint[nPts];
			for (int i=0;i<nPts;i++) {
				CvPoint2D32f pp = doc->cameramodel.coordsReal2Image(cvPoint3D32f(door[i].x, door[i].y, 0));
				pts[i] = CPoint((int)(pp.x - off.x), (int)(pp.y - off.y));
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
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldPen);
			cvZero(door_mask);
			Rasterize(door_mask, cvScalar(1));
			delete pts;
			return;
		}			
		case DOORS_INTEREST_BOUNDARY: {
			CPen pNew(PS_SOLID, 1, RGB(255, 0, 0));
			CPen* pOldPen = pDC->SelectObject(&pNew);
			int nPts = door.GetSize();
			CPoint *pts = new CPoint[nPts];
			for (int i=0;i<nPts;i++) {
				CvPoint2D32f pp = doc->cameramodel.coordsReal2Image(cvPoint3D32f(door[i].x, door[i].y, 0));
				pts[i] = CPoint((int)(pp.x - off.x), (int)(pp.y - off.y));
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
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldPen);
			cvZero(door_mask);
			Rasterize(door_mask, cvScalar(1));
			delete pts;
			return;
		}	
		case DOORS_COUNTER: {
			if (!nInVertices)
				return;

			CPen red(PS_SOLID, 1, RGB(255, 0, 0));
			CPen green(PS_SOLID, 1, RGB(0, 255, 0));
			// draw "in" vertices
			CPen* pOldPen = pDC->SelectObject(&green);
			CPoint *pts = new CPoint[nInVertices];
			for (int i=0;i<nInVertices;i++) {
				CvPoint2D32f pp = doc->cameramodel.coordsReal2Image(cvPoint3D32f(door[i].x, door[i].y, 0));
				pts[i] = CPoint((int)(pp.x - off.x), (int)(pp.y - off.y));
			}
			pDC->Polyline(pts, nInVertices);
			// draw "out" vertices
			pDC->SelectObject(&red);
			int nPts = door.GetSize()-nInVertices+1;
			delete pts;
			if (!nPts)
				return;
			pts = new CPoint[nPts];
			for (int i=0;i<nPts;i++) {
				CvPoint2D32f pp = doc->cameramodel.coordsReal2Image(cvPoint3D32f(door[nInVertices+i-1].x, door[nInVertices+i-1].y, 0));
				pts[i] = CPoint((int)(pp.x - off.x), (int)(pp.y - off.y));
			}
			pDC->Polyline(pts, nPts);
			pDC->MoveTo(pts[nPts-1]);
			pDC->LineTo(cPoint(doc->cameramodel.coordsReal2Image(cvPoint3D32f(door[0].x, door[0].y, 0))));
			pDC->SelectObject(pOldPen);
			CBrush bNew(RGB(0, 255, 0));
			CBrush* pOldBrush = pDC->SelectObject(&bNew);
			for (int i=0;i<nPts;i++) {
				pDC->Ellipse(pts[i].x-DOT, pts[i].y-DOT, pts[i].x+DOT, pts[i].y+DOT);
			}
			delete pts;
			
			// cleanup
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldPen);
			cvZero(door_mask);
			Rasterize(door_mask, cvScalar(1));
			return;
		}	
		default:
			return;
	}
}
// ------------------------------------------------------------------------
void DoorModel::Rasterize(IplImage* frame, CvScalar color) {
	switch (m_doorType) {
		case DOORS_UNIFORM:
			cvSet(frame, color);
		case DOORS_POINTS:
			cvSet(frame, color);
		case DOORS_INTEREST_REGION: {
			int nPts = door.GetSize();
			CvPoint *pts = new CvPoint[nPts];
			for (int i=0;i<nPts;i++) {
				pts[i] = cvPointFrom32f(doc->cameramodel.coordsReal2Image(cvPoint3D32f(door[i].x, door[i].y, 0)));
			}
			// Draw a smoothed poly-line
			cvFillPoly(frame, &pts, &nPts, 1, color);
			delete pts;
			return;
		}
		case DOORS_INTEREST_BOUNDARY: {
			int nPts = door.GetSize();
			CvPoint *pts = new CvPoint[nPts];
			for (int i=0;i<nPts;i++) {
				pts[i] = cvPointFrom32f(doc->cameramodel.coordsReal2Image(cvPoint3D32f(door[i].x, door[i].y, 0)));
			}
			// Draw a smoothed poly-line
			cvFillPoly(frame, &pts, &nPts, 1, color);
			delete pts;
			return;
		}
		case DOORS_COUNTER: {
			int nPts = door.GetSize();
			CvPoint *pts = new CvPoint[nPts];
			for (int i=0;i<nPts;i++) {
				pts[i] = cvPointFrom32f(doc->cameramodel.coordsReal2Image(cvPoint3D32f(door[i].x, door[i].y, 0)));
			}
			// Draw a smoothed poly-line
			cvFillPoly(frame, &pts, &nPts, 1, color);
			delete pts;
			return;
		}
		default:
			return;
	}
}