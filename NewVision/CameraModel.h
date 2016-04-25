#if !defined( CAMERAMODEL_H )
#define CAMERAMODEL_H
#pragma once
#include "cv.h" 
#include "highgui.h" 
#include "Model.h"
#include "Line.h"
#include "CameraModelXML.h"

enum CameraModelType {PROJECTION=0, SPHERICAL=1, XMLFILE=2};
// PROJECTION - perspective projection model
// SPHERICAL - unprojected map (or equirectangular projection) model
// XMLFILE - perspective projection model with distortion correction loaded from XML file 

class CameraModel : public Model, public CameraModelXML
{
	DECLARE_SERIAL(CameraModel)
	// all camera model parameters are in normalized X'Y' coordinates for numerical stability
	// X' = (X-w/2)/((w+h)/4)     Y' = (Y-h/2)/((w+h)/4)
	// where w and h are width and height of the image
public:
	CvMat *VX, *VY, *VZ, *V0, *P, *K; // vanishing points and projection matrix
	CvMat *hImage, *hReal, *pReal;  // image/world homogeneous and real coordinates
	CvMat *A2x2, *B2x1, *X2x1, *A3x3, *B3x1, *X3x1; // mascellaneous matrices
	// projection matrix P = K [R T]
	// K - intrinsic camera parameters
	// R - camera rotation
	// T - camera translation

	double m_lambda;
	double scale;
	int w, h;
	CArray<CLine> x, y, z, r;
	CArray<int> num;
	CString m_sourcename;
	double m_elevation; // camera elevation 
	double m_minBodyDistance, m_maxBodyDistance; // minimum and maximum body distance for this camera model
	int m_cameraType; // {PROJECTION=0, SPHERICAL=1}

	CameraModel(void);
	virtual ~CameraModel(void);
	virtual void Serialize(CArchive& ar);
	CvPoint2D32f coordsHomo2Image(CvMat* v);	
	void		 coordsImage2Homo(CvPoint2D32f p, CvMat* v);
	CvSize		 sizeHomo2Image(CvMat* v);	
	CvPoint2D32f coordsImage2Spheric(CvPoint2D32f p);
	CvPoint2D32f coordsSpheric2Image(CvPoint2D32f s);

	CvPoint2D32f coordsReal2Image(CvPoint3D32f p, double pRw=1);	
	CvPoint3D32f coordsImage2Real(CvPoint2D32f p, double z=0);	
	CvPoint3D32f coordsImage2Real(CvPoint2D32f p, CvPoint3D32f ref1, CvPoint3D32f ref2);	

	CvPoint	intersectVZ2Contour(CvPoint pt);
	double 	computeScanlineWeight(int y);
	double 	computeHeight(CvPoint2D32f top, CvPoint2D32f bottom);
	double 	computeX(CvPoint2D32f pIm, CvPoint3D32f pReal);
	double 	computeY(CvPoint2D32f pIm, CvPoint3D32f pReal);
	double 	computeZ(CvPoint2D32f pIm, CvPoint3D32f pReal);
	double 	coordsImage2RealSameXY_Feet2Floor(CvPoint2D32f top, CvPoint2D32f bot, CvPoint3D32f* head = NULL, CvPoint3D32f* foot=NULL);
	double 	coordsDistanceFromCamera(CvPoint3D32f p);
	void   	getImagePlane(CvPoint3D32f u, CvPoint3D32f v, CvPoint3D32f n);

	void Draw(IplImage* frame, CvScalar color);
	void DrawDistanceMap(IplImage* frame);
	void DrawVanishingPoints(IplImage* frame);
	void DrawVerticalSpheroid(IplImage* frame, CvPoint3D32f center, CvSize2D32f axes, CvScalar color, bool fill = false);
	void DrawVerticalSpheroid(CDC* pDC, CvPoint3D32f center, CvSize2D32f axes);
};	

#endif // !defined( CAMERAMODEL_H)