#if !defined( ZONEMODEL_H )
#define ZONEMODEL_H

#pragma once
#include <afxtempl.h>
#include "Model.h"
#include "cv.h"      
#include "SmartPtrArray.h"

class FloorRegion;

class ZoneModel : public Model
{
DECLARE_SERIAL( ZoneModel )
public:
	int last_id;
	CMap<int, int, FloorRegion*, FloorRegion*> zone;
	ZoneModel(void);
	~ZoneModel(void);
	virtual void Serialize(CArchive& ar);
	bool IsRealPointIn(CvPoint2D32f p, int zoneID);
	int GetZone(CvPoint2D32f p);
	int GetClosestZone(CvPoint2D32f p);
	void DrawFrame(IplImage* frame, CvScalar color);
	void DrawFloor(IplImage* frame, CvScalar color);
	void DrawFrame(CDC* pDC, CPoint off);
	void DrawFloor(CDC* pDC, CPoint off);
	void RasterizeFloor(IplImage* frame, CvScalar color=CV_RGB(255,255,255));
};

#endif // !defined( ZONEMODEL_H )
