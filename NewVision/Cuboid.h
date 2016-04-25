
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
#if !defined( CUBOID_H )
#define CUBOID_H
#pragma once
#include <afxtempl.h>
#include "cv.h"  
#include "Object3D.h"  

class CNewVisionDoc;

// Cuboid is a cuboid (paralellopiped) in image coordinates 
// representing static store fixtures on the floor that might occlude the object being tracked
// p1,p2,p3 represent the base of the fixture (z=0), while p4 gives the height
class Cuboid : public Obstacle {
	DECLARE_DYNAMIC(Cuboid)
public:
	// local pointers to external structures
	static const int NPTS = 8;
	CvPoint3D32f v[NPTS]; // contains all 8 cuboid vertices for speed considerations
	Cuboid(void);
	Cuboid(CvPoint2D32f p1, CNewVisionDoc* doc);
	Cuboid(const Cuboid&);
	Cuboid& operator=(const Cuboid& that);
	~Cuboid(void);

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
#endif // !defined( CUBOID_H )