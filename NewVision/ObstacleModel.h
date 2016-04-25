#pragma once

#include "cv.h" 
#include "highgui.h" 
#include "Model.h"
#include "Line.h"
#include "SmartPtrArray.h"

class Object3D;
class ObstacleModel : public Model
{
	DECLARE_SERIAL(ObstacleModel)
public:
	int last_id;
	SmartPtrArray<Object3D> obstacle;
	ObstacleModel(void);
	~ObstacleModel(void);
	virtual void Serialize(CArchive& ar);
	void DrawFrame(CDC* pDC , int ox, int oy, CPen* pen);
	void DrawFloor(CDC* pDC , int ox, int oy, CPen* pen);
	void DrawFrame(IplImage* frame, CvScalar color);
	void DrawFloor(IplImage* frame, CvScalar color);
	void Rasterize(IplImage* frame, CvScalar color=cvScalar(1));
	void RasterizeFloor(IplImage* frame, CvScalar color=cvScalar(1));
	Object3D* GetClosestObstacle(CvPoint2D32f p, double maxDistance=DBL_MAX);
#if USE_X3DLIBRARY
	class X3DDriver;
	void ExportToX3D(X3DDriver &xd);
#endif
	
};
