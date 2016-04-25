#include "StdAfx.h"
#include "BackgroundModel.h"
#include "NewVisionDoc.h"
#include "Globals.h"
#include "Blob.h"

using namespace OpenCVExtensions;

// --------------------------------------------------------------------------
BackgroundModel::BackgroundModel(void) {
	m_initialized = false;
}
// --------------------------------------------------------------------------
BackgroundModel::~BackgroundModel(void) {
}
// --------------------------------------------------------------------------
bool BackgroundModel::Initialize() {
	if (m_initialized)
		DeInitialize();

	if (!doc->trackermodel.IsInitialized())
		return false;
	// initialize a codebook
	if (GetSize() == 0)
		SetSize(doc->trackermodel.cx*doc->trackermodel.cy);

	return true;
}
// --------------------------------------------------------------------------
void BackgroundModel::DeInitialize() {
	if (!m_initialized)
		return;
	m_initialized = false;
	RemoveAll();
}
// --------------------------------------------------------------------------
void BackgroundModel::Update(int nFrame/*=-1*/, IplImage *mask) {
	// accept only unsigned char x 3 images
	ASSERT(imgFrame->depth == IPL_DEPTH_8U);
	ASSERT(imgFrame->nChannels == 3);

	if (!mask) {
		// check if it is time to update
		if (nFrame != -1 && nFrame % m_updateFreq)
			return;

		// if done updating - exit
		if (m_mode == STATIC && nFrame != -1 && nFrame >= m_initStage)
			return;

		// remove stale codewords
		if (nFrame != -1 && (nFrame % m_removeStaleFreq) == 0)
			RemoveStale();
		// create a mask of non-dormant bodies
		cvZero(body_mask);
		doc->bodymodel.RasterizeNonDormant(body_mask);

	}

	// update each codeword
	Pixel* p = new Pixel();
	for (int x=0; x<imgFrame->width; x++)
	for (int y=0; y<imgFrame->height; y++)
	{
		if ( mask && mask->imageData[mask->widthStep*y+x] != 0 || 
			!mask && body_mask->imageData[body_mask->widthStep*y+x] == 0 ) {
			Image::getPixel(imgFrame, p, x, y);
			this->GetAt(y*imgFrame->width+x).UpdateColor(p, nFrame, m_CDmax, m_BDmin, m_BDmax);
			if (doc->trackermodel.m_mode == TRGBFILE) {
				int val = (UCHAR)(imgThermal->imageData[imgThermal->widthStep*y+x]);
				this->GetAt(y*imgFrame->width+x).UpdateThermal(val, nFrame, m_ThermalMin, m_ThermalMax);
			}
		}
	} 
	delete p;
} 
// --------------------------------------------------------------------------
void BackgroundModel::SubtractBackground(IplImage* FGMap, int nFrame) {
	// accept only unsigned char x 3 images
	ASSERT(imgFrame->depth == IPL_DEPTH_8U);
	ASSERT(imgFrame->nChannels == 3);
	ASSERT(FGMap->nChannels == 1);
	ASSERT(FGMap->depth == IPL_DEPTH_8U);

	// initialize a codebook
	if (GetSize() == 0)
		SetSize(Image::getSizeInPixels(imgFrame));

	// start subtraction after m_trackAfterAgePercentage of codewords have reached m_trackAfterAge
	if (m_mode == ADAPTIVE) {
		int all = 0;
		int mature = 0;
		for (int i=0; i<Image::getSizeInPixels(imgFrame); i++)
		{
			mature += this->GetAt(i).CountOlderThan(m_trackAfterAge);
			all += (int)this->GetAt(i).color.GetSize();
		}	
		if (1.0*mature/all < 1.0*m_trackAfterAgePercentage/100)
			return;
	}
	if (m_mode == STATIC) {
		if (nFrame < m_initStage) {
			cvZero(FGMap);
			return;
		}
	}

	// -------------- Match Deterministically -------------------------
	Pixel* p = new Pixel();
	for (int y=0; y<imgFrame->height; y++)
	for (int x=0; x<imgFrame->width; x++)
	{
		Image::getPixel(imgFrame, p, x, y);
		if (this->GetAt(y*imgFrame->width+x).MatchColor(p, nFrame, m_CDmax, m_BDmin, m_BDmax, m_minAge))
			FGMap->imageData[y*FGMap->widthStep+x] = 0;
		else
			FGMap->imageData[y*FGMap->widthStep+x] = (UCHAR)255;
		
		if (TRGBFILE) {
			int val = (UCHAR)(imgThermal->imageData[imgThermal->widthStep*y+x]);
			if (this->GetAt(y*imgFrame->width+x).MatchThermalByDeviation(val, nFrame, m_ThermalMin, m_ThermalMax, m_minAge))
				FGMap->imageData[y*FGMap->widthStep+x] = 0;
		}

	} 
	delete p;

	// -------------- Match Probabilistically -------------------------
	//Pixel* p = new Pixel();
	//for (int y=0; y<imgFrame->height; y++)
	//	for (int x=0; x<imgFrame->width; x++)
	//	{
	//		double dColor = 0, dThermal = 0;
	//		Image::getPixel(imgFrame, p, x, y);
	//		dColor = this->GetAt(y*imgFrame->width+x).MatchColorProbabilistic(p, nFrame, m_CDmax, m_BDmin, m_BDmax, m_minAge);
	//		if (TRGBFILE) {
	//			int val = (UCHAR)(imgThermal->imageData[imgThermal->widthStep*y+x]);
	//			dThermal = this->GetAt(y*imgFrame->width+x).MatchThermalByDeviationProbabilistic(val, nFrame, m_ThermalMin, m_ThermalMax, m_minAge);
	//		
	//		}
	//		//if (dColor + dThermal > 1.0/m_sensitivity)
	//		//	FGMap->imageData[y*FGMap->widthStep+x] = (UCHAR)255;
	//		//if (dColor + dThermal > 0.5/m_sensitivity)
	//		//	FGMap->imageData[y*FGMap->widthStep+x] = (UCHAR)(255*(dColor + dThermal)/m_sensitivity);
	//		//else 
	//		//	FGMap->imageData[y*FGMap->widthStep+x] = 0;
	//		if (dColor > 1.0/m_sensitivity)
	//			FGMap->imageData[y*FGMap->widthStep+x] = (UCHAR)255;
	//		else if (dColor > 0.5/m_sensitivity)
	//			FGMap->imageData[y*FGMap->widthStep+x] = (UCHAR)(255*(dColor)/m_sensitivity);
	//		else 
	//			FGMap->imageData[y*FGMap->widthStep+x] = 0;
	//		//if (dThermal > 1.0/m_sensitivity)
	//		//	FGMap->imageData[y*FGMap->widthStep+x] = (UCHAR)255;
	//		//else if (dThermal > 0.5/m_sensitivity)
	//		//	FGMap->imageData[y*FGMap->widthStep+x] = (UCHAR)(255*(dThermal)/m_sensitivity);
	//		//else 
	//		//	FGMap->imageData[y*FGMap->widthStep+x] = 0;
	//	} 
	//delete p;

	// -------------- Morphologically post-process the image -------------------------
	if (m_Erode) {
		IplConvKernel* elem = cvCreateStructuringElementEx( m_ErodeSize, m_ErodeSize, 0, 0, CV_SHAPE_RECT);
		cvErode(FGMap, FGMap, elem); 
		cvReleaseStructuringElement(&elem);
	}
	if (m_Dilate) {
		IplConvKernel* elem = cvCreateStructuringElementEx( m_DilateSize, m_DilateSize, 0, 0, CV_SHAPE_RECT);
		cvDilate(FGMap, FGMap, elem);
		cvReleaseStructuringElement(&elem);
	}
}
// --------------------------------------------------------------------------
void BackgroundModel::SubtractBackgroundProbabilistic(IplImage* FGMapDouble, int nFrame) {
	// accept only unsigned char x 3 images
	ASSERT(imgFrame->depth == IPL_DEPTH_8U);
	ASSERT(imgFrame->nChannels == 3);
	ASSERT(FGMapDouble->nChannels == 1);
	ASSERT(FGMapDouble->depth == IPL_DEPTH_32F);
	
	// start subtraction after m_trackAfterAgePercentage of codewords have reached m_trackAfterAge
	if (m_mode == ADAPTIVE) {
		int all = 0;
		int mature = 0;
		for (int i=0; i<Image::getSizeInPixels(imgFrame); i++)
		{
			mature += this->GetAt(i).CountOlderThan(m_trackAfterAge);
			all += (int)this->GetAt(i).color.GetSize();
		}	
		if (1.0*mature/all < 1.0*m_trackAfterAgePercentage/100)
			return;
	}
	if (m_mode == STATIC) {
		if (nFrame < m_initStage) {
			cvZero(FGMapDouble);
			return;
		}
	}
	
	Pixel* p = new Pixel();
	for (int y=0; y<imgFrame->height; y++)
	for (int x=0; x<imgFrame->width; x++)
	{
		Image::getPixel(imgFrame, p, x, y);
		((float*)(FGMapDouble->imageData + FGMapDouble->widthStep*y))[x] = (float)this->GetAt(y*imgFrame->width+x).MatchColorProbabilistic(p, nFrame, m_CDmax, m_BDmin, m_BDmax, m_minAge);
	} 
	delete p;
}
// --------------------------------------------------------------------------
void BackgroundModel::GetSizes(IplImage* result) {
	int max_size = 0;
	for (int i=0;i<GetSize();i++) {
		result->imageData[i] = (char)this->GetAt(i).color.GetSize();
		if (result->imageData[i] > max_size)
			max_size = result->imageData[i];
	} 
	cvScale(result, result, 255/max_size);
}
// --------------------------------------------------------------------------
void BackgroundModel::RemoveStale() {
	for (int i=0;i<GetSize();i++) {
		this->GetAt(i).RemoveStale((int)(m_staleCutoff));
	} 
}
// --------------------------------------------------------------------------
void BackgroundModel::DrawHeat(IplImage* srcFrame1, IplImage* tgtFrame) {
	double min_val = 0, max_val = 0;
	cvMinMaxLoc(FGMap, &min_val, &max_val);
	//IplImage* srcFrame2 = cvCreateImage(cvSize(srcFrame1->width, srcFrame1->height), IPL_DEPTH_8U, 3);	
	for (int y=0; y<srcFrame1->height; y++)
	for (int x=0; x<srcFrame1->width; x++)
	{
		int a = sizeof(float);
		float value = ((UCHAR*)(FGMap->imageData + FGMap->widthStep*y))[x];
		int index = NTEMPERATURES-(int)(NTEMPERATURES*(value-min_val-1)/(max_val-min_val))-1;
		//p->B = (unsigned char)(255*(value-min_val)/(max_val-min_val));
		//p->G = 255-p->B;
		Pixel* p = &HEAT_COLOR[index-1];
		if (index>0)
			OpenCVExtensions::Image::setPixel(blobImg, p, x, y);
	}
	Image::Overlay(srcFrame1, blobImg, tgtFrame, FGMap);
}
// --------------------------------------------------------------------------
void BackgroundModel::Draw(IplImage* frame) {
	// update each codeword
	Pixel* p = new Pixel();
	for (int y=0; y<frame->height; y++)
	for (int x=0; x<frame->width; x++)
	{
		this->GetAt(y*frame->width+x).GetRGB(p);
		Image::setPixel(frame, p, x, y);
	} 
	delete p;
}
// --------------------------------------------------------------------------
void BackgroundModel::Serialize( CArchive& ar ) {
	CArray<CodeBook>::Serialize( ar );
	if( ar.IsStoring()) {
		ar  << m_mode
			<< m_CDmax << m_BDmin << m_BDmax << m_initStage 
			<< m_Erode << m_ErodeSize << m_Dilate << m_DilateSize 
			<< m_staleCutoff << m_removeStaleFreq << m_trackAfterAge << m_trackAfterAgePercentage << m_minAge << m_updateFreq;
	}
	else {
		ar  >> m_mode
			>> m_CDmax >> m_BDmin >> m_BDmax >> m_initStage
			>> m_Erode >> m_ErodeSize >> m_Dilate >> m_DilateSize 
			>> m_staleCutoff >> m_removeStaleFreq >> m_trackAfterAge >> m_trackAfterAgePercentage >> m_minAge >> m_updateFreq;
	}
}
// --------------------------------------------------------------------------
int BackgroundModel::GetModelSizeInCodewords() {
	int size = 0;
	for (int i=0;i<GetSize();i++)
		size += this->GetAt(i).color.GetSize();
	return size;
}
// --------------------------------------------------------------------------