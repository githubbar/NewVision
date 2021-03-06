#include "StdAfx.h"
#include "cameramodel.h"
#include "Globals.h"
#include "NewVisionDoc.h"
#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::Diagnostics;

IMPLEMENT_SERIAL(CameraModel, CObject, 1)
// ------------------------------------------------------------------------
CameraModel::CameraModel(void)
{
	m_lambda = 1;
	VX = cvCreateMat(3, 1, CV_64FC1);
	VY = cvCreateMat(3, 1, CV_64FC1);
	VZ = cvCreateMat(3, 1, CV_64FC1);
	V0 = cvCreateMat(3, 1, CV_64FC1);
	K = cvCreateMat(3, 3, CV_64FC1);
	P = cvCreateMat(3, 4, CV_64FC1);
	hImage = cvCreateMat(3, 1, CV_64FC1); 
	hReal  = cvCreateMat(4, 1, CV_64FC1); 
	pReal = cvCreateMat(3, 1, CV_64FC1); 
	A2x2 = cvCreateMat(2, 2, CV_64FC1); 
	B2x1 = cvCreateMat(2, 1, CV_64FC1); 
	X2x1 = cvCreateMat(2, 1, CV_64FC1); 
	A3x3 = cvCreateMat(3, 3, CV_64FC1); 
	B3x1 = cvCreateMat(3, 1, CV_64FC1); 
	X3x1 = cvCreateMat(3, 1, CV_64FC1); 
}
// ------------------------------------------------------------------------
CameraModel::~CameraModel(void)
{
	cvReleaseMat(&VX);cvReleaseMat(&VY);cvReleaseMat(&VZ);cvReleaseMat(&V0);cvReleaseMat(&P);
	cvReleaseMat(&hImage);cvReleaseMat(&hReal);cvReleaseMat(&pReal);
	cvReleaseMat(&A2x2); cvReleaseMat(&B2x1); cvReleaseMat(&X2x1);
	cvReleaseMat(&A3x3); cvReleaseMat(&B3x1); cvReleaseMat(&X3x1);
}
// ------------------------------------------------------------------------
void CameraModel::Serialize( CArchive& ar )
{
	// Use versionable schema for reading multiple versions of .proj files
	// IMPLEMENT_SERIAL(CMyObject, CObject, VERSIONABLE_SCHEMA|1)
	// void CMyObject::Serialize(CArchive& ar) 
	//   if (ar.IsLoading())
	//       int nVersion = ar.GetObjectSchema();
	if( ar.IsStoring()) {
		ar << m_cameraType;
	}
	else {
		ar >> m_cameraType;
	}

	switch (m_cameraType) {
		case XMLFILE: {
			if( ar.IsStoring()) {
				ar << m_sourcename;
			}
			else {
				ar >> m_sourcename;
			}
			break;
		  }
		default: {

			if( ar.IsStoring()) {
				ar << m_elevation;
			}
			else {
				ar >> m_elevation;
			}
			x.Serialize(ar);
			y.Serialize(ar);
			z.Serialize(ar);
			r.Serialize(ar);
			num.Serialize(ar);
		}
	}
	CObject::Serialize(ar);
}
// ------------------------------------------------------------------------
CvSize CameraModel::sizeHomo2Image(CvMat* v )
{
	switch (m_cameraType) {
		case XMLFILE: 
		case PROJECTION: {
			// v CvSize be 3x1 matrix
			CvSize p = cvSize(0,0);
			double s = cvmGet(v, 2, 0); 

			p.width = (float)(cvmGet(v, 0, 0)/s);
			p.height = (float)(cvmGet(v, 1, 0)/s);
			return p;
		 }
	}
	return cvSize(0,0);
}
// ------------------------------------------------------------------------
CvPoint2D32f CameraModel::coordsHomo2Image(CvMat* v) {
	// v must be 3x1 matrix
	CvPoint2D32f p = cvPoint2D32f(0,0);
	double s = cvmGet(v, 2, 0); 

	switch (m_cameraType) {
		case XMLFILE:
		case PROJECTION: {
			p.x = (float)(cvmGet(v, 0, 0)/s);
			p.y = (float)(cvmGet(v, 1, 0)/s);
			return p;
		}
	}
	return cvPoint2D32f(0,0);
}
// ------------------------------------------------------------------------
void CameraModel::coordsImage2Homo(CvPoint2D32f p, CvMat* v) {
	// v must be 3x1 matrix
	switch (m_cameraType) {
		case PROJECTION:
		case XMLFILE: {
			cvmSet(v, 0, 0, p.x);
			cvmSet(v, 1, 0, p.y);
			cvmSet(v, 2, 0, 1);
			return;
		}
	}
}
// ------------------------------------------------------------------------
CvPoint2D32f CameraModel::coordsImage2Spheric(CvPoint2D32f p) {
    // returns latitude and longitude [phi, theta] in spherical coordinates from [x,y] - pixel location
	// assumptions:
	// (1) latitude is mapped linearly along y axis with y = 0 (topmost horizontal line) 
	// corresponding to phi = PI/2 (the equator) and y = h-1 (bottommost horizontal line)
	// corresponding to phi = PI (the south pole)
	// (2) longitude is mapped linearly along x axis with x = 0 (leftmost vertical line) 
	// corresponding to theta = 0 and x = w-1 (rightmost vertical line) corresponding to 
	// theta = 2*PI*(1-1/(w-1)) (2*PI itself corresponds to x = 0)
	CvPoint2D32f s;
	s.x = (float)(PI/2 + p.y*(PI/2)/(h-1));
	s.y = (float)(p.x*(2*PI)/w);
	return s;
}
// ------------------------------------------------------------------------
CvPoint2D32f CameraModel::coordsSpheric2Image(CvPoint2D32f s) {
    // returns latitude and longitude [phi, theta] in spherical coordinates from [x,y] - pixel location
	CvPoint2D32f p;
	p.x = (float)((s.y*w)/(2*PI));
	p.y = (float)((s.x-PI/2)*(h-1)/(PI/2));
	ASSERT(p.x != w && p.y != h);
	return p;
}
// ------------------------------------------------------------------------
CvPoint2D32f CameraModel::coordsReal2Image(CvPoint3D32f pR, double pRw) {
	switch (m_cameraType) {
		case XMLFILE:
		case PROJECTION: {
			// solve Ax=B using SVD
			cvSetReal2D(hReal, 0, 0, pR.x);
			cvSetReal2D(hReal, 1, 0, pR.y);
			cvSetReal2D(hReal, 2, 0, pR.z);
			cvSetReal2D(hReal, 3, 0, pRw);
			cvMatMul(P, hReal, hImage);
			CvPoint2D32f pImage = coordsHomo2Image(hImage); 
			return pImage;
		}
		case SPHERICAL: {
			// spherical camera is at [0,0,m_elevation] in world coordinates looking down at [0,0,0]
			CvPoint2D32f s; 
			ASSERT(!Double::IsNaN(pR.x));
			ASSERT(!Double::IsNaN(pR.y));
			
			s.y = (float)(atan(pR.y/pR.x));
 			if (pR.x <= 0) s.y += (float)(PI);   // 2nd and 3rd quadrangle
			if (pR.x > 0 && pR.y < 0) s.y += (float)(2*PI); // 4th quadrangle
			s.x = (float)(atan2(m_elevation-pR.z, d(cvPoint2D32f(pR.x, pR.y)))+PI/2);
			s.y = abs(s.y);
			CvPoint2D32f pImage = coordsSpheric2Image(s);
			return pImage;	
		}
	}
	return cvPoint2D32f(0,0);
}
// ------------------------------------------------------------------------
CvPoint3D32f CameraModel::coordsImage2Real(CvPoint2D32f p, double z) {
	CvPoint3D32f p3D = cvPoint3D32f( 0, 0, z );
	switch (m_cameraType) {
		case XMLFILE:
		case PROJECTION: {
			coordsImage2Homo(p, hImage);
			// solve [sx, sy, s] = P*[X, Y, Z, 1] for 'X' and 'Y', using SVD
			// substitute s = p31*X+p32*Y+p33*Z+p34
			cvmSet(A2x2, 0, 0, cvmGet(hImage,0,0)*cvmGet(P,2,0)-cvmGet(P,0,0));
			cvmSet(A2x2, 0, 1, cvmGet(hImage,0,0)*cvmGet(P,2,1)-cvmGet(P,0,1));
			cvmSet(A2x2, 1, 0, cvmGet(hImage,1,0)*cvmGet(P,2,0)-cvmGet(P,1,0));
			cvmSet(A2x2, 1, 1, cvmGet(hImage,1,0)*cvmGet(P,2,1)-cvmGet(P,1,1));
			cvmSet(B2x1, 0, 0, (cvmGet(P,0,2)-cvmGet(hImage,0,0)*cvmGet(P,2,2))*z+cvmGet(P,0,3)-cvmGet(P,2,3)*cvmGet(hImage,0,0));
			cvmSet(B2x1, 1, 0, (cvmGet(P,1,2)-cvmGet(hImage,1,0)*cvmGet(P,2,2))*z+cvmGet(P,1,3)-cvmGet(P,2,3)*cvmGet(hImage,1,0));
			cvSolve(A2x2, B2x1, X2x1);
			p3D.x = (float)(cvGetReal2D(X2x1,0,0));
			p3D.y = (float)(cvGetReal2D(X2x1,1,0));
			break;
		}
		case SPHERICAL: {
			CvPoint2D32f s = coordsImage2Spheric(p);
			p3D.x = (float)(cos(s.y)*tan(PI-s.x)*(m_elevation-z));
			p3D.y = (float)(sin(s.y)*tan(PI-s.x)*(m_elevation-z));
			ASSERT(!Double::IsNaN(p3D.x));
			ASSERT(!Double::IsNaN(p3D.y));
		}
	}
	return p3D;
}
// ------------------------------------------------------------------------
CvPoint3D32f CameraModel::coordsImage2Real(CvPoint2D32f p, CvPoint3D32f ref1, CvPoint3D32f ref2) {
	// compute point world coordinates, given its image coordinates
	// and two two world points on the same plane with it and differing only in Z coordinate 
	// (i.e. top and bottom poles of the ellipsoid)
	// must hold ref1.z < ref2.z
	// NOTE: dead case - if p1 == p2 
	CvPoint3D32f p3D;
	CvPoint2D32f p1 = coordsReal2Image(ref1);
	CvPoint2D32f p2 = coordsReal2Image(ref2);
	if (p1.x == p2.x && p1.y == p2.y) {
		ASSERT(false);
		return ref1;
	}
	// find Z world coordinate for p
	double Z = 0;
	switch (m_cameraType) {
		case XMLFILE: 
		case PROJECTION: {
			// pick 3rd point to designate a plane on the line prependicular to [p1, p2] 100 pixels awayf from it 
			// originating in the midline between p1, p2
			double p1p2 = d(p1, p2);
			CvPoint2D32f p3; 
			p3.x = (float)((p1.x+p2.x)/2 + (p1.y-p2.y)*100/p1p2);
			p3.y = (float)((p1.y+p2.y)/2 + (p1.x-p2.x)*100/p1p2);
			CvPoint3D32f ref3 = coordsImage2Real(p3, (ref1.z + ref2.z)/2);

			// find the intersection of projection ray with a plane [p1,p2,p3]
			coordsImage2Homo(p, hImage);
			// solve [sx, sy, s] = P*[X, Y, Z, 1] using SVD
			// substitue s = p31*X+p32*Y+p33*Z+p34
			// det(p,p1,p2,p3) = 0   // equation of a plane
			cvmSet(A3x3, 0, 0, cvmGet(hImage,0,0)*cvmGet(P,2,0)-cvmGet(P,0,0));
			cvmSet(A3x3, 0, 1, cvmGet(hImage,0,0)*cvmGet(P,2,1)-cvmGet(P,0,1));
			cvmSet(A3x3, 0, 2, cvmGet(hImage,0,0)*cvmGet(P,2,2)-cvmGet(P,0,2));
			cvmSet(A3x3, 1, 0, cvmGet(hImage,1,0)*cvmGet(P,2,0)-cvmGet(P,1,0));
			cvmSet(A3x3, 1, 1, cvmGet(hImage,1,0)*cvmGet(P,2,1)-cvmGet(P,1,1));
			cvmSet(A3x3, 1, 2, cvmGet(hImage,1,0)*cvmGet(P,2,2)-cvmGet(P,1,2));
			cvmSet(A3x3, 2, 0, (ref2.y-ref1.y)*(ref3.z-ref1.z)-(ref3.y-ref1.y)*(ref3.z-ref1.z));
			cvmSet(A3x3, 2, 1, (ref2.z-ref1.z)*(ref3.x-ref1.x)-(ref3.z-ref1.z)*(ref3.x-ref1.x));
			cvmSet(A3x3, 2, 2, (ref2.x-ref1.x)*(ref3.y-ref1.y)-(ref3.x-ref1.x)*(ref3.y-ref1.y));
			
			cvmSet(B3x1, 0, 0, cvmGet(P,0,3)-cvmGet(P,2,3)*cvmGet(hImage,0,0));
			cvmSet(B3x1, 1, 0, cvmGet(P,1,3)-cvmGet(P,2,3)*cvmGet(hImage,1,0));
			cvmSet(B3x1, 2, 0, 
				ref1.x*((ref2.y-ref1.y)*(ref3.z-ref1.z)-(ref3.y-ref1.y)*(ref3.z-ref1.z))+
				ref1.y*((ref2.z-ref1.z)*(ref3.x-ref1.x)-(ref3.z-ref1.z)*(ref3.x-ref1.x))+
				ref1.z*((ref2.x-ref1.x)*(ref3.y-ref1.y)-(ref3.x-ref1.x)*(ref3.y-ref1.y))
				);
			
			cvSolve(A3x3, B3x1, X3x1);
			p3D.x = (float)(cvGetReal2D(X3x1,0,0));
			p3D.y = (float)(cvGetReal2D(X3x1,1,0));
			p3D.z = (float)(cvGetReal2D(X3x1,2,0));
			return p3D;
		}
		case SPHERICAL: {
			Z = (p1.y-p.y)* (ref2.z - ref1.z)/(p1.y - p2.y) + ref1.z;
			p3D = coordsImage2Real(p, Z);
		}
	}
	return p3D;
}
// ------------------------------------------------------------------------
double CameraModel::computeX(CvPoint2D32f pIm, CvPoint3D32f pReal) {
	// compute X of image point, knowing Y and Z in real coordinates of the point on the same X-line
	CvPoint3D32f pIm2Real = coordsImage2Real(pIm, pReal.z);
	return pIm2Real.x;
}
// ------------------------------------------------------------------------
double CameraModel::computeY(CvPoint2D32f pIm, CvPoint3D32f pReal) {
	// compute Y of image point, knowing X and Z in real coordinates of the point on the same Y-line
	CvPoint3D32f pIm2Real = coordsImage2Real(pIm, pReal.z);
	return pIm2Real.y;
}
// ------------------------------------------------------------------------
double CameraModel::computeZ(CvPoint2D32f pIm, CvPoint3D32f pRef) {
	// compute Z of image point 'pIm', knowing X and Y world coordinates of the point 'pRef' on the same Z-line
	switch (m_cameraType) {
		case XMLFILE: 
		case PROJECTION: {
			coordsImage2Homo(pIm, hImage);
			cvmSet(pReal, 0, 0, pRef.x); cvmSet(pReal, 1, 0, pRef.y);

			CvMat *A2x1 = cvCreateMat(2, 1, CV_64FC1); 
			CvMat *X1x1 = cvCreateMat(1, 1, CV_64FC1); 

			// solve [sx, sy, sz] = P*[X, Y, Z, 1] for 'Z', using SVD
			// from eq.3, substitute s = (p31*X+p32*Y+p33*Z+p34)/z
			cvmSet(A2x1, 0, 0, cvmGet(P,2,2)*cvmGet(hImage,0,0)/cvmGet(hImage,2,0) - cvmGet(P,0,2));
			cvmSet(A2x1, 1, 0, cvmGet(P,2,2)*cvmGet(hImage,1,0)/cvmGet(hImage,2,0) - cvmGet(P,1,2));

			cvmSet(B2x1, 0, 0, (cvmGet(P,0,0)-cvmGet(P,2,0)*cvmGet(hImage,0,0)/cvmGet(hImage,2,0))*cvmGet(pReal,0,0)
							+ (cvmGet(P,0,1)-cvmGet(P,2,1)*cvmGet(hImage,0,0)/cvmGet(hImage,2,0))*cvmGet(pReal,1,0)
							+ cvmGet(P,0,3)-cvmGet(P,2,3)*cvmGet(hImage,0,0)/cvmGet(hImage,2,0));


			cvmSet(B2x1, 1, 0, (cvmGet(P,1,0)-cvmGet(P,2,0)*cvmGet(hImage,1,0)/cvmGet(hImage,2,0))*cvmGet(pReal,0,0)
							+(cvmGet(P,1,1)-cvmGet(P,2,1)*cvmGet(hImage,1,0)/cvmGet(hImage,2,0))*cvmGet(pReal,1,0)
							+ cvmGet(P,1,3)-cvmGet(P,2,3)*cvmGet(hImage,1,0)/cvmGet(hImage,2,0));
			cvSolve(A2x1, B2x1, X1x1, CV_SVD);
			double zz = (float)(cvGetReal2D(X1x1,0,0)); 
			cvReleaseMat(&A2x1);
			cvReleaseMat(&X1x1);
			return zz;


		}
		case SPHERICAL: {
			// TODO: small bug here when the point reach a singularity directly below the spherical camera
			CvPoint2D32f pReal2Im = coordsReal2Image(pRef);
			pIm.x = pReal2Im.x;
			// out of two homothetic triangles catets proportion
			CvPoint3D32f head_f = coordsImage2Real(pIm, pRef.z);
			double dHead = d(head_f, cvPoint3D32f(0,0,head_f.z));
			double dReal = d(pRef, cvPoint3D32f(0,0,pRef.z));
			return pRef.z+(m_elevation-pRef.z)*(dHead - dReal)/dHead;
		}
	}
	return 0; 
}
// ------------------------------------------------------------------------
double CameraModel::computeScanlineWeight(int y) {
	double SMALL_INT = 0.001;
	switch (doc->cameramodel.m_cameraType) {
		case XMLFILE:
		case PROJECTION: {
 			double distY = coordsDistanceFromCamera(coordsImage2Real(cvPoint2D32f(w/2, y), doc->bodymodel.m_heightM/2));
			double distFAR = coordsDistanceFromCamera(coordsImage2Real(cvPoint2D32f(w/2, 0), doc->bodymodel.m_heightM/2));
			return distY/distFAR+SMALL_INT; 
		}
		case SPHERICAL:
			return cos(PI/2*y/(doc->cameramodel.h-1))+SMALL_INT; // normalized by circle area size the scanline covers;
	}
	return 0;
}
// ------------------------------------------------------------------------
double CameraModel::computeHeight(CvPoint2D32f top, CvPoint2D32f bottom) {
    return coordsImage2RealSameXY_Feet2Floor(top, bottom);
}
// ------------------------------------------------------------------------
double CameraModel::coordsImage2RealSameXY_Feet2Floor(CvPoint2D32f top, CvPoint2D32f bot, CvPoint3D32f* head, CvPoint3D32f* foot) {
	switch (m_cameraType) {
		case XMLFILE: 
		case PROJECTION: {
			CvPoint3D32f bot3D = coordsImage2Real(bot);	
			CvPoint3D32f top3D = bot3D;
			top3D.z = (float)computeZ(top, bot3D);
			if (head) {
				*head = top3D;
			}
			if (foot) {
				*foot = bot3D;
			}
			return top3D.z;
		}
		case SPHERICAL: {
			// foot and head have the same longtitude in spherical coordinates
			// only the lattitude and the radius are different
			CvPoint3D32f foot_f = coordsImage2Real(bot, 0);
			// get head projection on the floor along the radial ray
			CvPoint3D32f head_f = coordsImage2Real(top, 0);
			float h0 = (float)(m_elevation*(d(head_f)-d(foot_f))/d(head_f));
			if (foot) {
				foot->x = foot_f.x; foot->y = foot_f.y; foot->z = foot_f.z;
			}
			if (head) {
				head->x = foot_f.x; head->y = foot_f.y; head->z = h0;
			}
			return (int)h0;
		}
	}
	return -1;
}
// ------------------------------------------------------------------------
double CameraModel::coordsDistanceFromCamera(CvPoint3D32f p) {
	switch (m_cameraType) {
		case PROJECTION: {
			return d(p, cvPoint3D32f(0, 0, m_elevation)); 
		}
		case SPHERICAL: {
			return d(p, cvPoint3D32f(0, 0, m_elevation)); 
		}
		case XMLFILE: {
			return d(p, cvPoint3D32f(mCposx, mCposy, mCposz)); 
		}
		default: 
			return 0;
	}
}
// ------------------------------------------------------------------------
void CameraModel::Draw(IplImage* frame, CvScalar color) {
	switch (m_cameraType) {
		case XMLFILE:
		case PROJECTION: {
			double s = 1;
			CvPoint3D32f C = cvPoint3D32f(
				cvmGet(doc->cameramodel.P, 2,0),
				cvmGet(doc->cameramodel.P, 2,1), 
				cvmGet(doc->cameramodel.P, 2,2)
			);
			CvPoint2D32f principal = coordsReal2Image(C, 0);
			cvCircle(frame, cvPointFrom32f(principal), 3, CV_RGB(0,0,0),-1); 

			//CvPoint2D32f p1 = coordsReal2Image(cvPoint3D32f(0, 0, m_elevation));
			//CvPoint2D32f p2 = coordsReal2Image(cvPoint3D32f(s, 0, m_elevation));
			//CvPoint2D32f p3 = coordsReal2Image(cvPoint3D32f(0, s, m_elevation));
			//CvPoint2D32f p4 = coordsReal2Image(cvPoint3D32f(0, 0, m_elevation+s));
			//cvLine(frame, cvPointFrom32f(p1), cvPointFrom32f(p2), color);
			//cvLine(frame, cvPointFrom32f(p1), cvPointFrom32f(p3), color);
			//cvLine(frame, cvPointFrom32f(p1), cvPointFrom32f(p4), color);
			//cvCircle(frame, cvPointFrom32f(p1), 2, CV_RGB(0,0,0),-1); 
			//cvCircle(frame, cvPointFrom32f(p2), 2, CV_RGB(255,0,0),-1); 
			//cvCircle(frame, cvPointFrom32f(p3), 2, CV_RGB(0,255,0),-1); 
			//cvCircle(frame, cvPointFrom32f(p4), 2, CV_RGB(0,0,255),-1); 
			////cvCircle(frame, cvPointFrom32f(coordsReal2Image(C)), 4, CV_RGB(255,255,255),-1); 
			//drawText("x", cvPoint((int)p2.x+3,(int)p2.y+3), frame, color, CV_RGB(255,255,255), 0.8f);
			//drawText("y", cvPoint((int)p3.x+3,(int)p3.y+3), frame, color, CV_RGB(255,255,255), 0.8f);
			//drawText("z", cvPoint((int)p4.x+3,(int)p4.y+3), frame, color, CV_RGB(255,255,255), 0.8f);
			break;
		}
		case SPHERICAL: {
		}
	}
}
// ------------------------------------------------------------------------
void CameraModel::DrawDistanceMap(IplImage* frame) {
	double min_val = 0, max_val = 0;
	cvMinMaxLoc(distance_mask, &min_val, &max_val);
	for (int y=0; y<frame->height; y++)
	for (int x=0; x<frame->width; x++)
	{
		int a = sizeof(float);
		float value = ((float*)(distance_mask->imageData + distance_mask->widthStep*y))[x];
		int index = NTEMPERATURES-(int)(NTEMPERATURES*(value-min_val-1)/(max_val-min_val))-1;
		//p->B = (unsigned char)(255*(value-min_val)/(max_val-min_val));
		//p->G = 255-p->B;
 		Image::setPixel(frame, &HEAT_COLOR[index-1], x, y);
	}
}
// ------------------------------------------------------------------------
/**
 * Return the equation of the image plane in world coordinates
 * \param u abscissa axis
 * \param v ordinate axis
 * \param n plane normal
 */
void CameraModel::getImagePlane(CvPoint3D32f u, CvPoint3D32f v, CvPoint3D32f n) {
	// find eye point projection
	CvPoint2D32f o = coordsReal2Image(cvPoint3D32f(0, 0, m_elevation));	

	// back project with Z = 0;
	//CvPoint3D32f O = coordsImage2Real(o);
	//n = cvPoint3D32f(O.x - 0, O.y - 0, O.z - m_elevation);
}
// ------------------------------------------------------------------------
void CameraModel::DrawVerticalSpheroid(IplImage* frame, CvPoint3D32f center, CvSize2D32f axes, 
							   CvScalar color, bool fill ) {

	if (axes.width <= 0 || axes.height <= 0)
		return;
	if (center.z + axes.height >= m_elevation)
		return;
	switch (m_cameraType) {
		case XMLFILE: 
		case PROJECTION: {
		
			// ===============================================================
			// ========== Find projected ellipse =============================
			// @BOOK{Hartley2003,
			// title = {Multiple View Geometry in Computer Vision},
			//	publisher = {Cambridge University Press},
			//	year = {2003},
			//  page = {201},
			//	author = {Richard Hartley and Andrew Zisserman},
			//	address = {New York, NY, USA},
			//	isbn = {0521540518}
			//	}
			// also see: http://mathworld.wolfram.com/Ellipse.html
			// ===============================================================

			// Define Q_4x4 a matrix of a quadric
			CvMat* Q = cvCreateMat(4, 4, CV_64FC1); 
			cvZero(Q);
			cvmSet(Q, 0, 0, 1.0/(axes.width*axes.width)); 
			cvmSet(Q, 1, 1, 1.0/(axes.width*axes.width));
			cvmSet(Q, 2, 2, 1.0/(axes.height*axes.height));
			cvmSet(Q, 0, 3, -center.x/(axes.width*axes.width));
			cvmSet(Q, 3, 0, -center.x/(axes.width*axes.width)); 
			cvmSet(Q, 1, 3, -center.y/(axes.width*axes.width)); 
			cvmSet(Q, 3, 1, -center.y/(axes.width*axes.width)); 
			cvmSet(Q, 2, 3, -center.z/(axes.height*axes.height));			
			cvmSet(Q, 3, 2, -center.z/(axes.height*axes.height));			
			cvmSet(Q, 3, 3, 
				(center.x*center.x + center.y*center.y)/(axes.width*axes.width) 
				+(center.z*center.z)/(axes.height*axes.height) - 1);

			// compute q_3x3 a matrix of a conic as q = P Q_ P', where Q_ is adjoint (i.e. conjugate transposed) of Q (Q-1, if Q is invertible), q_ is adjoint of q
			CvMat* Q_ = cvCreateMat(4, 4, CV_64FC1); 
			CvMat* q_ = cvCreateMat(3, 3, CV_64FC1); 
			cvInvert(Q, Q_);
			CvMat* q34 = cvCreateMat(3, 4, CV_64FC1); 
			CvMat* q = cvCreateMat(3, 3, CV_64FC1); 
			cvGEMM(P, Q_, 1, NULL, 0, q34);
			cvGEMM(q34, P, 1, NULL, 0, q_, CV_GEMM_B_T);
			cvInvert(q_, q);

			//cvScale(q, q, 1.0/cvmGet(q, 2, 2));
			//cvScale(q, q, m_lambda);
			double 
				a = cvmGet(q,0,0),
				b = cvmGet(q,0,1),
				c = cvmGet(q,1,1),
				d = cvmGet(q,0,2),
				f = cvmGet(q,1,2),
				g = cvmGet(q,2,2);

			CvPoint imcenter = cvPointFrom32f(cvPoint2D32f((c*d-b*f)/(b*b-a*c), (a*f-b*d)/(b*b-a*c)));
			//cvCircle(frame, imcenter, 2, color, 2);

			// ellipse phi angle
			CvMat* evecs = cvCreateMat(3, 3, CV_64FC1); 
			CvMat* evals = cvCreateMat(1, 3, CV_64FC1); 
			cvEigenVV(q, evecs, evals);
			CvMat* evec1 = cvCreateMat(1, 3, CV_64FC1); cvGetRow(evecs, evec1, 0);
			double angle = a < c ? rad2deg(0.5*atan2( (2*b), (a-c) )) : rad2deg(PI/2 - 0.5*atan2( (2*b), (a-c) ));			
 			cvReleaseMat(&evecs); cvReleaseMat(&evals); cvReleaseMat(&evec1); 

			double Num = 2*(a*f*f+c*d*d+g*b*b-2*b*d*f-a*c*g);
			double Den = sqrt( (a-c)*(a-c) + 4*b*b );
			CvSize imaxes;
			imaxes.width = sqrt( abs(Num / ( (b*b-a*c) * ( Den-(c+a) ) )));
			imaxes.height = sqrt( abs(Num / ( (b*b-a*c) * ( -Den-(c+a) ) )));
			if (imcenter.x > 0 && imcenter.y > 0 && imaxes.width > 0 && imaxes.height > 0)
				cvEllipse(frame, imcenter, imaxes, angle, 0, 360, color, fill ? CV_FILLED : 1, fill ? 8 : CV_AA);
		
			// clean up matrices
			cvReleaseMat(&Q); cvReleaseMat(&q); cvReleaseMat(&q34); 
			cvReleaseMat(&Q_); cvReleaseMat(&q_); 
			break;
		}
		case SPHERICAL: {
			// find start and stop angle for scanlines (angles of two ellipse tangent lines from the camera point; the tangents that form the plane intersecting the shperoiod vertically in the middle)
			const double EPSILON = 1e-3;
			double a = axes.width;
			double c = axes.height;
			double a2 = a*a;
			double c2 = c*c;
			double dist = center.x*center.x + center.y*center.y;
			double c0 = center.z;
			int xLeft1=0, xRight1=0;
			double el = m_elevation;
			double l = (el-c0); // difference in z-coordinate between the camera and the center of the spheroid
			// Note: take (a^2) multiplier out of the equations to reduce order of magnitude		   
			long double E = a*a-dist;
			long double F = 2*sqrt(dist)*l;
			long double G = c2 - l*l;
			long double A = -F*F-G*G-E*E+2*G*E;
			long double B = F*F+2*G*G-2*G*E;
			long double C = -G*G;
			long double D = B*B-4*A*C;
			long double k1 = (-B+sqrt(D))/(2*A);
			long double k2 = (-B-sqrt(D))/(2*A);;

			// start, stop are latitude angles in radians measured starting from Z-axis pointing up.
			double start = PI-acos(sqrt(k1));
			double stop  = PI-acos(sqrt(k2));

			// if the camera is inside the body?
			if (Double::IsNaN(start) || Double::IsNaN(stop)) {
				start = 0;
				stop = PI;
			}

			int yy1 = (int)ceil((start-PI/2)*(h-1)/(PI/2));
			int yy2 = dist < a2 ? (h-1) : (int)floor((stop-PI/2)*(h-1)/(PI/2));
			int xLeft0 = -1, xRight0 = -1;
			// for each horizontal (linear angle increment) scanline of spheroid
			for ( int y = yy1; y <= yy2; y++ ) { 
				// find the tangent points of conic ray (lat=phi, long=theta, tip=h)
				// and the spheroid (axes = axes; center = center(x, y, z)
				double phi = PI/2 - y*(PI/2)/(h-1);
				double k = tan(phi);
				A = c2*el*el*k*k+
					a2*el*el;
				C = c2*dist+a2*l*l-a2*c2;
				E = c2*el*k;
				F = a2*l*el;
				double U1 = (sqrt(A*C)-F)/E;
				//double U2 = (-sqrt(A*C)-F)/E;
				// y0 might be close to zero => solve for y (division occurs only by x0)
				double D1, x1, y1, x2, y2;
				double U1_2 = U1*U1;
				double cy_2 = center.y*center.y;
				double cx_2 = center.x*center.x;
				D1 = U1_2*(cy_2-dist)+dist*cx_2;
				//if (D1 < 0) {
				//	Trace::WriteLine(String::Concat(S"y = ", __box(y), S" ##frame ", __box(doc->trackermodel.m_frameNumber), S" D1 =", __box(D1)
				//		, S" U1 =", __box(U1) , S" dist =", __box(dist), S" center.x =", __box(center.x), S" center.y =", __box(center.y)));
				//	Trace::WriteLine(String::Concat(S"center.x = ", __box(center.x), S"center.y = ", __box(center.y), S"center.z = ", __box(center.z)
				//		, S"axes.width = ", __box(axes.width), S"axes.height = ", __box(axes.height)));
				//}

				//double D2 = U2*U2*center.y*center.y-dist*(U2*U2-center.x*center.x);
				// EPSILON prevent very small negative values of D1 from causing the image to carry over to the other side of the image
				if (D1 >= -EPSILON && (abs(center.x) >= EPSILON || abs(center.y) >= EPSILON)) { // spheroid has tangents with current conical scan-line and not right below the camera 
					if (abs(center.x) > EPSILON) {
 						y1 = (U1*center.y - sqrt(D1))/dist;
						y2 = (U1*center.y + sqrt(D1))/dist;
						x1 = (U1-center.y*y1)/center.x;
						x2 = (U1-center.y*y2)/center.x;
					}
					else {
						D1 = U1*U1*(center.x*center.x-dist)+dist*center.y*center.y;
						x1 = (U1*center.x - sqrt(D1))/dist;
						x2 = (U1*center.x + sqrt(D1))/dist;
						y1 = (U1-center.x*x1)/center.y;
						y2 = (U1-center.x*x2)/center.y;
					}
					double theta1 = fmod(2*PI + atan2(y1, x1), 2*PI); 
					double theta2 = fmod(2*PI + atan2(y2, x2), 2*PI); 
					xLeft1  = round(theta1*w/(2*PI));
					xRight1 = round(theta2*w/(2*PI));

					// quadrants II and III
					if (center.x < -EPSILON || (center.x == 0 && center.y > EPSILON)) {
						int temp = xLeft1;
						xLeft1 = xRight1;
						xRight1 = temp;
					}
				}
				else {
					xLeft1 = 0;
					xRight1 = w-1;
					//doc->Message(String::Format("D1 = {2} xLeft1 = {0}  xRight1 = {1}", xLeft1, xRight1, D1));
					//AfxMessageBox("Again!");
				}

				// draw a line or two points
				if (fill) {
					if (xLeft1 > xRight1) {
						cvLine(frame, cvPoint(xLeft1, y), cvPoint(w-1, y), color);
						cvLine(frame, cvPoint(0, y), cvPoint(xRight1, y), color);
					}
					else {
						cvLine(frame, cvPoint(xLeft1, y), cvPoint(xRight1, y), color);
					}
				}
				else {
					if (y == yy1 || y == yy2) {
						if (xLeft1 > xRight1) {
							cvLine(frame, cvPoint(xLeft1, y), cvPoint(w-1, y), color);
							cvLine(frame, cvPoint(0, y), cvPoint(xRight1, y), color);
						}
						else {
							cvLine(frame, cvPoint(xLeft1, y), cvPoint(xRight1, y), color);
						}
					}
					if ( xLeft0 != -1 && (xLeft0-xRight0)*(xLeft1-xRight1) >= 0 ) {
						cvLine(frame, cvPoint(xLeft0, y), cvPoint(xLeft1, y), color);
						cvLine(frame, cvPoint(xRight0, y), cvPoint(xRight1, y), color);
					}
					else {
						cvLine(frame, cvPoint(xLeft1, y), cvPoint(xLeft1, y), color);
						cvLine(frame, cvPoint(xRight1, y), cvPoint(xRight1, y), color);
					}
					xLeft0 = xLeft1; xRight0 = xRight1;
				}
			}
		}
	}
}
// ------------------------------------------------------------------------
void CameraModel::DrawVerticalSpheroid(CDC* pDC, CvPoint3D32f center, CvSize2D32f axes)
{
	if (axes.width == 0 || axes.height == 0)
		return;
	if (center.z + axes.height >= m_elevation)
		return;

	// find start and stop angle for scan lines (angles of two ellipse tangent lines from the camera point; the tangents that form the plane intersecting the shperoiod vertically in the middle)
	const double EPSILON = 1e-3;
	double a = axes.width;
	double c = axes.height;
	double a2 = a*a;
	double c2 = c*c;
	double dist = center.x*center.x + center.y*center.y;
	double c0 = center.z;
	int xLeft1=0, xRight1=0;
	double el = m_elevation;
	double l = (el-c0); // difference in z-coordinate between the camera and the center of the spheroid
	// Note: take (a^2) multiplier out of the equations to reduce order of magnitude		   
	long double E = a*a-dist;
	long double F = 2*sqrt(dist)*l;
	long double G = c2 - l*l;
	long double A = -F*F-G*G-E*E+2*G*E;
	long double B = F*F+2*G*G-2*G*E;
	long double C = -G*G;
	long double D = B*B-4*A*C;
	long double k1 = (-B+sqrt(D))/(2*A);
	long double k2 = (-B-sqrt(D))/(2*A);;
	// start, stop are latitude angles in radians measured starting from Z-axis pointing up.
	double start = PI-acos(sqrt(k1));
	double stop  = PI-acos(sqrt(k2));

	// if the camera is inside the body?
	if (Double::IsNaN(start) || Double::IsNaN(stop)) {
		start = 0;
		stop = PI;
	}

	switch (m_cameraType) {
		case XMLFILE:
		case PROJECTION: {
			double theta = atan2(center.y, center.x);
			CvPoint3D32f projTop = cvPoint3D32f(cos(start-PI/2)*cos(theta), cos(start-PI/2)*sin(theta), el-sin(start-PI/2));
			CvPoint3D32f projBot = cvPoint3D32f(cos(stop-PI/2)*cos(theta), cos(stop-PI/2)*sin(theta), el-sin(stop-PI/2));
			CvPoint2D32f imProjTop = coordsReal2Image(projTop);
			CvPoint2D32f imProjBot= coordsReal2Image(projBot);
			double max_phi = (start+stop)/2;
			double k = tan(max_phi);
			A = c2*el*el*k*k+a2*el*el;
			C = c2*dist+a2*l*l-a2*c2;
			E = c2*el*k;
			F = a2*l*el;
			double U1 = (sqrt(A*C)-F)/E;
			double D1, x1, y1, x2, y2;
			D1 = U1*U1*(center.y*center.y-dist)+dist*center.x*center.x;
			if (abs(center.x) > EPSILON) {
				y1 = (U1*center.y - sqrt(D1))/dist;
				y2 = (U1*center.y + sqrt(D1))/dist;
				x1 = (U1-center.y*y1)/center.x;
				x2 = (U1-center.y*y2)/center.x;
			}
			else {
				return;
			}
			double max_theta1 = atan2(y1, x1)-PI; 
			double max_theta2 = atan2(y2, x2)-PI; 
			int BIG = 1;
			CvPoint3D32f projLeft = cvPoint3D32f(BIG*cos(max_phi-PI/2)*cos(max_theta1), BIG*cos(max_phi-PI/2)*sin(max_theta1), el-BIG*sin(max_phi-PI/2));
			CvPoint3D32f projRight = cvPoint3D32f(BIG*cos(max_phi-PI/2)*cos(max_theta2), BIG*cos(max_phi-PI/2)*sin(max_theta2), el-BIG*sin(max_phi-PI/2));
			CvPoint2D32f imProjLeft = coordsReal2Image(projLeft);
			CvPoint2D32f imProjRight= coordsReal2Image(projRight);
			double angle = (double)(180*atan(1.*(imProjBot.y-imProjTop.y)/(imProjTop.x-imProjBot.x))/PI);
			// find axes as the distance from N-S and E-W poles
			int a1 = round(d(imProjLeft, imProjRight)/2); // width
			int a2 = round(d(imProjTop, imProjBot)/2); // height
			//int a1 = round(a2*axes.width/axes.height); // width
			CvSize imaxes = cvSize(a2, a1); //////// Note: cvEllipse takes these in reverse order; Maybe not?
			CvPoint2D32f centerPoint = coordsReal2Image(center);
			pDC->Ellipse(centerPoint.x-axes.width, centerPoint.y-axes.height, centerPoint.x+axes.width, centerPoint.y+axes.height);
			break;
		}
		case SPHERICAL: {
			int yy1 = (int)ceil((start-PI/2)*(h-1)/(PI/2));
			int yy2 = dist < a2 ? (h-1) : (int)floor((stop-PI/2)*(h-1)/(PI/2));
			int xLeft0 = -1, xRight0 = -1;
			// for each horizontal (linear angle increment) scanline of spheroid
			for ( int y = yy1; y <= yy2; y++ ) { 
				// find the tangent points of conic ray (lat=phi, long=theta, tip=h)
				// and the spheroid (axes = axes; center = center(x, y, z)
				double phi = PI/2 - y*(PI/2)/(h-1);
				double k = tan(phi);
				A = c2*el*el*k*k+
					a2*el*el;
				C = c2*dist+a2*l*l-a2*c2;
				E = c2*el*k;
				F = a2*l*el;
				double U1 = (sqrt(A*C)-F)/E;
				//double U2 = (-sqrt(A*C)-F)/E;
				// y0 might be close to zero => solve for y (division occurs only by x0)
				double D1, x1, y1, x2, y2;
				double U1_2 = U1*U1;
				double cy_2 = center.y*center.y;
				double cx_2 = center.x*center.x;
				D1 = U1_2*(cy_2-dist)+dist*cx_2;
				//if (D1 < 0) {
				//	Trace::WriteLine(String::Concat(S"y = ", __box(y), S" ##frame ", __box(doc->trackermodel.m_frameNumber), S" D1 =", __box(D1)
				//		, S" U1 =", __box(U1) , S" dist =", __box(dist), S" center.x =", __box(center.x), S" center.y =", __box(center.y)));
				//	Trace::WriteLine(String::Concat(S"center.x = ", __box(center.x), S"center.y = ", __box(center.y), S"center.z = ", __box(center.z)
				//		, S"axes.width = ", __box(axes.width), S"axes.height = ", __box(axes.height)));
				//}

				//double D2 = U2*U2*center.y*center.y-dist*(U2*U2-center.x*center.x);
				// EPSILON prevent very small negative values of D1 from causing the image to carry over to the other side of the image
				if (D1 >= -EPSILON && (abs(center.x) >= EPSILON || abs(center.y) >= EPSILON)) { // spheroid has tangents with current conical scan-line and not right below the camera 
					if (abs(center.x) > EPSILON) {
						y1 = (U1*center.y - sqrt(D1))/dist;
						y2 = (U1*center.y + sqrt(D1))/dist;
						x1 = (U1-center.y*y1)/center.x;
						x2 = (U1-center.y*y2)/center.x;
					}
					else {
						D1 = U1*U1*(center.x*center.x-dist)+dist*center.y*center.y;
						x1 = (U1*center.x - sqrt(D1))/dist;
						x2 = (U1*center.x + sqrt(D1))/dist;
						y1 = (U1-center.x*x1)/center.y;
						y2 = (U1-center.x*x2)/center.y;
					}
					double theta1 = fmod(2*PI + atan2(y1, x1), 2*PI); 
					double theta2 = fmod(2*PI + atan2(y2, x2), 2*PI); 
					xLeft1  = round(theta1*w/(2*PI));
					xRight1 = round(theta2*w/(2*PI));

					// quadrants II and III
					if (center.x < -EPSILON || (center.x == 0 && center.y > EPSILON)) {
						int temp = xLeft1;
						xLeft1 = xRight1;
						xRight1 = temp;
					}
				}
				else {
					xLeft1 = 0;
					xRight1 = w-1;
					//doc->Message(String::Format("D1 = {2} xLeft1 = {0}  xRight1 = {1}", xLeft1, xRight1, D1));
					//AfxMessageBox("Again!");
				}

				//doc->Message(System::String::Format("{0} - {1}", xLeft1, xRight1));
				if (y == yy1 || y == yy2) {
					if (xLeft1 > xRight1) {
						pDC->MoveTo(xLeft1, y);
						pDC->LineTo(w-1, y);
					}
					else {
						pDC->MoveTo(xLeft1, y);
						pDC->LineTo(xRight1, y);
					}
				}
				if ( xLeft0 != -1 && (xLeft0-xRight0)*(xLeft1-xRight1) >= 0 ) {
					pDC->MoveTo(xLeft0, y);
					pDC->LineTo(xLeft1, y);
					pDC->MoveTo(xRight0, y);
					pDC->LineTo(xRight1, y);
				}
				else {
					pDC->MoveTo(xLeft1, y);
					pDC->LineTo(xLeft1, y);
					pDC->MoveTo(xRight1, y);
					pDC->LineTo(xRight1, y);
				}
				xLeft0 = xLeft1; xRight0 = xRight1;
			}
		}
	}
}
// ------------------------------------------------------------------------
void CameraModel::DrawVanishingPoints( IplImage* frame )
{
	CvPoint vx = cvPointFrom32f(coordsHomo2Image(VX));
	CvPoint vy = cvPointFrom32f(coordsHomo2Image(VY));
	CvPoint vz = cvPointFrom32f(coordsHomo2Image(VZ));
	CvPoint v0 = cvPointFrom32f(coordsHomo2Image(V0));
	cvLine(frame, vx, vy, CV_RGB(255, 0, 0), 1);
	cvLine(frame, vy, vz, CV_RGB(255, 0, 0), 1);
	cvLine(frame, vz, vx, CV_RGB(255, 0, 0), 1);
	cvLine(frame, vx, v0, CV_RGB(0, 255, 0), 1);
	cvLine(frame, vy, v0, CV_RGB(0, 255, 0), 1);
	cvLine(frame, vz, v0, CV_RGB(0, 255, 0), 1);
	cvCircle(frame, vx, 2, CV_RGB(255, 0, 0), 2);
	cvCircle(frame, vy, 2, CV_RGB(0, 255, 0), 2);
	cvCircle(frame, vz, 2, CV_RGB(0, 0, 255), 2);
	cvCircle(frame, v0, 2, CV_RGB(255, 255, 255), 2);
}
// ----------------------------------------------------------
CvPoint CameraModel::intersectVZ2Contour(CvPoint pt) {
	// Find a point on the intersection of the image boundary 
	// and the ray, formed by vanishing point and point p (of the contour)

	switch (m_cameraType) {
		case XMLFILE: {
			//CvPoint res; 
			//return res;
		}
		case PROJECTION: {
			double _VZ[] = {0,0,1}, _p1[] = {0,0,1}, _p2[] = {0,0,1};
			double _PT[] = {pt.x-w/2,pt.y-h/2,(w+h)/4}, _line1[] = {0,0,1}, _line2[] = {0,0,1};
			CvMat p1, p2, PT, line1, line2;
			//cvCopy(VZ, &VZ);
			//cvInitMatHeader( &VZ, 3, 1, CV_64FC1, _VZ);

			cvInitMatHeader( &p1, 3, 1, CV_64FC1, _p1);
			cvInitMatHeader( &p2, 3, 1, CV_64FC1, _p2);
			cvInitMatHeader( &line1, 3, 1, CV_64FC1, _line1);
			cvInitMatHeader( &line2, 3, 1, CV_64FC1, _line2);
			cvInitMatHeader( &PT, 3, 1, CV_64FC1, _PT);

			CvPoint2D32f vz = coordsHomo2Image(VZ);
			if (vz.x < 0 || vz.x >= w/2) {
				if (vz.x < 0) {
					_p1[0] = -w/2; _p1[1] = -h/2; _p1[2] = (w+h)/4;
					_p2[0] = -w/2; _p2[1] = +h/2; _p2[2] = (w+h)/4;
				}
				else {
					_p1[0] = w/2-1; _p1[1] = -h/2; _p1[2] = (w+h)/4;
					_p2[0] = w/2-1; _p2[1] = +h/2; _p2[2] = (w+h)/4;
				}
				cvCrossProduct(&p1, &p2, &line1);
				cvCrossProduct(&PT, VZ, &line2);
				cvCrossProduct(&line1, &line2, &p1);
				CvPoint res = cvPointFrom32f(coordsHomo2Image(&p1));
				if (res.x < w && res.x >= 0 && res.y < h && res.y >= 0)
					return res;
			}
			if (vz.y < 0 || vz.y >= h/2) {
				if (vz.y < 0) {
					_p1[0] = -w/2; _p1[1] = -h/2; _p1[2] = (w+h)/4;
					_p2[0] = +w/2; _p2[1] = -h/2; _p2[2] = (w+h)/4;
				}
				else {
					_p1[0] = -w/2; _p1[1] = +h/2-1; _p1[2] = (w+h)/4;
					_p2[0] = +w/2; _p2[1] = +h/2-1; _p2[2] = (w+h)/4;
				}
				cvCrossProduct(&p1, &p2, &line1);
				cvCrossProduct(&PT, VZ, &line2);
				cvCrossProduct(&line1, &line2, &p1);
				CvPoint res = cvPointFrom32f(coordsHomo2Image(&p1));
				if (res.x < w && res.x >= 0 && res.y < h && res.y >= 0)
					return res;
			}
						 }
		case SPHERICAL:  {
			return cvPoint(pt.x, h-1);
		 }
	}
	return pt;
}