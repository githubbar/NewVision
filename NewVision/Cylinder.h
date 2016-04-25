
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
#if !defined( CYLINDER_H )
#define CYLINDER_H
#pragma once
#include <afxtempl.h>
#include "cv.h"  
#include "Object3D.h"  

class CNewVisionDoc;

// Cylinder is a Cylinder (parallelepiped) in image coordinates 
// representing static store fixtures on the floor that might occlude the object being tracked
// p1,p2,p3 represent the base of the fixture (z=0), while p4 gives the height
class Cylinder : public Obstacle {
	DECLARE_DYNAMIC(Cylinder)
public:
	CvPoint3D32f p1, p2; // base and top center points
	double R;
	Cylinder(void);
	Cylinder(CvPoint2D32f p1, CNewVisionDoc* doc);
	Cylinder(const Cylinder&);
	Cylinder& operator=(const Cylinder& that);
	~Cylinder(void);

	void GetOccludedFloorArea(CvSeq* points);
	CvPoint3D32f GetCenter();

	bool Move(CvPoint3D32f p);
	bool Move1(CvPoint2D32f p);
	bool Move2(CvPoint2D32f p);
	bool Move3(CvPoint2D32f p);
	bool Move4(CvPoint2D32f p);

	void DrawFloor(IplImage* frame, CvScalar color); 
	void DrawFloor(CDC* pDC, int ox, int oy, CPen* pen);
	void DrawFrame(IplImage* frame, CvScalar color);
	void DrawFrame(CDC* pDC , int ox, int oy, CPen* pen);
	
	virtual double GetDistanceFromCamera(); // distance from camera center in centimeters
	virtual void RasterizeFrame(IplImage* frame, CvScalar color=CV_RGB(255,255,255)); 
	virtual void RasterizeFloor(IplImage* frame, CvScalar color=CV_RGB(255,255,255)); 
	void Serialize( CArchive& archive );
	
	bool IsFramePointIn(CvPoint point, CvPoint3D32f& pcurrent);
	bool IsFloorPointIn(CvPoint point, CvPoint3D32f& pcurrent);
};
#endif // !defined( CYLINDER_H )