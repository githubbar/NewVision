#if !defined( SKINMODEL_H )
#define SKINMODEL_H

#pragma once
#include "Model.h"
#include "cv.h"      
#include "LABHistogram2D.h"

enum SkinModelType {SKIN_2D_HISTOGRAM = 0};

class SkinModel : public Model
{
DECLARE_SERIAL( SkinModel )
public:

	// ------------- Persistant variables ---------------------
	int m_skinType; // {SKIN_2D_HISTOGRAM = 0}
	LABHistogram2D color;

	SkinModel(void);
	~SkinModel(void);
	virtual void Serialize(CArchive& ar);
	void         AddValue(CvScalar bgr);
	int          GetValue(CvScalar bgr);
	int          GetValue(double a, double b);
	CvPoint2D32f GetSkinAreaCenter(IplImage* frame, IplImage* mask, double& nMatches);
	void         DrawSkinAreas(IplImage* output, IplImage* frame);
	void         Draw(IplImage* frame, CvScalar color);
	void         Draw(CDC* pDC);
};
#endif // !defined( SKINMODEL_H )