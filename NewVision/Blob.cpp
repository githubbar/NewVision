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

#include "StdAfx.h"
#include "NewVision.h"
#include "blob.h"
#include "math.h"
#include "Globals.h"
#include "NewVisionDoc.h"
// ----------------------------------------------------------
Blob::Blob(void) {
}
// ----------------------------------------------------------
Blob::~Blob() {
	//cvClearSeq(m_contour);
	//cvClearSeq(heads);
}
// ----------------------------------------------------------
Blob::Blob(CvSeq* c, CNewVisionDoc* doc) {
	this->doc = doc;
	heads = cvCreateSeq(CV_SEQ_ELTYPE_GENERIC, sizeof(CvSeq), sizeof(BlobRay), OpenCVmemory);
	// initialize contour		
	m_contour = copyContour(c);
	
	// initialize position (center of mass)
	initPos();

	// initialize size 
	initSize();

	// initialize head candidates
	InitHeadCandidates();

}
// ----------------------------------------------------------
void Blob::merge(Blob* that) {
	// reinitialize contour		
	appendContour(that->m_contour);
	// reinitialize position (center of mass)
	initPos();
	// reinitialize size 
	initSize();	
}
// ----------------------------------------------------------
// create a copy of CvSeq POLYLINE contour (no holes)
CvSeq* Blob::copyContour(CvSeq* c) {
	CvSeq* contour = cvCreateSeq( c->flags, sizeof(CvContour), sizeof(CvPoint), OpenCVmemory);

	if( CV_IS_SEQ_POLYLINE( c ))
    {
		int i, count = c->total;
		CvSeqReader reader;
		CvSeqWriter writer;
		cvStartReadSeq(c, &reader, 0);
		cvStartAppendToSeq(contour, &writer);
		CvPoint pt;

        /* scroll the reader by 1 point */
        for( i = 0; i < count; i++ )
        {
			CV_READ_SEQ_ELEM( pt, reader );
			CV_WRITE_SEQ_ELEM( pt, writer );
	    }
		cvEndWriteSeq( &writer );
	}
	return contour;
}
// ----------------------------------------------------------
// append CvSeq POLYLINE contour (no holes) to the existing contour
void Blob::appendContour(CvSeq* c) {
	if( CV_IS_SEQ_POLYLINE( c ))
    {
		int i, count = c->total;
		CvSeqReader reader;
		CvSeqWriter writer;
		cvStartReadSeq(c, &reader, 0);
		cvStartAppendToSeq(m_contour, &writer);
		CvPoint pt;

        /* scroll the reader by 1 point */
        for( i = 0; i < count; i++ )
        {
			CV_READ_SEQ_ELEM( pt, reader );
			CV_WRITE_SEQ_ELEM( pt, writer );
	    }
		cvEndWriteSeq( &writer );
	}
}
// ----------------------------------------------------------
void Blob::initPos() {
	CvMoments moments;
	cvMoments(m_contour, &moments);
	double M00,M01,M10;
	double x, y;
	M00 = cvGetSpatialMoment( &moments, 0, 0 );
	M10 = cvGetSpatialMoment( &moments, 1, 0 );
	M01 = cvGetSpatialMoment( &moments, 0, 1 );
	x = M10/M00;
	y = M01/M00;
	#pragma warning( disable : 4244 )
	pos = cvPoint(x, y);
	#pragma warning( default : 4244 )
}
// ----------------------------------------------------------
void Blob::initSize() {
	CvRect r = cvBoundingRect(m_contour, 1);
	size = cvSize(r.width, r.height);
}
// ----------------------------------------------------------
CvSeq* Blob::getContour() {
	return m_contour;
}

// ----------------------------------------------------------
CvPoint Blob::getPos() {
	return pos;
}
// ----------------------------------------------------------
CvSize Blob::getSize() {
	return size;
}
// ----------------------------------------------------------
CvPoint Blob::getVelocity() {
	return velocity;
}
// ----------------------------------------------------------
double Blob::distance(Blob* that) {
	return d_contour2contour(this->m_contour, that->m_contour);
}
// ----------------------------------------------------------
int Blob::numVertices() {
	return m_contour->total;
}
// ----------------------------------------------------------
void Blob::getProjectionHistogram(CvSeq* hist) {
	CameraDialog* cm = &doc->cameramodel;
	BlobDialog*   bm = &doc->blobmodel;
	// create a mask image
	cvZero(blob_mask);
	cvDrawContours(blob_mask , m_contour, cvScalar(1), cvScalar(1), 0, CV_FILLED, 8);
			// go through all the points on the contour
	CvSeqReader reader;
	cvStartReadSeq(this->m_contour, &reader, 0);
	CvPoint pt;
	for(int n = 0; n < this->m_contour->total; n++ ) {
		CV_READ_SEQ_ELEM( pt, reader);
		//CvPoint ptVZ = cm->intersectVZ2Contour(pt); 
		CvPoint ptVZ = cvPointFrom32f(cm->coordsHomo2Image(cm->VZ));
		CvLineIterator iterator;
		cvClipLine( cvSize(doc->trackermodel.cx, doc->trackermodel.cy) , &pt, &ptVZ);
		int count = cvInitLineIterator(blob_mask, pt, ptVZ, &iterator, 8);
		BlobRay br;
		br.p1 = pt;
		bool under = false; // if the vertex is on the underside
		for( int i = 0; i < count; i++ ) {
			if (iterator.ptr[0] ==  0 && i <= MIN_PIX_TO_BE_OVER) {
				under = true;
				break;
			}
			if (iterator.ptr[0] != 0) { // store x,y for the last non-empty point
				int offset, x, y;
				offset = (int)(iterator.ptr - (uchar*)(blob_mask->imageData));
				y = offset/blob_mask ->widthStep;
				x = (offset - y*blob_mask ->widthStep)/(blob_mask->nChannels) ;
				br.p2.x = x;
				br.p2.y = y;
				br.count += iterator.ptr[0]; // +=1
			}
			CV_NEXT_LINE_POINT(iterator);
		}
		if (!under)
			cvSeqPush(hist, &br);
	}
}
// ----------------------------------------------------------
void Blob::InitHeadCandidates() {
	CameraDialog* cm = &doc->cameramodel;
	BlobDialog*   bm = &doc->blobmodel;
	
	CvSeq* temp = cvCreateSeq(CV_SEQ_ELTYPE_GENERIC, sizeof(CvSeq), sizeof(BlobRay), OpenCVmemory);
	getProjectionHistogram(temp);

	// find local peaks
	int N = temp ->total;
	for(int i = 0; i < N; i++ ) {	
		BlobRay* br = (BlobRay*)cvGetSeqElem(temp, i);
		bool max = true;
		for(int j = 0; j < (int)(1./bm->m_maxBodies*N/2); j++ ) {	
			BlobRay* br_prev = (BlobRay*)cvGetSeqElem(temp, i-1-j < 0 ? (N+i-1-j)%N : i-1-j);
			if (br->count < br_prev->count) {
				max = false;
				break;
			}
		}
		if (max) {
			for(int j = 0; j < (int)(1.0/bm->m_maxBodies*N/2); j++ ) {	
				BlobRay* br_next = (BlobRay*)cvGetSeqElem(temp, (i+1+j)%N);
				if (br->count < br_next->count) {
					max = false;
					break;
				}
			}
		}
		if (max) {
			cvSeqPush(heads, br);
		}
	}
	cvClearSeq(temp);

	// find max candidate remove the short heads
	int maxCount = 0;
	for(int i = 0; i < heads->total; i++ ) {
		BlobRay* br1 = (BlobRay*)cvGetSeqElem(heads, i);
		if (br1->count > maxCount)
			maxCount = br1->count;
	}
	for(int i = 0; i < heads->total; i++ ) {
		BlobRay* br1 = (BlobRay*)cvGetSeqElem(heads, i);
		// if less that MIN_RAY_SIZE % of the longest
		if (br1->count < bm->m_minRaySize*maxCount) {
			cvSeqRemove(heads, i);
			i--;
		}
	}

	// remove neighboring candidates with equal counts
	// TODO: modify to work only within a window M, use average as a candidate!
	cvSeqSort(heads, BlobRay::cmp_count);
	for(int i = 0; i < heads->total; i++ ) {	
		BlobRay* br1 = (BlobRay*)cvGetSeqElem(heads, i);
		for(int j = i+1; j < heads->total; j++ ) {	
			BlobRay* br2 = (BlobRay*)cvGetSeqElem(heads, j);
			// if two or more are the same 
			if (br1->count == br2->count) {
				cvSeqRemove(heads, j);
				j--;
			}
			else 
				break;
		}
	}
}
// ----------------------------------------------------------
void Blob::Draw(IplImage* frame, CvScalar color) {
	CameraDialog* cm = &doc->cameramodel;
	cvDrawContours(frame, m_contour, color, CV_RGB(100,255,0), 0, CV_FILLED, 8);
	//if (m_confidence == 1)
	//	cvDrawContours(frame, m_contour, color, color, 0, 1, 8);
	//else
	//	cvDrawContours(frame, m_contour, CV_RGB(255,0,0), CV_RGB(255,0,0), 0, 1, 8);
	for (int j=0;j<heads->total;j++) {
		BlobRay* ray = (BlobRay*)cvGetSeqElem(heads, j);
		/*
		CvSize axes;
		axes.height = (int)sqrt((double)((ray->p1.x-ray->p2.x)*(ray->p1.x-ray->p2.x)+
			(ray->p1.y-ray->p2.y)*(ray->p1.y-ray->p2.y)))/2;
		axes.width = (int)(0.4*axes.height);
		CvPoint p;
		p.x = (ray->p1.x + ray->p2.x)/2;
		p.y = (ray->p1.y + ray->p2.y)/2;
		double slope = (double)((ray->p1.y-ray->p2.y));
		slope = slope/(ray->p1.x - ray->p2.x);
		slope = atan(slope);
		double angle = 	180*(slope)/PI;
		cvEllipse(frame, p, axes, -90-angle, 0, 360, CV_RGB(0,0,255),1);
		*/

		// draw endpoints
		cvCircle(frame, ray->p1, 2, CV_RGB(255,0,0),-1); 
		cvCircle(frame, ray->p2, 2, CV_RGB(0,255,0),-1); 

		/*CvPoint3D32f h3, f3;
		double f = cm->coordsImage2RealSameXY_Feet2Floor(cvPointTo32f(ray->p1), cvPointTo32f(ray->p2), &h3, &f3);
		char s[20];
		sprintf(s, "%d", (int)f);
		*/
		
	}
}
// ----------------------------------------------------------
void Blob::DrawHeight(IplImage* frame, CvScalar color) {
	double max_f = 0;
	BlobRay* max_ray = NULL;
	for (int j=0;j<heads->total;j++) {
		BlobRay* ray = (BlobRay*)cvGetSeqElem(heads, j);
		double f = doc->cameramodel.computeHeight(cvPointTo32f(ray->p1), cvPointTo32f(ray->p2));
		if (f > max_f) {
			max_f = f;
			max_ray = ray;
		}
	}
	if (max_ray)
		drawText(max_f, max_ray->p1, frame, color, CV_RGB(255,255,255), 0.9f);
}
// ----------------------------------------------------------
void Blob::Rasterize(IplImage* frame, CvScalar color) {
	cvDrawContours(frame, m_contour, color, color, 0, CV_FILLED, 8);
}
