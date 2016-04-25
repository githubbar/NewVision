
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
#include "Globals.h"
#include "Image.h"
#include "CameraModel.h"   
#include "BodyModel.h"   
#include "BodyActivityModel.h"   
#include "NewVisionDoc.h"
#include "Body.h"
#include "BodyPath.h"
#using <mscorlib.dll>
#using <System.dll>
using namespace OpenCVExtensions;
using namespace System;
using namespace System::Diagnostics;

// Static controllables
double	Body::weights_scale = 255; // for the mean-shift kernel
double	Body::min_mean_shift_err = 5;
double	Body::max_mean_shift_iter = 10;

IMPLEMENT_DYNAMIC(Object3D, CObject)
IMPLEMENT_DYNAMIC(Obstacle, Object3D)
IMPLEMENT_DYNAMIC(Body, Object3D)

// --------------------------------------------------------------------------
Body::Body(CvPoint2D32f point, double height, double width, CNewVisionDoc* doc, int id, int frameNumber) {
	this->doc = doc;
	this->id = id;
	this->orientation = 0;	
	hc_prev = cvPoint2D32f(0,0);
	area_prev = cvRect(0,0,0,0);
	depthZ = 0;
	startFrame = frameNumber;
	stopFrame = frameNumber;
	invisibleFor = 0;
	dormantFor = 0;
	outsiderFor = 0;
	bot = cvPoint3D32f(point.x, point.y, 0);
	bot_prev = bot;
	top = cvPoint3D32f(point.x, point.y, height);
	predicted = bot;
	widthRatio = width/height;
	confidence = 0;
	pDoor = 1;
	pWidth = 1;
	pHeight = 1;
	pBodyOverlap = 1;
	pTempColor = 1;
	pTempWidth = 1;
	pTempHeight = 1;
	pTempPosition = 1;
	dHist = 0;
	m_dFlow = cvPoint2D32f(0,0);
	m_dAbsAvgFlow = cvPoint2D32f(0,0);
	KalmanInit();
}
// --------------------------------------------------------------------------
Body::Body(BodyPath* path, CNewVisionDoc* doc, int frameNumber) {
	this->doc = doc;
	this->id = path->id;
	this->orientation = 0;
	hc_prev = cvPoint2D32f(0,0);
	area_prev = cvRect(0,0,0,0);
	depthZ = 0;
	startFrame = frameNumber;
	stopFrame = frameNumber;
	invisibleFor = 0;
	dormantFor = 0;
	outsiderFor = 0;
	Step step = path->GetAt(frameNumber-path->startFrame);
	bot.x = step.p.x;
	bot.y = step.p.y;
	bot.z = 0;
	bot_prev = bot;
	top = cvPoint3D32f(bot.x, bot.y, step.height);
	widthRatio = step.width/step.height;
	orientation = step.orientation;
	predicted = bot;
	confidence = path->autoConfidence;
	dHist = 0;
	m_dFlow = cvPoint2D32f(0,0);
	m_dAbsAvgFlow = cvPoint2D32f(0,0);
	KalmanInit();
}
// --------------------------------------------------------------------------
Body::Body(BlobRay* r, CNewVisionDoc* doc, int id, int frameNumber) {
	this->doc = doc;
	this->id = id;
	this->orientation = 0;
	hc_prev = cvPoint2D32f(0,0);
	area_prev = cvRect(0,0,0,0);
	depthZ = 0;
	startFrame = frameNumber;
	stopFrame = frameNumber;
	invisibleFor = 0;
	dormantFor = 0;
	outsiderFor = 0;
	doc->cameramodel.coordsImage2RealSameXY_Feet2Floor(cvPointTo32f(r->p1), cvPointTo32f(r->p2), &top, &bot);
	
	// extreeme case, the body is under the camera in spherical model
	//if (top.z == 0 && bot.z == 0)
	//	top.z = (float)doc->bodymodel.m_heightM;
	bot = cvPoint3D32f(top.x, top.y, 0);
	bot_prev = bot;
	predicted = bot;
	widthRatio = INITIAL_WIDTH_RATIO ;
	confidence = 0;
	pDoor = 1;
	pWidth = 1;
	pHeight = 1;
	pBodyOverlap = 1;
	pTempColor = 1;
	pTempWidth = 1;
	pTempHeight = 1;
	pTempPosition = 1;
	dHist = 0;
	m_dFlow = cvPoint2D32f(0,0);
	m_dAbsAvgFlow = cvPoint2D32f(0,0);
	KalmanInit();
}
// --------------------------------------------------------------------------
Body::Body(void) {
	dHist = 0;
	m_dFlow = cvPoint2D32f(0,0);
	m_dAbsAvgFlow = cvPoint2D32f(0,0);
	this->posKalman = NULL;
	this->orientKalman = NULL;
}
// --------------------------------------------------------------------------
Body::~Body(void) {
	if (this->posKalman)
		cvReleaseKalman(&posKalman);
//	cvReleaseKalman(&orientKalman);
}
// --------------------------------------------------------------------------
Body::Body(const Body& that) {
	this->id = that.id;
	this->orientation = that.orientation;
	this->doc = that.doc;
	this->startFrame = that.startFrame;
	this->stopFrame = that.stopFrame;
	this->invisibleFor = that.invisibleFor;
	this->dormantFor = that.dormantFor;
	this->outsiderFor = that.outsiderFor;
	this->color = that.color;
	this->depthZ = that.depthZ;
	this->hc_prev = that.hc_prev;
	this->area_prev = that.area_prev;
	KalmanInit();
	KalmanCopy(that.posKalman, this->posKalman);
//	KalmanCopy(that.orientKalman, this->orientKalman);
	top = that.top;
	bot = that.bot;
	bot_prev = that.bot_prev;
	predicted = that.predicted;
	widthRatio = that.widthRatio;
	this->confidence = that.confidence;
	this->pDoor = that.pDoor;
	this->pWidth = that.pWidth;
	this->pHeight = that.pHeight;
	this->pBodyOverlap = that.pBodyOverlap;
	this->pTempColor = that.pTempColor;
	this->pTempWidth = that.pTempWidth;
	this->pTempHeight = that.pTempHeight;
	this->pTempPosition = that.pTempPosition;
	this->dHist = that.dHist;
	m_dFlow = that.m_dFlow;
	m_dAbsAvgFlow = that.m_dAbsAvgFlow;
}
// --------------------------------------------------------------------------
Body& Body::operator=(const Body& that) {
	this->id = that.id;
	this->orientation = that.orientation;
	this->doc = that.doc;
	this->startFrame = that.startFrame;
	this->stopFrame = that.stopFrame;
	this->invisibleFor = that.invisibleFor;
	this->dormantFor = that.dormantFor;
	this->outsiderFor = that.outsiderFor;
	this->color = that.color;
	this->depthZ = that.depthZ;
	this->hc_prev = that.hc_prev;
	this->area_prev = that.area_prev;
	if (this->posKalman == NULL)
		KalmanInit();
	KalmanCopy(that.posKalman, this->posKalman);
//	KalmanCopy(that.orientKalman, this->orientKalman);
	top = that.top;
	bot = that.bot;
	bot_prev = that.bot_prev;
	predicted = that.predicted;
	widthRatio = that.widthRatio;
	this->confidence = that.confidence;
	this->pDoor = that.pDoor;
	this->pWidth = that.pWidth;
	this->pHeight = that.pHeight;
	this->pBodyOverlap = that.pBodyOverlap;
	this->pTempColor = that.pTempColor;
	this->pTempWidth = that.pTempWidth;
	this->pTempHeight = that.pTempHeight;
	this->pTempPosition = that.pTempPosition;
	this->dHist = that.dHist;
	m_dFlow = that.m_dFlow;
	m_dAbsAvgFlow = that.m_dAbsAvgFlow;
	return *this;
}
// --------------------------------------------------------------------------
void Body::HistogramCompute(IplImage* frame, CvHistogram* hist, int accumulate) {
	CvSize2D32f	axes = GetAxes();

	// generate binary body outline mask
	cvZero(mask); 
	
	// TEMP: don't use weights, instead sample all pixels within the reduced ellipsoid
	axes.height *= 0.9;
	axes.width *= 0.9;
	// END TEMP

	// create FG mask
	doc->cameramodel.DrawVerticalSpheroid(mask, GetCenter(), axes, cvScalar(1), true);
	cvThreshold(body_zbuffer, weight, depthZ, 1, CV_THRESH_BINARY_INV);
	cvAnd(mask, weight, mask);

	// find bounding box
	CvSeq* contour = cvCreateSeq(CV_SEQ_ELTYPE_GENERIC, sizeof(CvContour), sizeof(CvPoint2D32f), OpenCVmemory);
	cvFindContours(mask, OpenCVmemory, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	
	// x or y out of bounds? => no histogram
	if (!contour)
		return;

	CvRect R = cvBoundingRect(contour);
	double s1 = R.width, s2 = R.height;
	int x1 = max(min(R.x, lab->width-1), 0);
	int y1 = max(min(R.y, lab->height-1), 0);
    int x2 = R.x+R.width; x2 = max(min(x2, lab->width-1), 0);
	int y2 = R.y+R.height; y2 = max(min(y2, lab->height-1), 0);

	//// create weights matrix
	//cvZero(weight);
	//for (int x = x1; x <= x2; x++) {
	//	for (int y = y1; y <= y2; y++) {
	//		if (mask->imageData[y*mask->widthStep+x] != 0 && fg->imageData[y*fg->widthStep+x] != 0) {
	//			// generate one element of anisotropic Gaussian kernel
	//			CvPoint3D32f p = doc->cameramodel.coordsImage2Real(cvPoint2D32f(x, y), bot, top);
	//			double squareDist = d_2(p, GetCenter(), cvPoint3D32f(axes.width, axes.width, axes.height));
	//			double k = exp(-squareDist);
	//			weight->imageData[y*weight->widthStep+x] = round(weights_scale*k);
	//		}
	//	}
	//}
	
	// compute weighted histogram
	cvSetImageROI(frame, R);
	cvSetImageROI(mask, R);
	cvSetImageROI(weight, R);
	cvSetImageROI(lab, R);
	cvSetImageROI(l_plane, R);
	cvSetImageROI(a_plane, R);
	cvSetImageROI(b_plane, R);

	cvCvtColor(frame, lab, CV_BGR2Lab);
	cvCvtPixToPlane( lab, l_plane, a_plane, b_plane, 0 );
	IplImage* planes[] = { a_plane, b_plane };
	//cvCalcHistWeighted(planes, hist, accumulate, mask, weight);
	cvCalcHist(planes, hist, accumulate, mask);

	cvResetImageROI(frame);
	cvResetImageROI(mask);
	cvResetImageROI(weight);
	cvResetImageROI(lab);
	cvResetImageROI(l_plane);
	cvResetImageROI(a_plane);
	cvResetImageROI(b_plane);
}
// --------------------------------------------------------------------------
void Body::HistogramUpdate(IplImage* frame) {
	HistogramCompute(frame, this->color.h, 1 /*accumulate values*/);
}
// --------------------------------------------------------------------------
void Body::KalmanCopy(CvKalman* src, CvKalman* dst) {
	cvCopy(src->state_post, dst->state_post);
	cvCopy(src->state_pre, dst->state_pre);
    cvCopy(src->transition_matrix, dst->transition_matrix);
	cvCopy(src->measurement_matrix, dst->measurement_matrix);
	cvCopy(src->process_noise_cov, dst->process_noise_cov);
	cvCopy(src->measurement_noise_cov, dst->measurement_noise_cov);
	cvCopy(src->error_cov_post, dst->error_cov_post);
	cvCopy(src->error_cov_pre, dst->error_cov_pre);
}
// --------------------------------------------------------------------------
void Body::KalmanInit() {
	// ---------- Initialize posKalman
	// state is [x,y, dx,dy], measurement is [x,y]
	posKalman = cvCreateKalman( 4, 2, 0 );
	const float A[] = { 1, 0, 1, 0,
						0, 1, 0, 1,
						0, 0, 1, 0,
						0, 0, 0, 1 };
	const float H[] = { 1, 0, 0, 0,
						0, 1, 0, 0 };
	memcpy(posKalman->transition_matrix->data.fl, A, sizeof(A));
	memcpy(posKalman->measurement_matrix->data.fl, H, sizeof(H));
    cvSetIdentity( posKalman->process_noise_cov, cvRealScalar(doc->bodymodel.m_posModelNoise) );
    cvSetIdentity( posKalman->measurement_noise_cov, cvRealScalar(doc->bodymodel.m_posMeasurementNoise) );
	//cvSetIdentity( posKalman->error_cov_pre, cvRealScalar(1));
    //cvSetIdentity( posKalman->error_cov_post, cvRealScalar(1));
	
	// set the initial state to be the top position with 0 velocity
	cvmSet(posKalman->state_pre, 0, 0, bot.x);
	cvmSet(posKalman->state_pre, 1, 0, bot.y);
	cvmSet(posKalman->state_pre, 2, 0, 0);
	cvmSet(posKalman->state_pre, 3, 0, 0);
}
// --------------------------------------------------------------------------
void Body::KalmanPredict() {
	double PREV_ORIENT_WEIGHT =  0.5;
	double FACE_ORIENT_WEIGHT = 0.3;
	double MOTION_ORIENT_WEIGHT = 0.7;

	// ========== Predict Position =============================
	const CvMat* pPrediction = cvKalmanPredict( posKalman );
	predicted = cvPoint3D32f((float)cvmGet(pPrediction,0,0), (float)cvmGet(pPrediction,1,0), 0);


	// ========== Predict face orientation ======================
	// 1 - Find image coordinates of the face center by skin-color matching
	double Pface = 0, faceOrient = 0;
	CvPoint3D32f faceCenter = GetFaceCenter(imgFrame, Pface); 
	// 2 - Find image coordinates of the head center
	CvPoint3D32f headCenter = GetHeadCenter();
	// 3 - Find probability
	Pface *= d(faceCenter,headCenter)/max(GetHeight()*HEAD_HEIGHT, GetWidth()*HEAD_WIDTH);
	// 4 - Compute face orientation angle in the image 
	faceOrient = angle(cvPoint2D32f(headCenter.x, headCenter.y), cvPoint2D32f(faceCenter.x, faceCenter.y));

	// ========== Predict motion orientation ======================
	double Pmotion = 0, motionOrient = 0;
	motionOrient = angle(cvPoint2D32f(bot.x, bot.y), cvPoint2D32f(predicted.x, predicted.y));
	BodyPath* bp;
	if (!doc->m_TrackingData.data.Lookup(id, bp)) {
		//AfxMessageBox("Error: Body path not found!");
		return;
	}
	int SMOOTHBY = 10;
	motionOrient = bp->GetMotionAngle(doc->trackermodel.m_frameNumber, SMOOTHBY); 
	Pmotion = min(1, d(cvPoint2D32f(bot.x, bot.y), cvPoint2D32f(predicted.x, predicted.y))/MAX_VELOCITY);

	// ====== Combine Face and Motion Orientation with the Past Measurements ========
	double Pall = (Pface*FACE_ORIENT_WEIGHT+Pmotion*MOTION_ORIENT_WEIGHT);
	if (Pall == 0)
		return;
	double cc = PREV_ORIENT_WEIGHT*cos(orientation) + (1 - PREV_ORIENT_WEIGHT)*
		(Pface*FACE_ORIENT_WEIGHT*cos(faceOrient) + Pmotion*MOTION_ORIENT_WEIGHT*cos(motionOrient))/Pall;
	double ss = PREV_ORIENT_WEIGHT*sin(orientation) + (1 - PREV_ORIENT_WEIGHT)*
		(Pface*FACE_ORIENT_WEIGHT*sin(faceOrient) + Pmotion*MOTION_ORIENT_WEIGHT*sin(motionOrient))/Pall;
	if (cc != 0 || ss != 0)
		orientation = atan2(ss, cc);

	if (id==2)
		echo(String::Format(" id = {0}; Pface = {1} aface = {2} Pmotion = {3} amotion = {4}",id, Pface, faceOrient, Pmotion, motionOrient));

	//echo(String::Concat( S" id = ", __box(id), S":  err(x) = ", __box(posKalman->error_cov_pre->data.fl[0]), S":  err(y) = ", __box(posKalman->error_cov_pre->data.fl[2])));
	//echo(String::Concat( S" id = ", __box(id), S":  ", __box(faceOrient/PI), S", ", __box(faceAbs), S", ", __box(motionOrient/PI), S", ", __box(motionAbs), S" => ", __box(orientation)));
	//echo(String::Concat( S" id = ", __box(id), S":  ", __box(orOld/PI), S"->", __box(orientation/PI), S""));
}
// --------------------------------------------------------------------------
void Body::KalmanObserve() {
	// observe position
	CvMat* oPosition = cvCreateMat(2,1,CV_32FC1); 
	cvmSet(oPosition,0,0,bot.x);cvmSet(oPosition,1,0,bot.y);
	cvKalmanCorrect(posKalman, oPosition);
	cvReleaseMat(&oPosition);
}
// --------------------------------------------------------------------------
void Body::MoveTo(CvPoint3D32f foot) {
	// move body to a new position 
	bot = foot;
	top = cvPoint3D32f(foot.x, foot.y, top.z);
}
// --------------------------------------------------------------------------
void Body::MoveHeadTo(CvPoint3D32f head) {
	// move body to a new position 
	top = head;
	bot = cvPoint3D32f(head.x, head.y, bot.z);
}
// --------------------------------------------------------------------------
double Body::GetHeight() {
	return top.z;
}
// --------------------------------------------------------------------------
void Body::SetHeight(double h) {
	top.z = (float)h;
}
// --------------------------------------------------------------------------
double Body::GetWidth() {
	return top.z*widthRatio;
}
// --------------------------------------------------------------------------
void Body::SetWidth(double w) {
	widthRatio = (float)(w/top.z);
}
// --------------------------------------------------------------------------
CvPoint3D32f Body::MeanShift(IplImage* frame, IplImage* FGmask) {
	if (cvCountNonZero(FGmask) == 0)
		return bot;

	float min_value, max_value;
	cvGetMinMaxHistValue(color.h, &min_value, &max_value);
	if (max_value == 0)
		return bot;
	
	cvCvtColor(frame, lab, CV_BGR2Lab);
	CvPoint3D32f foot, foot_new = bot;

	//if (OutOfBounds(GetHead(), frame) || OutOfBounds(GetFloor(), frame)) 
	//	return;

	int i=0;
	do {
		// move to the new center
		foot = foot_new;
		foot_new = MeanShiftGetCenter(lab, FGmask, foot);
		// ignore out of bounds bodies (TODO: what to do with partially out of bounds bodies?)
		//if (OutOfBounds(center_new, frame))
		//	return GetCenter();
		i++;
	} while (d(foot, foot_new) > min_mean_shift_err && i < max_mean_shift_iter);
	
	//// empty blob => norm is zero
	//if (Double::IsNaN(foot_new.x))
		
	
	// return the new bot position
	return cvPoint3D32f(foot_new.x, foot_new.y, bot.z);
}
// --------------------------------------------------------------------------
CvPoint2D32f Body::MeanShiftImage(IplImage* frame, IplImage* FGmask) {
	CvPoint3D32f center = GetCenter();
	CvPoint2D32f imcenter = doc->cameramodel.coordsReal2Image(center);
	CvPoint2D32f imcenter_new = imcenter;

	if (cvCountNonZero(FGmask) == 0)
		return imcenter;

	float min_value, max_value;
	cvGetMinMaxHistValue(color.h, &min_value, &max_value);
	if (max_value == 0)
		return imcenter;

	cvCvtColor(frame, lab, CV_BGR2Lab);

	//if (OutOfBounds(GetHead(), frame) || OutOfBounds(GetFloor(), frame)) 
	//	return;

	int i=0;
	do {
		// move to the new center
		imcenter = imcenter_new;
		imcenter_new = MeanShiftGetImageCenter(lab, FGmask, imcenter);
		// ignore out of bounds bodies (TODO: what to do with partially out of bounds bodies?)
		//if (OutOfBounds(center_new, frame))
		//	return GetCenter();
		i++;
	} while (d(imcenter, imcenter_new) > min_mean_shift_err && i < max_mean_shift_iter);

	// return the new bot position
	return imcenter_new;
}
// --------------------------------------------------------------------------
CvPoint2D32f Body::MeanShiftGetImageCenter(IplImage* lab, IplImage* FGmask, CvPoint2D32f imcenter) {
	CvSize2D32f imaxes = GetImageAxes();
	CvRect R; 
	R.x = imcenter.x - imaxes.width/2;
	R.y = imcenter.y - imaxes.height/2;
	R.width = imaxes.width;
	R.height = imaxes.height;
	int x1 = max(min(R.x, lab->width-1), 0);
	int y1 = max(min(R.y, lab->height-1), 0);
	int x2 = R.x+R.width; x2 = max(min(x2, lab->width-1), 0);
	int y2 = R.y+R.height; y2 = max(min(y2, lab->height-1), 0);
	// compute histogram
	double x0 = 0, y0 = 0, norm = 0;

	for (int x = x1; x <= x2; x++) {
		for (int y = y1; y <= y2; y++) {
			if (mask->imageData[y*mask->widthStep+x] != 0) {
				// generate one element of anisotropic Gaussian kernel
				double squareDist = d_2(cvPoint2D32f(x, y), imcenter, cvPoint2D32f(imaxes.width, imaxes.height));

				double k = exp(-squareDist);
				double a = (byte)lab->imageData[y*lab->widthStep+x+1];
				double b = (byte)lab->imageData[y*lab->widthStep+x+2];
				double h = color.GetValue(a, b);
				if (h) {
					x0 += k*h*x;
					y0 += k*h*y;
					norm += k*h;
					//x0 += x;
					//y0 += y;
					//norm += 1;
				}
			}
		}
	}
	if (norm == 0)
		return imcenter;

	x0 /= norm; y0 /= norm;
	return cvPoint2D32f(x0, y0);
}
// --------------------------------------------------------------------------
CvPoint3D32f Body::MeanShiftGetCenter(IplImage* lab, IplImage* FGmask, CvPoint3D32f foot) {
	bool useFGmask = false; // do not use foreground mask
	CvPoint3D32f center = cvPoint3D32f(foot.x, foot.y, foot.z + abs(top.z-bot.z)/2);
	CvPoint3D32f head = cvPoint3D32f(foot.x, foot.y, foot.z + abs(top.z-bot.z));
	CvPoint2D32f imcenter = doc->cameramodel.coordsReal2Image(center);
	CvPoint2D32f imhead = doc->cameramodel.coordsReal2Image(head);
	CvPoint2D32f imfoot = doc->cameramodel.coordsReal2Image(foot);
	CvSize2D32f	axes = GetAxes();

	// generate binary body outline mask
	cvZero(mask); 
	doc->cameramodel.DrawVerticalSpheroid(mask, center, GetAxes(), cvScalar(1), true);
	// find bounding box
	CvSeq* contour = cvCreateSeq(CV_SEQ_ELTYPE_GENERIC, sizeof(CvContour), sizeof(CvPoint2D32f), OpenCVmemory);
	cvFindContours(mask, OpenCVmemory, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	
	// x or y out of bounds? => no histogram
	if (!contour)
		return foot;

	ASSERT(contour);
		
	CvRect R = cvBoundingRect(contour);
	double s1 = R.width, s2 = R.height;
	int x1 = max(min(R.x, lab->width-1), 0);
	int y1 = max(min(R.y, lab->height-1), 0);
    int x2 = R.x+R.width; x2 = max(min(x2, lab->width-1), 0);
	int y2 = R.y+R.height; y2 = max(min(y2, lab->height-1), 0);
	// compute histogram
	double x0 = 0, y0 = 0, norm = 0;

	for (int x = x1; x <= x2; x++) {
		for (int y = y1; y <= y2; y++) {
			if (mask->imageData[y*mask->widthStep+x] != 0) {
				// generate one element of anisotropic Gaussian kernel
				CvPoint3D32f p = doc->cameramodel.coordsImage2Real(cvPoint2D32f(x, y), foot, head);
				double squareDist = d_2(p, center, cvPoint3D32f(axes.width, axes.width, axes.height));

				double k = exp(-squareDist);
				double a = (byte)lab->imageData[y*lab->widthStep+x+1];
				double b = (byte)lab->imageData[y*lab->widthStep+x+2];
				double h = color.GetValue(a, b);
				if (h && (!useFGmask || FGmask->imageData[y*FGmask->widthStep+x] != 0)) {
					//x0 += k*h*x;
					//y0 += k*h*y;
					//norm += k*h;
					x0 += x;
					y0 += y;
					norm += 1;
				}
			}
		}
	}
	if (norm == 0)
		return foot;

	x0 /= norm; y0 /= norm;
	center = doc->cameramodel.coordsImage2Real(cvPoint2D32f(x0, y0), center.z);
	return cvPoint3D32f(center.x, center.y, foot.z);
}
// --------------------------------------------------------------------------
double Body::GetVisiblePixelSize() {
	// apply exclusion mask formed by other bodies
	cvThreshold(body_zbuffer, weight, depthZ, 1, CV_THRESH_BINARY_INV);
	// superimpose binary body outline 
	cvZero(mask);RasterizeFrame(mask, cvScalar(1));
	cvMul(mask, weight, mask);
	return cvCountNonZero(mask);
}
// --------------------------------------------------------------------------
CvPoint3D32f Body::GetHead() {
	return top;
};
// --------------------------------------------------------------------------
CvPoint3D32f Body::GetFoot() {
	return bot;
}
// --------------------------------------------------------------------------
CvPoint3D32f Body::GetCenter() {
	return cvPoint3D32f((top.x+bot.x)/2, (top.y+bot.y)/2, (top.z+bot.z)/2);
}
// --------------------------------------------------------------------------
CvPoint2D32f Body::GetImageFaceCenter(IplImage* frame, double& nMatches) {
	// create a mask of visible pixels
	cvZero(mask); 
	RasterizeHead(mask, cvScalar(1));
	cvThreshold(body_zbuffer, weight, depthZ, 1, CV_THRESH_BINARY_INV);
	cvAnd(mask, weight, mask);

	// find the center of skin color region
	CvPoint2D32f p = doc->skinmodel.GetSkinAreaCenter(frame, mask, nMatches);

	if (p.x == -1 && p.y == -1) // no skin found in head region
		return GetImageHeadCenter();
	else
		return p;
}
// --------------------------------------------------------------------------
CvPoint2D32f Body::GetImageHeadCenter() {
	return doc->cameramodel.coordsReal2Image(GetHeadCenter());
}
// --------------------------------------------------------------------------
CvPoint2D32f Body::GetImageHeadOpticalFlow(IplImage* frame) {
	CvPoint2D32f hc = GetImageHeadCenter();
	if (hc_prev.x == 0)
		hc_prev = hc;
	CvPoint2D32f ht = GetImageHead();
	double R = d(hc, ht);
	const double BLOCKS_PER_HEAD = 5;
	int block_sz = max(1, (int)(2*R/BLOCKS_PER_HEAD));
	CvRect area = cvRect((int)(hc.x-R), (int)(hc.y-R), (int)(2*R), (int)(2*R));
	CvRect area_prev = cvRect((int)(hc_prev.x-R), (int)(hc_prev.y-R), (int)(2*R), (int)(2*R));
	cvCvtColor(frame, a_plane, CV_RGB2GRAY);
	cvCvtColor(imgPreviousFrame, b_plane, CV_RGB2GRAY);
	cvSetImageROI(a_plane, area);
	cvSetImageROI(b_plane, area_prev);
	cvSetImageROI(velx, area);
	cvSetImageROI(vely, area);
	cvCalcOpticalFlowLK(a_plane, b_plane, cvSize(block_sz, block_sz), velx, vely);
	double flowX = cvAvg(velx).val[0];
	double flowY = cvAvg(vely).val[0];
	cvResetImageROI(a_plane);
	cvResetImageROI(b_plane);
	cvResetImageROI(velx);
	cvResetImageROI(vely);
	hc_prev = hc;
	return cvPoint2D32f(flowX, flowY);
}
// --------------------------------------------------------------------------
CvPoint2D32f Body::GetImageOpticalFlow(IplImage* frame) {
	CvRect area = GetBoundingRect();
	//area.width % 2 != 0 ? area.width -=1 : 1;
	//area.height % 2 != 0 ? area.height -=1 : 1;

	const double BLOCK_SIZE = 5;
	int test = 1;
	if (area.width < BLOCK_SIZE || area.height < BLOCK_SIZE) {
		return cvPoint2D32f(0,0);
	}

	cvCvtColor(frame, a_plane, CV_RGB2GRAY);
	cvCvtColor(imgPreviousFrame, b_plane, CV_RGB2GRAY);
	cvSetImageROI(a_plane, area);
	cvSetImageROI(b_plane, area);
	cvSetImageROI(velx, area);
	cvSetImageROI(vely, area);
	cvCalcOpticalFlowLK(a_plane, b_plane, cvSize(BLOCK_SIZE, BLOCK_SIZE), velx, vely);
	double flowX = cvAvg(velx).val[0];
	double flowY = cvAvg(vely).val[0];
	cvResetImageROI(a_plane);
	cvResetImageROI(b_plane);
	cvResetImageROI(velx);
	cvResetImageROI(vely);
	area_prev = area;
	return cvPoint2D32f(flowX, flowY);
}
// --------------------------------------------------------------------------
CvPoint3D32f Body::GetFaceCenter(IplImage* frame, double& nMatches) {
	// find the center of skin color region
	CvPoint2D32f faceCenter = GetImageFaceCenter(frame, nMatches);
	return doc->cameramodel.coordsImage2Real(faceCenter, (1-HEAD_HEIGHT/2)*top.z + HEAD_HEIGHT/2*bot.z);
}
// --------------------------------------------------------------------------
CvPoint3D32f Body::GetHeadCenter() {
	return cvPoint3D32f(top.x, top.y, (1-HEAD_HEIGHT/2)*top.z + HEAD_HEIGHT/2*bot.z);
}
// --------------------------------------------------------------------------
CvPoint2D32f Body::GetImageHead() {
	return doc->cameramodel.coordsReal2Image(top);
};
// --------------------------------------------------------------------------
CvPoint2D32f Body::GetImageFoot() {
	return doc->cameramodel.coordsReal2Image(bot);
}
// --------------------------------------------------------------------------
CvPoint2D32f Body::GetImageCenter() {
	return doc->cameramodel.coordsReal2Image(GetCenter());
}
// --------------------------------------------------------------------------
CvSize2D32f Body::GetAxes() {
	double h = abs(top.z-bot.z)/2;
	double w = widthRatio*h;
	return cvSize2D32f((float)w, (float)h);
}
// --------------------------------------------------------------------------
CvSize2D32f Body::GetImageAxes() {
	CvRect r = GetBoundingRect();
	return cvSize2D32f(r.width, r.height);
}
// --------------------------------------------------------------------------
double Body::GetDistanceFromCamera() {
	// get the depth of the center of the body
	return doc->cameramodel.coordsDistanceFromCamera(GetCenter());
}
// --------------------------------------------------------------------------
void Body::GetTagRect(CvRect& r) {
	CvFont  font;
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1.0f, 1.0f);
	CvSize sz; int baseline; const OFFSET = 3;
	CString text; text.Format("%-d", (int)id);
	cvGetTextSize(text, &font, &sz, &baseline );
	r.x = (int)GetImageHead().x - 5 - OFFSET;
	r.y = (int)GetImageHead().y - 10 - OFFSET;
	r.width = sz.width+2*OFFSET;
	r.height = sz.height+2*OFFSET;
}
// --------------------------------------------------------------------------
void Body::DrawFrame(IplImage* frame, CvScalar color) {
	doc->cameramodel.DrawVerticalSpheroid(frame, GetCenter(), GetAxes(), color);
	drawTextInRectangle(id, cvPoint((int)GetImageHead().x-5, (int)GetImageHead().y-20), frame, CV_RGB(255,0,0), CV_RGB(255,255,255), 1.0f);
	//drawTextInRectangle(round(confidence*1000), cvPoint((int)GetImageFoot().x-5, (int)GetImageFoot().y), frame, CV_RGB(0,0,0), CV_RGB(255,255,255), 0.7f);	
	//drawText(m_dAbsAvgFlow.x/GetImageAxes().width, cvPoint((int)GetImageFoot().x-5, (int)GetImageFoot().y), frame, CV_RGB(0,0,255), CV_RGB(0,0,55), 0.7f);	
	//drawText(m_dAbsAvgFlow.y/GetImageAxes().height, cvPoint((int)GetImageFoot().x-5, (int)GetImageFoot().y+15), frame, CV_RGB(0,0,255), CV_RGB(0,0,55), 0.7f);	
	//drawTextInRectangle(100*dHist, cvPoint((int)GetImageFoot().x-5, (int)GetImageFoot().y), frame, CV_RGB(0,0,0), CV_RGB(255,255,255), 0.7f);


	//CvRect bRect = GetBoundingRect();
	//cvRectangle(frame, cvPoint(bRect.x, bRect.y), cvPoint(bRect.x+bRect.width, bRect.y+bRect.height), CV_RGB(0, 128, 200), 1, CV_AA);
	//cvRectangle(frame, p1, p2, CV_RGB(200, 128, 0), 1, CV_AA); // TEST: draw loaded bounding rect

	//drawTextInRectangle((int)doc->cameramodel.coordsDistanceFromCamera(this->predicted), cvPoint((int)GetImageHead().x-5, (int)GetImageHead().y-10), frame, CV_RGB(0,0,0), CV_RGB(255,255,255), 0.7f);
	//drawTextInRectangle((int)doc->cameramodel.coordsDistanceFromCamera(this->bot), cvPoint((int)GetImageFoot().x-5, (int)GetImageFoot().y), frame, CV_RGB(0,0,0), CV_RGB(255,255,255), 0.7f);

	//drawTextInRectangle((int)GetDistanceFromCamera(), cvPoint((int)GetImageFoot().x-5, (int)GetImageFoot().y), frame, CV_RGB(0,0,0), CV_RGB(255,255,255), 0.7f);
	//char s[20]; sprintf(s, "x=%d y=%d", (int)bot.x, (int)bot.y);
	//drawTextInRectangle(s, cvPoint((int)GetImageFoot().x-5, (int)GetImageFoot().y), frame, CV_RGB(0,0,0), CV_RGB(255,255,255), 0.7f);
	//if (d(bot, predicted) < doc->bodymodel.m_maxPosChange)
	
	//DrawHead(frame, CV_RGB(255,0,0));
	//DrawFoot(frame, CV_RGB(0,255,0));
	//DrawHeadArea(frame, CV_RGB(255,200,150));
	//DrawHeadCenter(frame, CV_RGB(255,0,255));
	//DrawFaceCenter(frame, CV_RGB(255,255,0));
	//DrawCurrentVsPredicted(frame, color);
	//DrawOrientationArrow(frame, color);
}
// --------------------------------------------------------------------------
void Body::DrawFrame(CDC* pDC , int ox, int oy, CPen* pen )
{
	// draw vertexes
	CPen* pOldPen = pDC->SelectObject(pen);
	doc->cameramodel.DrawVerticalSpheroid(pDC, GetCenter(), GetAxes());
	double size = 1.0;
	CPen   *penLabel = new CPen(PS_SOLID, 1, RGB(255,0,0));
	CBrush *brushLabel = new CBrush(RGB(255,255,255));
	pDC->SelectObject(penLabel);
	CBrush* pOldBrush = pDC->SelectObject(brushLabel);
	drawTextInRectangle(id, cvPoint((int)GetImageHead().x-5, (int)GetImageHead().y-10), pDC, size);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	delete penLabel;
	delete brushLabel;
	return;	
}
// --------------------------------------------------------------------------
void Body::DrawFrameLabel(IplImage* frame, CvScalar labelFG, CvScalar labelBG, double labelSize) {
	//doc->cameramodel.DrawVerticalSpheroid(frame, GetCenter(), GetAxes(), color);
	drawTextInRectangle(id, cvPoint((int)GetImageHead().x-5, (int)GetImageHead().y-10), frame, labelFG, labelBG, labelSize);
}
// --------------------------------------------------------------------------
void Body::DrawFloor(IplImage* frame, CvScalar color) {
	CvPoint xy = doc->floormodel.coordsReal2Floor(bot);
	drawTextInCircle(id, xy, frame, CV_RGB(0,0,0), color, 0.8f);
}
// --------------------------------------------------------------------------
void Body::DrawHeight(IplImage* frame, CvScalar color) {
	drawTextInRectangle((int)GetHeight(), cvPoint((int)GetImageHead().x-5, (int)GetImageHead().y+5), frame, CV_RGB(0,0,0), CV_RGB(255,255,255), 0.7f);
}
// --------------------------------------------------------------------------
void Body::DrawInfo(IplImage* frame, CvScalar color) {
	drawTextInRectangle(String::Format("door: {0:f2}", pDoor), cvPoint((int)GetImageHead().x-5, (int)GetImageHead().y-70), frame, color, CV_RGB(0,0,0), 0.7f);
	drawTextInRectangle(String::Format("over: {0:f2}", pBodyOverlap), cvPoint((int)GetImageHead().x-5, (int)GetImageHead().y-60), frame, color, CV_RGB(0,0,0), 0.7f);
	//drawTextInRectangle(String::Format("W/H: {0:f2}/{0:f2}", pWidth, pHeight), cvPoint((int)GetImageHead().x-5, (int)GetImageHead().y-50), frame, color, CV_RGB(0,0,0), 0.7f);
	drawTextInRectangle(String::Format("~Col: {0:f2}", pTempColor), cvPoint((int)GetImageHead().x-5, (int)GetImageHead().y-50), frame, color, CV_RGB(0,0,0), 0.7f);
	//drawTextInRectangle(String::Format("~W/H: {0:f2}/{0:f2}", pTempWidth, pTempHeight), cvPoint((int)GetImageHead().x-5, (int)GetImageHead().y-30), frame, color, CV_RGB(0,0,0), 0.7f);
	drawTextInRectangle(String::Format("~Pos: {0:f2}", pTempPosition), cvPoint((int)GetImageHead().x-5, (int)GetImageHead().y-40), frame, color, CV_RGB(0,0,0), 0.7f);
	drawTextInRectangle(String::Format("All: {0:f2}", confidence), cvPoint((int)GetImageHead().x-5, (int)GetImageHead().y-30), frame, CV_RGB(100,color.val[1],color.val[2]), CV_RGB(0,0,0), 0.7f);
}
// --------------------------------------------------------------------------
void Body::DrawCurrentVsPredicted(IplImage* frame, CvScalar color) {
	//CvPoint pr = cvPointFrom32f(doc->cameramodel.coordsReal2Image(predicted));
	//CvPoint pr = cvPointFrom32f(doc->cameramodel.coordsReal2Image(MeanShift(frame, fg)));
	//cvCircle(frame, cvPointFrom32f(GetImageFoot()), 3, CV_RGB(255,255,0), CV_FILLED);
	//cvCircle(frame, pr, 1, CV_RGB(255,0,0), CV_FILLED);

	CvPoint3D32f center = GetCenter();
	CvPoint2D32f imcenter = doc->cameramodel.coordsReal2Image(center);
	CvPoint2D32f imcenternew = MeanShiftImage(frame, fg);

	cvCircle(frame, cvPointFrom32f(imcenter), 3, CV_RGB(0,55,255), CV_FILLED);
	cvCircle(frame, cvPointFrom32f(imcenternew), 1, CV_RGB(0,255,0), CV_FILLED);
}
// --------------------------------------------------------------------------
void Body::DrawOrientationArrow(IplImage* frame, CvScalar color) {
	CvPoint3D32f begin = GetFoot();
	CvPoint3D32f end = cvPoint3D32f(begin.x+cos(orientation)*ARROW_LENGTH,
		begin.y+sin(orientation)*ARROW_LENGTH, 0);
	if (begin.y*end.y > 0)
		cvLine(frame, 
			cvPointFrom32f(doc->cameramodel.coordsReal2Image(begin)), 
			cvPointFrom32f(doc->cameramodel.coordsReal2Image(end)), color);
	else 
		cvLine(frame, 
			cvPointFrom32f(doc->cameramodel.coordsReal2Image(begin)), 
			cvPointFrom32f(doc->cameramodel.coordsReal2Image(end)), color);

	cvCircle(frame, cvPointFrom32f(doc->cameramodel.coordsReal2Image(end)), 2, CV_RGB(255,0,0), 1);
}
// --------------------------------------------------------------------------
void Body::DrawSectorHighlight(IplImage* srcFrame, IplImage* tgtFrame) {
	if(Double::IsNaN(orientation) || Double::IsInfinity(orientation) || Double::IsNegativeInfinity(orientation))
		return;
	CvPoint3D32f begin = GetFoot();
	CvPoint3D32f end1 = cvPoint3D32f(begin.x+cos(orientation - SECTOR_ANGLE)*ARROW_LENGTH,
		begin.y+sin(orientation - SECTOR_ANGLE)*ARROW_LENGTH, 0);
	CvPoint3D32f end2 = cvPoint3D32f(begin.x+cos(orientation + SECTOR_ANGLE)*ARROW_LENGTH,
		begin.y+sin(orientation + SECTOR_ANGLE)*ARROW_LENGTH, 0);
	CvPoint imbegin = cvPointFrom32f(doc->cameramodel.coordsReal2Image(begin));
	CvPoint imend1 = cvPointFrom32f(doc->cameramodel.coordsReal2Image(end1));
	CvPoint imend2 = cvPointFrom32f(doc->cameramodel.coordsReal2Image(end2));
	//double Y_CUTOFF = 0.9;
	//if (   imbegin.y > Y_CUTOFF*doc->cameramodel.h
	//	|| imend1.y > Y_CUTOFF*doc->cameramodel.h
	//	|| imend2.y > Y_CUTOFF*doc->cameramodel.h)
	//	return;
	double X_CUTOFF = 0.5;
	int w = doc->cameramodel.w-1;
	if ( abs(imbegin.x-imend1.x) > X_CUTOFF*w ) {
		if (imbegin.x > w/2) {
			imend1.y = 1.0*(-imbegin.x)*(imend1.y-imbegin.y)/(imend1.x-imbegin.x)+imbegin.y;
			imend1.x = w;
		}
		else {
			imend1.y = 1.0*(0-imbegin.x)*(imend1.y-imbegin.y)/(imend1.x-imbegin.x)+imbegin.y;
			imend1.x = 0;
		}
	}
	if ( abs(imbegin.x-imend2.x) > X_CUTOFF*w ) {
		if (imbegin.x > w/2) {
			imend2.y = 1.0*(-imbegin.x)*(imend2.y-imbegin.y)/(imend2.x-imbegin.x)+imbegin.y;
			imend2.x = w;
		}
		else {
			imend2.y = 1.0*(0-imbegin.x)*(imend2.y-imbegin.y)/(imend2.x-imbegin.x)+imbegin.y;
			imend2.x = 0;
		}
	}
	CvPoint pts[3] = {imbegin, imend1, imend2};
	int nPts = 3;
	cvZero(mask);
	cvFillConvexPoly(mask, pts, nPts, cvScalar(1), CV_AA);

	// highlight the masked region
	Pixel p(50, 50, 100);
	Image::Highlight(srcFrame, tgtFrame, mask, &p);
	cvLine(tgtFrame, imbegin, imend1, CV_RGB(255,0,255));
	cvLine(tgtFrame, imbegin, imend2, CV_RGB(255,0,255));
}
// --------------------------------------------------------------------------
void Body::DrawHeadArea(IplImage* frame, CvScalar color) {
	CvSize2D32f sz = GetAxes();
	sz.height *= (float)HEAD_HEIGHT; sz.width *= (float)HEAD_WIDTH;
	CvPoint3D32f ct = top;
	ct.z = (float)((1-HEAD_HEIGHT/2)*top.z + HEAD_HEIGHT/2*bot.z);
	doc->cameramodel.DrawVerticalSpheroid(frame, ct, sz, color);
}
// --------------------------------------------------------------------------
void Body::DrawHeadCenter(IplImage* frame, CvScalar color) {
	cvCircle(frame, cvPointFrom32f(GetImageHeadCenter()), 1, color, CV_FILLED);
}
// --------------------------------------------------------------------------
void Body::DrawHead(IplImage* frame, CvScalar color) {
	cvCircle(frame, cvPointFrom32f(GetImageHead()), 2, color, CV_FILLED);
}
// --------------------------------------------------------------------------
void Body::DrawFoot(IplImage* frame, CvScalar color) {
	cvCircle(frame, cvPointFrom32f(GetImageFoot()), 2, color, CV_FILLED);
}
// --------------------------------------------------------------------------
void Body::DrawFaceCenter(IplImage* frame, CvScalar color) {
	double nMatches = 0;
	cvCircle(frame, cvPointFrom32f(GetImageFaceCenter(imgFrame, nMatches)), 1, color, CV_FILLED);
}
// --------------------------------------------------------------------------
void Body::DrawSalesRepMark(IplImage* frame, CvScalar color) {
	drawTextInCircle("R", cvPoint((int)GetImageCenter().x-5, (int)GetImageCenter().y-10), frame, color, CV_RGB(255,255,255), 0.8f);
}
// --------------------------------------------------------------------------
void Body::DrawSalesRepTempMark(IplImage* frame, CvScalar color) {
	drawTextInCircle("R?", cvPoint((int)GetImageCenter().x-5, (int)GetImageCenter().y-10), frame, color, CV_RGB(255,255,255), 0.8f);
}
// --------------------------------------------------------------------------
void Body::DrawDwellMark(IplImage* frame, CvScalar color) {
	drawTextInCircle("?", cvPoint((int)GetImageHead().x-15, (int)GetImageHead().y-10), frame, color, CV_RGB(255,255,255), 0.8f);
}
// --------------------------------------------------------------------------
void Body::RasterizeFrame(IplImage* frame, CvScalar color) {
	doc->cameramodel.DrawVerticalSpheroid(frame, GetCenter(), GetAxes(), color, true);
}
// --------------------------------------------------------------------------
void Body::RasterizeFloor(IplImage* frame, CvScalar color) {
	CvPoint pf = doc->floormodel.coordsReal2Floor(bot);
	CvSize axes = doc->floormodel.sizeReal2Floor(cvSize2D32f(GetWidth(), GetWidth())); 
	cvEllipse(frame, pf, axes, 0, 0, 360, color, CV_FILLED);
}
// --------------------------------------------------------------------------
void Body::RasterizeHead(IplImage* frame, CvScalar color) {
	CvSize2D32f sz = GetAxes();
	sz.height *= (float)HEAD_HEIGHT; sz.width *= (float)HEAD_WIDTH;
	CvPoint3D32f ct = top;
	ct.z = (float)((1-HEAD_HEIGHT/2)*top.z + HEAD_HEIGHT/2*bot.z);
	doc->cameramodel.DrawVerticalSpheroid(frame, ct, sz, color, true);
}

// --------------------------------------------------------------------------
void Body::DrawHistogram(IplImage* frame, int scale, CvPoint p) {
	color.Draw(frame, scale, p);
}
// --------------------------------------------------------------------------
void Body::DrawWeightMask(IplImage* frame) {
	CvSize2D32f	axes = GetAxes();

	// generate binary body outline mask
	cvZero(mask); 
	doc->cameramodel.DrawVerticalSpheroid(mask, GetCenter(), axes, cvScalar(1), true);

	// find bounding box
	CvSeq* contour = cvCreateSeq(CV_SEQ_ELTYPE_GENERIC, sizeof(CvContour), sizeof(CvPoint2D32f), OpenCVmemory);
	cvFindContours(mask, OpenCVmemory, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	
	// x or y out of bounds? => no histogram
	if (!contour)
		return;

	CvRect R = cvBoundingRect(contour);
	double s1 = R.width, s2 = R.height;
	int x1 = max(min(R.x, lab->width-1), 0);
	int y1 = max(min(R.y, lab->height-1), 0);
    int x2 = R.x+R.width; x2 = max(min(x2, lab->width-1), 0);
	int y2 = R.y+R.height; y2 = max(min(y2, lab->height-1), 0);
	//echo(String::Concat(S"x1=", __box(x1), S" y1=", __box(y1), S" x2=", __box(x2), S" y2=", __box(y2)));
	// create weights matrix
	for (int x = x1; x <= x2; x++) {
		for (int y = y1; y <= y2; y++) {
			if (mask->imageData[y*mask->widthStep+x] != 0) {
				// generate one element of anisotropic gaussian kernel
				CvPoint3D32f p = doc->cameramodel.coordsImage2Real(cvPoint2D32f(x, y), bot, top);
				double squareDist = d_2(p, GetCenter(), cvPoint3D32f(axes.width, axes.width, axes.height));
				double k = exp(-squareDist);
				double weight = round(weights_scale*k);
				Pixel pixel(round(weight),0,0);
				Image::setPixel(frame, &pixel, x, y);
			}
		}
	}
}
// --------------------------------------------------------------------------
bool Body::IsOverlapping(Body* b2) {
	CvRect r1 = GetBoundingRect();
	CvRect r2 = b2->GetBoundingRect();
	// find max rect
	CvRect r = cvMaxRect(&r1, &r2); 
	//r.x = min(r1.x, r2.x); r.y = min(r1.y, r2.y);
	//r.width = max(r1.x+r1.width, r2.x+r2.width) - r.x; r.height = max(r1.y+r1.height, r2.y+r2.height) - r.y;

	if (r.width <= r1.width + r2.width && r.height <= r1.height + r2.height)
		return true;
	else
		return false;
}
// --------------------------------------------------------------------------
CvRect Body::GetBoundingRect() {
	cvZero(temp);
	doc->cameramodel.DrawVerticalSpheroid(temp, GetCenter(), GetAxes(), cvScalar(1), true);
	CvSeq* contour = cvCreateSeq(CV_SEQ_ELTYPE_GENERIC, sizeof(CvContour), sizeof(CvPoint2D32f), OpenCVmemory);
	cvFindContours(temp, OpenCVmemory, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	if (contour != 0)
		return cvBoundingRect(contour, 0);
	else 
		return cvRect(0,0,0,0);
}
// --------------------------------------------------------------------------
void Body::GetOccludedFloorArea(CvSeq* points) {
	// find a projection of a body on the floor

}