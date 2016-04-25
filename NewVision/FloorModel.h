#if !defined( FLOORMODEL_H )
#define FLOORMODEL_H
#pragma once
#include <afxtempl.h>
#include "Model.h"
#include "cv.h"      



class FloorModel : public Model
{
DECLARE_SERIAL( FloorModel )
public:

	// ------------- Persistant variables ---------------------
	CArray<CvPoint> m_extreme;
	int			    m_type;
	int             w, h; 
	CString         m_sourcename;
	CvPoint2D32f    floormin, floormax;

	FloorModel(void);
	~FloorModel(void);
	CvPoint coordsReal2Floor(CvPoint3D32f p);	
	CvPoint coordsReal2Floor(CvPoint2D32f p);	
	CvPoint3D32f coordsFloor2Real(CvPoint p);	
	CvSize sizeReal2Floor(CvSize2D32f s);
	void GetMinMaxXY(CvPoint2D32f& cammin, CvPoint2D32f& cammax);
	bool InBounds(CvPoint3D32f p);
	bool InBounds(CvPoint2D32f p);
	virtual void Serialize(CArchive& ar);
	void Draw(IplImage* frame, CvScalar color=CV_RGB(255,0,0));
	#if USE_X3DLIBRARY
		class X3DDriver;
		void ExportToX3D(X3DDriver &xd);
	#endif
};
template <> void AFXAPI SerializeElements <CvPoint> ( CArchive& ar, CvPoint* p, INT_PTR nCount );

#endif // !defined( FLOORMODEL_H )