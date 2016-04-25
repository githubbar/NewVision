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
 *	Description  
 *  -----------
 *  BODY - represents a vercally oriented spheroid in 3D coordinates.
 *  The bottom pole (bot) of the spheroid has Z=0 , i.e. is on the floor.
 *  Depending on the camera model, spheroid can project onto the images differently.
 *  This is a non-persistent, non-serializable single frame modelling class. 
 *  For persistancy and frame to frame history, see class BodyPath and 	
 *  SaveToBodyHistory(int nFrame) and 	LoadFromBodyHistory(int nFrame) from class BodyActivityModel.
*/
#if !defined( BODY_H )
#define BODY_H
#pragma once

#include "cv.h"    
#include "Object3D.h"  
#include "LABHistogram2D.h"

#define INITIAL_WIDTH_RATIO 0.4
#define HEAD_HEIGHT 1.0/5
#define HEAD_WIDTH 3.0/5
#define ARROW_LENGTH 200   // arrows of ... cm
#define SECTOR_ANGLE PI/10
#define MAX_VELOCITY 20
#define FLOW_UPDATE_SPEED 0.05	// distance based on optical flow
#define MIN_X_FLOW 0.1	// minimum horizonal movement as the percentage of body width in pixels

class Blob;
class BodyPath;
struct BlobRay;

	class Body : public Object3D {
	DECLARE_DYNAMIC(Body)
	public:
		// Attributes
		CvPoint3D32f   top, bot, bot_prev;    // head and floor position 
		CvPoint3D32f   predicted;   // predicted foot position
		double         orientation; // orientation angle [0 = looking along X-axis; increasing counter-clockwise to PI, decreasing clockwise to -PI]
		CvKalman*      posKalman;   // Kalman filter tracks the position of the body on the 2D floor (using 1st order motion model)
		CvKalman*      orientKalman;// Kalman filter tracks the orientation of the head (using 2nd order motion model)
		LABHistogram2D color;
		double         widthRatio;
		int            blobid;
		int            startFrame;
		int            stopFrame;
		bool           visible;
		int            invisibleFor;  
		int            dormantFor;  
		int            outsiderFor;  
		bool           dwell;
		

		CvPoint p1, p2;		 // bounding box corners
		CvPoint2D32f   hc_prev;
		CvRect		   area_prev;

		// Display variables
		double pDoor;
		double pWidth; 
		double pHeight;
		double pBodyOverlap;
		double pTempColor;
		double pTempWidth;
		double pTempHeight;
		double pTempPosition;

		double         confidence;
		double         dHist;		// histogram correlation (0 to 1)
		CvPoint2D32f   m_dFlow;		// distance based on optical flow
		CvPoint2D32f   m_dAbsAvgFlow;	// average absolute distance based on optical flow

		// Methods
						Body(void);
						Body(const Body& b);
						Body(CvPoint2D32f point, double height, double width, CNewVisionDoc* doc, int id, int frameNumber);
						Body(BodyPath* path, CNewVisionDoc* doc, int frameNumber);
						Body(BlobRay* r, CNewVisionDoc* doc, int id, int frameNumber);
		Body&			operator=(const Body& that);
						~Body(void);
		void			HistogramCompute(IplImage* frame, CvHistogram* hist, int accumulate=0);
		void			HistogramUpdate(IplImage* frame);
		void			MoveTo(CvPoint3D32f f3);
		void			MoveHeadTo(CvPoint3D32f h3);
		CvPoint3D32f	MeanShift(IplImage* frame, IplImage* FGmask);
		CvPoint2D32f	MeanShiftImage(IplImage* frame, IplImage* FGmask);
		CvPoint3D32f	MeanShiftGetCenter(IplImage* frame, IplImage* FGmask, CvPoint3D32f foot);
		CvPoint2D32f	MeanShiftGetImageCenter(IplImage* frame, IplImage* FGmask, CvPoint2D32f imcenter);
		void			KalmanInit();
		void			KalmanCopy(CvKalman* src, CvKalman* dst);
		void            KalmanPredict();
		void			KalmanObserve();
		CvPoint3D32f	GetHead();
		CvPoint3D32f	GetFoot();
		CvPoint3D32f	GetCenter();
		CvPoint2D32f	GetImageFaceCenter(IplImage* frame, double& nMatches);
		CvPoint3D32f	GetFaceCenter(IplImage* frame, double& nMatches);
		CvPoint2D32f	GetImageHeadCenter();
		CvPoint3D32f	GetHeadCenter();
		CvPoint2D32f	GetImageHeadOpticalFlow(IplImage* frame);
		CvPoint2D32f	GetImageOpticalFlow(IplImage* frame);
		double			GetFaceOrientation(IplImage* frame);

		void			GetTagRect(CvRect& r);
		CvPoint2D32f	GetImageHead();
		CvPoint2D32f	GetImageFoot();
		CvPoint2D32f	GetImageCenter();
		double			GetHeight();
		void			SetHeight(double h);
		double			GetWidth();
		void			SetWidth(double w);
		CvSize2D32f		GetAxes();
		CvSize2D32f		GetImageAxes();
		double			GetVisiblePixelSize(); // returns visible body projection size in pixels
		double			GetDistanceFromCamera(); // distance from camera center in centimeters
		CvRect			GetBoundingRect();
		bool			OutOfBounds(CvPoint2D32f p, IplImage* frame); 
virtual void            GetOccludedFloorArea(CvSeq* points);
virtual void            DrawFloor(IplImage* frame, CvScalar color);
virtual void            DrawFloor(CDC* pDC, int ox, int oy, CPen* pen) {};
virtual void            DrawFrame(IplImage* frame, CvScalar color);
virtual void            DrawFrameLabel(IplImage* frame, CvScalar labelFG=CV_RGB(255,0,0), CvScalar labelBG=CV_RGB(255,255,255), double labelSize = 1.0f);
virtual void            DrawFrame(CDC* pDC , int ox, int oy, CPen* pen);
		void			DrawCurrentVsPredicted(IplImage* frame, CvScalar color);
		void			DrawHead(IplImage* frame, CvScalar color);
		void			DrawFoot(IplImage* frame, CvScalar color);
		void			DrawHeadArea(IplImage* frame, CvScalar color);
		void			DrawHeight(IplImage* frame, CvScalar color);
		void			DrawInfo(IplImage* frame, CvScalar color);
		void			DrawHeadCenter(IplImage* frame, CvScalar color);
		void			DrawFaceCenter(IplImage* frame, CvScalar color);
		void			DrawSectorHighlight(IplImage* srcFrame, IplImage* tgtFrame);
		void			DrawOrientationArrow(IplImage* frame, CvScalar color);
		void			DrawSalesRepMark(IplImage * frame, CvScalar color);
		void			DrawSalesRepTempMark(IplImage * frame, CvScalar color);
		void			DrawDwellMark(IplImage * frame, CvScalar color);
		void			DrawWeightMask(IplImage* frame);
virtual void			RasterizeHead(IplImage* frame, CvScalar color);
virtual void			RasterizeFrame(IplImage* frame, CvScalar color=CV_RGB(255,255,255));
virtual void			RasterizeFloor(IplImage* frame, CvScalar color=CV_RGB(255,255,255));
virtual void			DrawHistogram(IplImage* frame, int scale=0, CvPoint p=cvPoint(0,0));
virtual bool            IsFramePointIn(CvPoint point, CvPoint3D32f& pcurrent) {return false;};
virtual bool            IsFloorPointIn(CvPoint point, CvPoint3D32f& pcurrent) {return false;};
virtual bool 			IsOverlapping(Body* b2);
virtual bool            Move(CvPoint3D32f p) {return true;};
virtual bool            Move1(CvPoint2D32f p) {return true;};
virtual bool            Move2(CvPoint2D32f p) {return true;};
virtual bool            Move3(CvPoint2D32f p) {return true;};
virtual bool            Move4(CvPoint2D32f p) {return true;};

	private:
		static double	weights_scale; // for the mean-shift kernel
		static double	min_mean_shift_err;
		static double	max_mean_shift_iter;
};

#endif // !defined( BODY_H )