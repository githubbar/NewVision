
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
 *	Description :  
*/
#if !defined (_BLOB_)
#define _BLOB_
#pragma once

#include "cv.h"    
#define MIN_PIX_TO_BE_OVER 2

class CNewVisionDoc;

class Blob {
	public:
		CNewVisionDoc*  doc;
		CvSeq* m_contour;
		CvSeq* heads;
		CvPoint pos;
		CvPoint velocity;
		CvSize size;
		double m_confidence;

						Blob(void);
						Blob(CvSeq* contour, CNewVisionDoc* doc);
						~Blob(void);
		void			initPos();
		void			initSize();
		CvSeq*			getContour();
		CvPoint			getPos();
		CvSize			getSize();
		CvPoint			getVelocity();
		void            Draw(IplImage* frame, CvScalar color);
		void			DrawHeight(IplImage* frame, CvScalar color);
		void			Rasterize(IplImage* frame, CvScalar color);
		void			merge(Blob* that);		                   // merge with another blob
		double			distance(Blob* that);                   // find distance between two blobs
		int				numVertices();                             // return the number of vertices
		void			getProjectionHistogram(CvSeq* hist);     
		void			InitHeadCandidates();

	private:
		CvSeq*			copyContour(CvSeq* c); 
		void			appendContour(CvSeq* c);
};

struct BlobRay {
	CvPoint p1, p2;
	int count;

	BlobRay(void) {
		count = 0;
	};

	bool isNeighbor(BlobRay* that, double minDist) {
		if (abs(p1.x-that->p1.x)+abs(p1.y-that->p1.y) < minDist*minDist)
			return true;
		return false;
	};
	
	// compare BlobRay elements based on count
	static int cmp_count( const void* _a, const void* _b, void* userdata )
	{
		BlobRay* a = (BlobRay*)_a;
		BlobRay* b = (BlobRay*)_b;
		return a->count > b->count ? -1 : a->count< b->count? 1 : 0;
	};

	// compare BlobRay elements based on p1.x
	static int cmp_p1x( const void* _a, const void* _b, void* userdata )
	{
		BlobRay* a = (BlobRay*)_a;
		BlobRay* b = (BlobRay*)_b;
		return a->p1.x > b->p1.x ? -1 : a->p1.x < b->p1.x ? 1 : 0;
	};
};

#endif // !defined( _BLOB_ )