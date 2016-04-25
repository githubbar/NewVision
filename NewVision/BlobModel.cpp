#include "StdAfx.h"
#include "BlobModel.h"
#include "NewVisionDoc.h"
#include "Globals.h"

IMPLEMENT_SERIAL(BlobModel, Model, 1)
// --------------------------------------------------------------------------
BlobModel::BlobModel(void)
{
	m_initialized = false;
}
// --------------------------------------------------------------------------
BlobModel::~BlobModel(void)
{
	blob.RemoveAll();
}
// --------------------------------------------------------------------------
bool BlobModel::Initialize() {
	if (m_initialized)
		DeInitialize();
	m_initialized = true;
	return true;
}
// --------------------------------------------------------------------------
void BlobModel::DeInitialize() {
	if (!m_initialized)
		return;
	m_initialized = false;
	cvZero(motionmap);
	blob.RemoveAll();
}
// --------------------------------------------------------------------------
void BlobModel::FindBlobs(IplImage* frame, IplImage* mask, double confidence) {
	if (!m_initialized)
		return;
	
	// clear the blob array
	blob.RemoveAll();
	// detect contours and populate array of blobs
	CvSeq* contour = 0;		
	cvFindContours(mask, OpenCVmemory, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	int nPix = frame->width*frame->height;
	for(CvSeq* cPtr = contour ; cPtr != 0; cPtr = cPtr->h_next)
	{
		// discard noise contours
		CvMoments moments;
		cvMoments(cPtr , &moments);
		double area = cvGetCentralMoment(&moments, 0, 0); // area
 		if (area > doc->blobmodel.m_minArea*nPix && area < doc->blobmodel.m_maxArea*nPix) {
			Blob *bl = new Blob(cPtr, doc);
			bl->m_confidence = confidence;
			blob.Add(bl);
		}
	}
}
// --------------------------------------------------------------------------
void BlobModel::Draw(IplImage* frame, CvScalar color) {
	if (!m_initialized)
		return;
	//echo(String::Format("blob. frame = {0}", doc->trackermodel.m_frameNumber));
	for (int i=0;i<blob.GetCount();i++) {
		blob[i]->Draw(frame, color);
	}
}
// --------------------------------------------------------------------------
void BlobModel::DrawHeight(IplImage* frame, CvScalar color) {
	if (!m_initialized)
		return;

	for (int i=0;i<blob.GetCount();i++) {
		blob[i]->DrawHeight(frame, color);
	}
}
// --------------------------------------------------------------------------
void BlobModel::Rasterize(IplImage* frame, CvScalar color) {
	if (!m_initialized)
		return;

	for (int i=0;i<blob.GetCount();i++) {
		blob[i]->Rasterize(frame, color);
	}
}
// --------------------------------------------------------------------------
void BlobModel::UpdateHeatMap(IplImage* motionmap) {
	// for each head candidate draw a circle on the floor; add it to the heatmap image
	// NOTE: due to scaling, circle becomes an ellipse
	double BLOB_RADIUS = 50; // blob projection on the floor average radius in centimeters 
	bool oneclose = false;
	for (int i=0;i<blob.GetCount();i++) {
		CvSeq* heads = doc->blobmodel.blob[i]->heads;
		for (int j=0;j<heads->total;j++) {
			BlobRay* br = (BlobRay*)cvGetSeqElem(heads, j);
			CvPoint3D32f head, foot;
			doc->cameramodel.coordsImage2RealSameXY_Feet2Floor(cvPointTo32f(br->p1), cvPointTo32f(br->p2), &head, &foot);
			// ignore short candidates
			if (head.z < doc->bodymodel.m_minHeight)
				continue;
			// ignore repeating artifact closeby candidates
			if (oneclose)
				continue;
			if (d(foot) < BLOB_RADIUS*2)
				oneclose = true;

			CvPoint c = doc->floormodel.coordsReal2Floor(foot);
			CvSize axes = doc->floormodel.sizeReal2Floor(cvSize2D32f(BLOB_RADIUS, BLOB_RADIUS)); 
			cvZero(motionmaptemp);
			cvEllipse(motionmaptemp, c, axes, 0, 0, 360, cvScalar(1), CV_FILLED);
			cvAcc(motionmaptemp, motionmap);
		}
	}
}
// --------------------------------------------------------------------------
