#include "StdAfx.h"
#include "SkinModel.h"
#include "NewVisionDoc.h"
#include "Globals.h"
IMPLEMENT_SERIAL(SkinModel, Model, 1)
// ------------------------------------------------------------------------
SkinModel::SkinModel(void)
{

}
// ------------------------------------------------------------------------
SkinModel::~SkinModel(void)
{
}
// ------------------------------------------------------------------------
void SkinModel::Serialize( CArchive& ar )
{
	CObject::Serialize(ar);
	if( ar.IsStoring()) {
		ar << m_skinType;
	}
	else {
		ar >> m_skinType;
	}
	color.Serialize( ar );	
}
// ------------------------------------------------------------------------
void SkinModel::AddValue(CvScalar bgr) {
	color.AddValue(bgr);
}
// ------------------------------------------------------------------------
int SkinModel::GetValue(double a, double b) {
	return (int)color.GetValue(a, b);
}
// ------------------------------------------------------------------------
int SkinModel::GetValue(CvScalar bgr) {
	return (int)color.GetValue(bgr);
}
// ------------------------------------------------------------------------
CvPoint2D32f SkinModel::GetSkinAreaCenter(IplImage* frame, IplImage* mask, double& nMatches) {
	CvPoint2D32f center = cvPoint2D32f(0,0);
	float norm = 0;
	int pixInMask = 0;
	for (int y=0; y<frame->height; y++)
	for (int x=0; x<frame->width; x++)
	{
		if (mask->imageData[y*mask->widthStep+x] != 0) {
			CvScalar pix = cvGet2D(frame, y, x);
			int value = GetValue(pix);
			pixInMask++;
			if (value > 0) {
				center.x += value*x;
				center.y += value*y;
				norm += value;
				
			}
		}
	}
	nMatches = 1.0*norm/pixInMask;
	if (norm == 0)
		return cvPoint2D32f(-1, -1);

	center.x /= norm;
	center.y /= norm;

	return center;
}

// ------------------------------------------------------------------------
void SkinModel::Draw(IplImage* frame, CvScalar c) {
	color.Draw(frame);
}
// ------------------------------------------------------------------------
void SkinModel::Draw(CDC* pDC) {
	color.Draw(pDC);
}
// ------------------------------------------------------------------------
void SkinModel::DrawSkinAreas(IplImage* output, IplImage* frame) {
	cvCvtColor(frame, lab, CV_BGR2Lab);
	float min_value = 0, max_value = 0;
	cvGetMinMaxHistValue( color.h, &min_value, &max_value, 0, 0 );
	for (int y=0; y<lab->height; y++)
	for (int x=0; x<lab->width; x++)
	{
		CvScalar pix = cvGet2D(lab, y, x);
		double a = pix.val[1];
		double b = pix.val[2];
		int value = GetValue(a, b);
		Pixel p(round((value-min_value)*255/(max_value-min_value)), 0, 0);
		Image::setPixel(output, &p, x, y);
	}
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
