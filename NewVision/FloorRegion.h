
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
#if !defined( FLOORREGION_H )
#define FLOORREGION_H
#pragma once
#include <afxtempl.h>
#include "cv.h"  
#include "Object3D.h" 

#define MAX_RADIUS 150

class FloorRegion : public Object3D {
public:
	CArray<CvPoint2D32f> p;
	enum RegionType {REGION_UNDIRECTIONAL=0, REGION_DIRECTIONAL=1, REGION_CIRCULAR=2};
	int m_regionType; // {REGION_UNDIRECTIONAL=0, REGION_DIRECTIONAL=1, REGION_CIRCULAR=2}
	int nInVertices; // number of "in" vertices for REGION_DIRECTIONAL model


	// Object3D overrides 
	virtual CvPoint3D32f    GetCenter();                                      // get 3D coordinates of object center        
	virtual double          GetDistanceFromCamera() {return 0;};                          // distance from camera center in centimeters
	virtual void            GetOccludedFloorArea(CvSeq* points) {};              // get a countor of occluded floor area
	virtual void            DrawFloor(IplImage* frame, CvScalar color);       // draw object floor projection image (OpenCV)
	virtual void            DrawFloor(CDC* pDC, int ox, int oy, CPen* pen);   // draw object floor projection image (Windows DC)
	virtual void            DrawFrame(IplImage* frame, CvScalar color);       // draw object frame projection image (OpenCV)
	virtual void            DrawFrame(CDC* pDC , int ox, int oy, CPen* pen);  // draw object frame projection image (Windows DC)
	virtual void            RasterizeFrame(IplImage* frame, CvScalar color);  // rasterize object frame projection 
	virtual void            RasterizeFloor(IplImage* frame, CvScalar color);  // rasterize object floor projection 
	virtual bool            IsFramePointIn(CvPoint point, CvPoint3D32f& pcurrent) {return false;};
	virtual bool            IsFloorPointIn(CvPoint point, CvPoint3D32f& pcurrent) {return false;};
	virtual int				FramePointIn(CvPoint point);
	virtual int				FloorPointIn(CvPoint point);
	virtual bool            Move(CvPoint3D32f p) {return false;};
	virtual bool			Move1(CvPoint2D32f p) {return false;};
	virtual bool            Move2(CvPoint2D32f p) {return false;};
	virtual bool            Move3(CvPoint2D32f p) {return false;};
	virtual bool            Move4(CvPoint2D32f p) {return false;};
	// -----------------

	FloorRegion(void);
	FloorRegion(int id, CNewVisionDoc* doc);
	FloorRegion(CvPoint2D32f p, CNewVisionDoc* doc);
	FloorRegion(const FloorRegion&);
	FloorRegion& operator=(const FloorRegion& that);
	~FloorRegion(void);
	virtual bool IsRealPointIn(CvPoint2D32f point);
	virtual void AddBorder(int w);
	virtual void Serialize(CArchive& ar);
	//virtual double DistanceFrom(CvPoint3D32f p); 
	//virtual double DistanceFrom(CvPoint2D32f p); 
};
template <> void AFXAPI SerializeElements <CvPoint2D32f> ( CArchive& ar, CvPoint2D32f* p, INT_PTR nCount );
template <> void AFXAPI SerializeElements <FloorRegion*> ( CArchive& ar, FloorRegion** p, INT_PTR nCount );
#endif // !defined( FLOORREGION_H )