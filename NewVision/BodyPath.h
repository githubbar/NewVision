
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
 * BodyPath - contains an array of body coordinates, width, visibility and confidence information, one record per frame. 
 * Additionally stores the header with body id, tags and start frame
 * Invisible records can be disregarded thus letting BodyPath store multiple segments of a broken path
*/

#if !defined( BODYPATH_H )
#define BODYPATH_H
#pragma once
#include <afxtempl.h>
#include "cv.h"

class CNewVisionDoc;
class FloorRegion;
class Tracklet;
class Vexel;
class INode;
//using namespace System;

class Step {
public:
	Step() {};
	Step(CvPoint2D32f p, double height, double width, double orientation, double confidence, bool visible) {
		this->p = p;
		this->height = height;
		this->width = width;
		this->orientation = orientation;
		this->confidence = confidence;
		this->visible = visible;
	}
	Step& operator=(const Step& that) {
		this->p = that.p;
		this->height = that.height;
		this->width = that.width;
		this->orientation = that.orientation;
		this->confidence = that.confidence;
		this->visible = that.visible;
		return *this;
	}

	virtual void Serialize(CArchive& ar) {
		if (ar.IsStoring())	ar << p.x << p.y << height << width << orientation << confidence << visible;
		else ar >> p.x >> p.y >> height >> width >> orientation >> confidence >> visible;
	}
	CvPoint2D32f p;
	double height;
	double width;
	double orientation;
	double confidence;
	bool   visible;
};
// --------------------------------------------------------------------------
class BodyPath : public CArray < Step > 
{
	DECLARE_SERIAL(BodyPath)
public:
	CMapStringToString	tags;				// stores manually added information tags
	int					id;					// unique body id
	int					startFrame;			// number of start frame
	bool				isManual;			// indicates if confidence was set manually
	double				manualConfidence;	// manually assigned confidence
	double				autoConfidence;		// automatically assigned confidence

	BodyPath(int id, CvPoint2D32f p, int startFrame, double height, double width, double orientation, double confidence, bool visible);
	BodyPath(void);
	BodyPath(const BodyPath& that);
	BodyPath& operator=(const BodyPath& that);
	virtual	~BodyPath(void);

	void 			Add(CvPoint2D32f p, double height, double width, double orientation, double confidence, bool visible);
	void 			Draw(IplImage* frame, CvScalar color, CNewVisionDoc* doc, int sigma=0);
	void 			DrawFloor(IplImage* frame, CvScalar color, CNewVisionDoc* doc, int sigma=0);
	void 			DrawTrackFloor(IplImage* frame, CvScalar color, CNewVisionDoc* doc, int sigma=0);
	void			DrawTimeTagsFloor(IplImage* frame, CvScalar color, CNewVisionDoc* doc, CString tagName);
	void 			GenerateTrackHeatmap(CNewVisionDoc* doc, IplImage* img=NULL, int f1=-1, int f2=-1, int freq=1, int sigma=0);
	void 			GenerateDwellHeatmap(CNewVisionDoc* doc);

	double 			GetHeight(bool useMedian=true);
	double 			GetWidth(bool useMedian=true);
	double 			GetConfidence(); // get confidence as a mean of Step::confidence()
	double			Compare(BodyPath* that);
	void			Append(BodyPath* that);
	void 			Merge(BodyPath* that, bool fill=true);
	void 			CutTail(int stopFrame);
	void 			CutHead(int startFrame);
	void 			ConvertToTracklets(CArray<Tracklet*> &tarray, int minLength=0);
	void 			ConvertToVexels(CArray<INode*> &varray, int step=1, int minSpeedPerFrame=0, int sigma=0);
	virtual void	Serialize(CArchive& ar);
	
	CvPoint2D32f	Smooth(int n, int sigma);
	double			GetMotionAngle(int nFrame, int sigma);
	CvPoint2D32f 	GetMotionVector(int nFrame, int sigma);
	CvPoint2D32f 	GetMotionVector( int nFrame1, int nFrame2, int sigma );
	bool 			ComputeDwellingState(int nFrame, int sigma, int dwellFrames, double dwellAreaRadius);
	bool 			IsDwelling(int nFrame, int sigma);
	void 			GetDwellLocations(CArray<CvPoint2D32f> &dwellLocation, CArray<int> &dwellFrame, CArray<int> &dwellDuration);
	int				GetLastVisibleFrame();
	void			SetTagValue(CString key, CString value);
	CString			GetTagValue(CString key);
	void			GetFirstAndLastVisible(int &first, int &last);
	System::String^ ListVisibleSegments();
	void			ListVisibleSegments(CArray<IntPair, IntPair&> &list);
	void			ListZoneEvents(FloorRegion *z, CArray<IntPair, IntPair&> &list, int outMax=0);
	int				CountVisibleFrames(int f1=-1, int f2=-1);
	double			GetDistanceCovered(int f1=-1, int f2=-1);
	double			GetSinuosity(int f1=-1, int f2=-1, int smooth=10);
	double			GetAreaCovered(int f1=-1, int f2=-1);
	int				BridgeInvisibleGaps(int maxLength);
	void			ShiftTime(int frames);
};
// --------------------------------------------------------------------------
#endif // !defined( BODYPATH_H )