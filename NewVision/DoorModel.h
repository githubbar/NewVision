#if !defined( DOORMODEL_H )
#define DOORMODEL_H

#pragma once
#include <afxtempl.h>
#include "Model.h"
#include "cv.h"      


enum DoorModelType {DOORS_UNIFORM=0, DOORS_POINTS=1, DOORS_INTEREST_REGION=2, DOORS_INTEREST_BOUNDARY=3, DOORS_COUNTER=4};

class DoorModel : public Model
{
DECLARE_SERIAL( DoorModel )
public:

	static const int DOT = 3;
	// ------------- Persistant variables ---------------------
	CArray<CvPoint2D32f> door;
	int m_doorType; // {DOORS_UNIFORM=0, DOORS_POINTS=1}
	int nInVertices; // number of "in" vertices for DOORS_COUNTER model
	DoorModel(void);
	~DoorModel(void);
	virtual void Serialize(CArchive& ar);
	double GetDistanceFromClosestDoor(CvPoint3D32f p); 
	double GetDistanceFromClosestDoor(CvPoint2D32f p); 
	bool IsInsideInterestRegion(CvPoint2D32f foot);
	void DrawFrame(IplImage* frame, CvScalar color);
	void DrawFloor(IplImage* frame, CvScalar color);
	void DrawFrame(CDC* pDC, CPoint off);
	void Rasterize(IplImage* frame, CvScalar color=CV_RGB(255,255,255));
};

#endif // !defined( DOORMODEL_H )
