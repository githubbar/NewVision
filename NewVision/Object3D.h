
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
#if !defined( OBJECT3D_H )
#define OBJECT3D_H
#pragma once

//class Comparable {
//	virtual static int Compare(Comparable* one, Comparable* two);
//};
#include "cv.h"    

class CameraModel;
class CNewVisionDoc;

class Object3D : public CObject {
DECLARE_DYNAMIC(Object3D)
public:
	static const int DOT = 5;

	// Data fields
	int             depthZ;
	int				id;
	CString         label;
	
	// Local pointers to external structures
	CNewVisionDoc*  doc;


	virtual CvPoint3D32f    GetCenter() = 0;                                      // get 3D coordinates of object center        
	virtual double          GetDistanceFromCamera() = 0;                          // distance from camera center in centimeters
	virtual void            GetOccludedFloorArea(CvSeq* points) = 0;              // get a countor of occluded floor area
	virtual void            DrawFloor(IplImage* frame, CvScalar color) = 0;       // draw object floor projection image (OpenCV)
	virtual void            DrawFloor(CDC* pDC, int ox, int oy, CPen* pen) = 0;   // draw object floor projection image (Windows DC)
	virtual void            DrawFrame(IplImage* frame, CvScalar color) = 0;       // draw object frame projection image (OpenCV)
	virtual void            DrawFrame(CDC* pDC , int ox, int oy, CPen* pen) = 0;  // draw object frame projection image (Windows DC)
	virtual void            RasterizeFrame(IplImage* frame, CvScalar color) = 0;  // rasterize object frame projection 
	virtual void            RasterizeFloor(IplImage* frame, CvScalar color) = 0;  // rasterize object floor projection 
	virtual bool            IsFramePointIn(CvPoint point, CvPoint3D32f& pcurrent) = 0; // pcurrent: the vertex the point is the closest to
	virtual bool            IsFloorPointIn(CvPoint point, CvPoint3D32f& pcurrent) = 0; 
	virtual bool            Move(CvPoint3D32f p) = 0;
	virtual bool			Move1(CvPoint2D32f p) = 0;
	virtual bool            Move2(CvPoint2D32f p) = 0;
	virtual bool            Move3(CvPoint2D32f p) = 0;
	virtual bool            Move4(CvPoint2D32f p) = 0;
	virtual int             Compare(Object3D* that) {
		double diff = this->GetDistanceFromCamera() - that->GetDistanceFromCamera();
		if (diff > 0)
			return 1;
		else if (diff == 0)
			return 0;
		else 
			return -1;
	}; // return 1 if this is further; 0 if the same distance; -1 otherwise
};

class Obstacle : public Object3D {
	DECLARE_DYNAMIC(Obstacle)
};
// --------------------------------------------------------------------------
template <> void AFXAPI SerializeElements <Object3D*> ( CArchive& ar, Object3D** p, INT_PTR nCount );
// --------------------------------------------------------------------------

#endif // !defined( OBJECT3D_H )