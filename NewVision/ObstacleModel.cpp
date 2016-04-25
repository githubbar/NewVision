#include "StdAfx.h"
#include "Globals.h"
#include "ObstacleModel.h"
#include "Object3D.h"
#include "NewVisionDoc.h"
#include "NewVisionView.h"
#include "Cuboid.h"
#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::Diagnostics;

IMPLEMENT_SERIAL(ObstacleModel, CObject, 1)

// ------------------------------------------------------------------------
ObstacleModel::ObstacleModel(void)
{
	last_id = 0;
}
// ------------------------------------------------------------------------
ObstacleModel::~ObstacleModel(void)
{
}
// ------------------------------------------------------------------------
void ObstacleModel::Serialize( CArchive& ar )
{
	CObject::Serialize(ar);
	obstacle.Serialize( ar );
	if (ar.IsLoading()) 
		for (int i=0; i<obstacle.GetCount(); i++) {
			obstacle[i]->doc = doc;
			obstacle[i]->id = last_id++;
		}
}
// ------------------------------------------------------------------------
Object3D* ObstacleModel::GetClosestObstacle(CvPoint2D32f p, double maxDistance) {
	double min_dist = INT_MAX;
	Object3D *obst = NULL;
	for (int i=0; i < obstacle.GetCount(); i++) {
		CvPoint3D32f p3 = obstacle[i]->GetCenter();
		double dist = d(cvPoint2D32f(p3.x, p3.y), p);
		if (dist < min_dist) {
			min_dist = dist;
			obst = obstacle[i];
		}
	}
	if (min_dist < maxDistance)
		return obst;
	else 
		return NULL;
}
// ------------------------------------------------------------------------
void ObstacleModel::DrawFrame(IplImage* frame, CvScalar color) {
}
// ------------------------------------------------------------------------
void ObstacleModel::DrawFrame(CDC* pDC , int ox, int oy, CPen* pen) {
	for (int i=0;i<obstacle.GetSize();i++) {
		obstacle[i]->DrawFrame(pDC, ox, oy, pen);
	}
}
// ------------------------------------------------------------------------
void ObstacleModel::DrawFloor(CDC* pDC , int ox, int oy, CPen* pen) {
	for (int i=0;i<obstacle.GetSize();i++) {
		obstacle[i]->DrawFloor(pDC, ox, oy, pen);
	}
}
// ------------------------------------------------------------------------
void ObstacleModel::DrawFloor(IplImage* frame, CvScalar color) {
	for (int i=0;i<obstacle.GetSize();i++) {
		obstacle[i]->DrawFloor(frame, color);
	}
}
// ------------------------------------------------------------------------
void ObstacleModel::Rasterize(IplImage* frame, CvScalar color) {
	for (int i=0;i<obstacle.GetSize();i++) {
		obstacle[i]->RasterizeFrame(frame, color);
	}
}
// ------------------------------------------------------------------------
void ObstacleModel::RasterizeFloor(IplImage* frame, CvScalar color) {
	for (int i=0;i<obstacle.GetSize();i++) {
		obstacle[i]->RasterizeFloor(frame, color);
	}
}
// ------------------------------------------------------------------------
#if USE_X3DLIBRARY
#include "X3DDriver.h"
void ObstacleModel::ExportToX3D( X3DDriver &xd )
{

	// --------- Add obstacles
	for (int i=0;i < doc->obstaclemodel.obstacle.GetCount();i++) {
		CString coords="", normals="";
		// x <-> y to convert from left-coordinate system to right coordinate system
		Cuboid *box = (Cuboid*)obstacle[i];
		// point='-1 -1 1, 1 -1 1, 1 1 1, -1 1 1, 1 1 -1, -1 1 -1, -1 -1 -1, 1 -1 -1'/>
		coords.Format("%.2f %.2f %.2f, %.2f %.2f %.2f, %.2f %.2f %.2f, %.2f %.2f %.2f, %.2f %.2f %.2f, %.2f %.2f %.2f, %.2f %.2f %.2f, %.2f %.2f %.2f", 
			box->v[0].y, box->v[0].x, box->v[0].z,
			box->v[1].y, box->v[1].x, box->v[1].z,
			box->v[2].y, box->v[2].x, box->v[2].z,
			box->v[3].y, box->v[3].x, box->v[3].z,
			box->v[4].y, box->v[4].x, box->v[4].z,
			box->v[5].y, box->v[5].x, box->v[5].z,
			box->v[6].y, box->v[6].x, box->v[6].z,
			box->v[7].y, box->v[7].x, box->v[7].z);
		xd.AddObstacle(box->id, coords, box->label);
	}
}
#endif
// ------------------------------------------------------------------------

