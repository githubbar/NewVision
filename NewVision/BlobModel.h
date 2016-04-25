#if !defined( BLOBMODEL_H )
#define BLOBMODEL_H

#pragma once
#include "Model.h"
#include "cv.h"      
#include "Blob.h"      
#include "SmartPtrArray.h"

#define BLOB_ARRAY_SIZE 128

class BlobModel : public Model
{
DECLARE_SERIAL(BlobModel)
public:

	// ------------- Persistant variables ---------------------
	double m_minArea;
	double m_maxArea;
	int m_maxBodies;
	double m_minRaySize;
	bool m_initialized;

	SmartPtrArray<Blob> blob;   // array of blobs

					BlobModel(void);
					~BlobModel(void);
	bool            Initialize();
	void            DeInitialize();
	void			FindBlobs(IplImage* frame, IplImage* mask, double confidence = 1);
	void			Draw(IplImage* frame, CvScalar color);
	void			DrawHeight(IplImage* frame, CvScalar color);
	void			Rasterize(IplImage* frame, CvScalar color);
	void			UpdateHeatMap(IplImage* motionmap);
};

#endif // !defined( BLOBMODEL_H )