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
*/
#include "stdafx.h"
#include "Globals.h"
#include "BodyPath.h"
#include "BodyPathCluster.h"
#include "NewVisionDoc.h"
#include "FloorRegion.h"
#include "Tracklet.h"
#include "Vexel.h"

#using <mscorlib.dll>
#using <System.dll>

using namespace System;
using namespace System::IO;
using namespace System::Diagnostics;

IMPLEMENT_SERIAL(BodyPath, CArray < Step >, 1)
// --------------------------------------------------------------------------
BodyPath::BodyPath(int id, CvPoint2D32f p, int startFrame, double height, double width, double orientation, double confidence, bool visible) {
	this->id = id;
	this->startFrame = startFrame;
	this->isManual = false;
	this->autoConfidence = 0;
	this->manualConfidence = 0;
	//employee = 9; 
	//int female;
	//int buyer;
	//int agegroup;
	CArray<Step>::Add(Step(p, height, width, orientation, confidence, visible));
}
// --------------------------------------------------------------------------
BodyPath::BodyPath(void)
{
	this->isManual = false;
	this->autoConfidence = 0;
	this->manualConfidence = 0;
}
// --------------------------------------------------------------------------	
BodyPath::BodyPath(const BodyPath& that) {
	Copy(that);
	// Copy tags
	POSITION pos;
	CString key, value;
	for( pos = that.tags.GetStartPosition(); pos != NULL; )
	{
		that.tags.GetNextAssoc(pos, key, value);
		this->tags.SetAt(key, value);
	}
	// End copy tags
	this->id = that.id;
	this->startFrame = that.startFrame;
	this->isManual = that.isManual;
	this->autoConfidence = that.autoConfidence;
	this->manualConfidence = that.manualConfidence;
}
// --------------------------------------------------------------------------	
BodyPath& BodyPath::operator=(const BodyPath& that) {
	Copy(that);
	// Copy tags
	this->tags.RemoveAll();
	POSITION pos;
	CString key, value;
	for( pos = that.tags.GetStartPosition(); pos != NULL; )
	{
		that.tags.GetNextAssoc(pos, key, value);
		this->tags.SetAt(key, value);
	}
	// End copy tags
	this->id = that.id;
	this->startFrame = that.startFrame;
	this->isManual = that.isManual;
	this->autoConfidence = that.autoConfidence;
	this->manualConfidence = that.manualConfidence;
	return *this;
}
// --------------------------------------------------------------------------
BodyPath::~BodyPath(void)
{
}
// --------------------------------------------------------------------------
void BodyPath::Add(CvPoint2D32f p, double height, double width, double orientation, double confidence, bool visible) {
	CArray<Step>::Add(Step(p, height, width, orientation, confidence, visible));
}
// --------------------------------------------------------------------------
void BodyPath::SetTagValue( CString key, CString value )
{
	this->tags.SetAt(key, value);
}
// --------------------------------------------------------------------------
CString BodyPath::GetTagValue( CString key )
{
	CString value;
	if (this->tags.Lookup(key, value))
		return value;
	else
		return "";
}
// --------------------------------------------------------------------------
double BodyPath::GetHeight(bool useMedian) {
	double h = 0;
	if (useMedian) {
		// create temp array
		double *aTmp = new double[this->GetCount()];
		int n = 0;
		for (int i=0; i < this->GetCount(); i++) {
			aTmp[n++] = this->GetAt(i).height;
		}
		// if not empty, compute median
		if (n>0)
			h = median<double>(aTmp, n);
		delete[] aTmp;
	}
	else {
		for (int i=0; i < this->GetCount(); i++) {
			h += this->GetAt(i).height;
		}
		h /= this->GetCount();
	}

	return h;
}
// --------------------------------------------------------------------------
double BodyPath::GetWidth(bool useMedian) {
	double w = 0;
	if (useMedian) {
	}
	else {
		for (int i=0; i < this->GetCount(); i++) {
			w += this->GetAt(i).width;
		}
		w /= this->GetCount();
	}
	return w;

}
// --------------------------------------------------------------------------
double BodyPath::GetConfidence() {
	double c = 0;
	int count = 0;
	for (int i=0; i < this->GetCount(); i++) {
		double cc = this->GetAt(i).confidence;
		if (Double::IsNaN(cc) ||  cc > 1 || cc < 0)
			this->GetAt(i).confidence = 0;
		if (this->GetAt(i).visible) {
			c += this->GetAt(i).confidence;
			count++;
		}
	}
	if (!count)
		return 0;
	else 
		return c/count;
}
// --------------------------------------------------------------------------
double BodyPath::Compare(BodyPath* that) {
	// find first and last common frame
	int b = max(this->startFrame, that->startFrame);
	int e = min(this->startFrame+this->GetCount(), that->GetCount()+that->startFrame);
	
	// if no common subsequence found, return large number
	if (b >= e)
		return Double::MaxValue;

	// compute distance ( shift = =0)
	double dist = 0;
	for (int i = b; i < e; i++) {
		dist += d(this->GetAt(i-this->startFrame).p, that->GetAt(i-that->startFrame).p);
	}
	return dist/(e-b+1);
}
// --------------------------------------------------------------------------
void BodyPath::Append(BodyPath* that) {
	this->Merge(that, false);
}
// --------------------------------------------------------------------------
void BodyPath::Merge(BodyPath* that, bool fill/*=true*/) {
	int masterLast, masterFirst, slaveLast, slaveFirst;
	this->GetFirstAndLastVisible(masterFirst, masterLast);
	that->GetFirstAndLastVisible(slaveFirst, slaveLast);
	// Initialize joint path
	int b = min(masterFirst, slaveFirst);
	int e = max(masterLast, slaveLast);
	CArray<Step> joint; // merge array

	// Combine visible steps 
	for (int i = b; i <= e; i++) {
		if ( i < masterFirst || i > masterLast || !this->GetAt(i-this->startFrame).visible ) 
				if (i >= slaveFirst && i <= slaveLast && that->GetAt(i-that->startFrame).visible )  	// not present in master track, but present in slave track
					joint.InsertAt(i - b, that->GetAt(i-that->startFrame));
				else {
					// no info from any tracks => insert dummy stub
					Step s; s.visible = false;
					joint.InsertAt(i - b, s);
				}
		else 
			joint.InsertAt(i - b, this->GetAt(i-this->startFrame));

	}
	
	// Interpolate the gap between this and that
	if (fill) {
		int bGap, eGap;
		Step *sFirst, *sLast;
		if (masterLast < slaveFirst) {
			 bGap = masterLast; eGap = slaveFirst;
			 sFirst = &this->GetAt(masterLast-this->startFrame);
			 sLast = &that->GetAt(slaveFirst-that->startFrame);
		}
		else {
			bGap = slaveLast; eGap = masterFirst;
			sFirst = &that->GetAt(slaveLast-that->startFrame);
			sLast = &this->GetAt(masterFirst-this->startFrame);

		}
		for (int i = bGap+1; i < eGap; i++) {
			double w1 = (eGap - i);
			double w2 = (i - bGap);
			Step inter;
			inter.p.x = (sFirst->p.x * w1 + sLast->p.x * w2) / (w1+w2);
			inter.p.y = (sFirst->p.y * w1 + sLast->p.y * w2) / (w1+w2);
			inter.orientation = sLast->orientation;
			inter.width = sLast->width; inter.height= sLast->height;
			inter.confidence = 1;
			inter.visible = fill;
			joint.SetAt(i-b, inter);
		}
	}

	this->RemoveAll();
	CArray::Copy(joint);
	this->startFrame = b;
}
// --------------------------------------------------------------------------
void BodyPath::CutTail(int stopFrame) {
	int stopIndex = min(stopFrame-this->startFrame+1, this->GetUpperBound());
	this->RemoveAt(stopIndex, this->GetCount() - stopIndex);
}
// --------------------------------------------------------------------------
void BodyPath::CutHead(int startFrame) {
	int startCount = max(0, startFrame-this->startFrame);
	this->RemoveAt(0, startCount);
	this->startFrame = startFrame;
}
// --------------------------------------------------------------------------
void BodyPath::GetFirstAndLastVisible(int &first, int &last) {
	first = startFrame;
	last = startFrame;
	bool foundFirst = false;
	for (int i=0; i < this->GetCount(); i++) {
		if (this->GetAt(i).visible) {
			last = i+startFrame;
			if (!foundFirst) {
				first = i+startFrame;
				foundFirst = true;
			}
		}
	}
}
// --------------------------------------------------------------------------
void BodyPath::Serialize( CArchive& ar )
{
	//TODO:check id=1 confidence is zero?
	tags.Serialize(ar);
	if (ar.IsStoring()) {
		autoConfidence = GetConfidence(); // store average path confidence in derived field
		ar << id << startFrame << isManual << manualConfidence << autoConfidence;
		//temp:
		//ar << id << (startFrame - 540000) << isManual << manualConfidence << autoConfidence;
	}
	else {
		//ar >> id >> startFrame;
		ar >> id >> startFrame >> isManual >> manualConfidence >> autoConfidence;
	}
	CArray< Step >::Serialize(ar);
	autoConfidence = GetConfidence(); // store average path confidence in derived field
}
// --------------------------------------------------------------------------
void BodyPath::Draw(IplImage* frame, CvScalar color, CNewVisionDoc* doc, int sigma) {
}
// --------------------------------------------------------------------------
void BodyPath::DrawFloor(IplImage* frame, CvScalar color, CNewVisionDoc* doc, int sigma) {
	//// Draw a vanishing trail
	////m_pathSmoothSigma
	//const int step = 10; //draw every ... frames;
	//int radius = 20; // max radius
	//double radiusFadingRate = 0.5; // the speed with which the size shrinks
	//double colorFadingRate = 0.5; // the speed with which the color fades 
	//const int TOO_WHITE = 230;
	//color = colorFromID(id); 
	//color.val[0] = min(color.val[0], TOO_WHITE);
	//color.val[1] = min(color.val[1], TOO_WHITE);
	//color.val[2] = min(color.val[2], TOO_WHITE);
	//for (int n=doc->trackermodel.m_frameNumber - sigma; n <= doc->trackermodel.m_frameNumber-step; n++) {
	//	CvScalar clr = colorLighter(color, colorFadingRate*(doc->trackermodel.m_frameNumber-n));
	//	if (n < startFrame || doc->trackermodel.m_frameNumber > startFrame + this->GetUpperBound()) 
	//		continue;
	//	Step s = this->GetAt(n-startFrame);
	//	if (s.visible) {
	//		int rd = max((int)(radius-radiusFadingRate*(doc->trackermodel.m_frameNumber-n)), 0);
	//		drawTextInCircle("", doc->floormodel.coordsReal2Floor(s.p), frame, CV_RGB(0,0,0), clr, 0.8f, rd);
	//		
	//	}
	//}
	//// Draw ID in a circle
	//if (doc->trackermodel.m_frameNumber < startFrame || doc->trackermodel.m_frameNumber > startFrame + this->GetUpperBound()) 
	//	return;

	//Step s = this->GetAt(doc->trackermodel.m_frameNumber-startFrame);
	//if (s.visible) {
	//	CvPoint xy = doc->floormodel.coordsReal2Floor(s.p);
	//	drawTextInCircle(id, xy, frame, CV_RGB(0,0,0), color, 0.8f, radius);
	//}
}
// --------------------------------------------------------------------------
bool BodyPath::ComputeDwellingState(int nFrame, int sigma, int dwellFrames, double dwellAreaRadius) {
	int nPts = this->GetCount();

	// not enough information to make a decision smooth
	if (nPts < dwellFrames || nFrame-startFrame < dwellFrames)
		return false;

	int lo = max(nFrame-dwellFrames-startFrame, 0), hi = min(nFrame-startFrame, this->GetCount()-1);

	CvSeq* seqPoints = cvCreateSeq(CV_SEQ_KIND_CURVE|CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), OpenCVmemory);

	// start from the most recent point 
	for (int i = lo; i <= hi; i++) {
		CvPoint point = cvPointFrom32f(Smooth(startFrame+i, sigma));
		cvSeqPush(seqPoints, &point);
	}
	if (seqPoints->total == 0) 
		return false;
	CvPoint2D32f center;
	float radius;

	cvMinEnclosingCircle(seqPoints, &center, &radius);
	return (radius < dwellAreaRadius) ? true : false;
	return false;
}
// --------------------------------------------------------------------------
void BodyPath::GetDwellLocations(CArray<CvPoint2D32f> &dwellLocation, CArray<int> &dwellFrame, CArray<int> &dwellDuration) {
	//bool dwelling = false;
	//int dwellCount = 0;
	//int fLastVisible = GetLastVisibleFrame();
	//for (int i=0; i < GetCount(); i++) {
	//	Step s  = this->GetAt(i);
	//	bool dwell = ComputeDwellingState(i+startFrame, )
	//	if ( && s.visible) {
	//		if (!dwelling) {
	//			dwellFrame.Add(startFrame+i);
	//			dwellLocation.Add(s.p);
	//			dwellDuration.Add(0);
	//			dwelling = true;
	//		}
	//		else {
	//			CvPoint2D32f* loci = &(dwellLocation[dwellLocation.GetCount()-1]);
	//			loci->x += s.p.x;
	//			loci->y += s.p.y;
	//		}
	//		dwellDuration[dwellDuration.GetCount()-1]++;
	//	}
	//	// finished dwelling 
	//	if( (!s.dwell || i == fLastVisible-startFrame) && dwelling) {
	//		CvPoint2D32f* loci = &(dwellLocation[dwellLocation.GetCount()-1]);
	//		loci->x /= dwellDuration[dwellDuration.GetCount()-1];
	//		loci->y /= dwellDuration[dwellDuration.GetCount()-1];
	//		dwelling = false;
	//	}
	//}
}
// --------------------------------------------------------------------------
void BodyPath::ShiftTime(int frames) {
	this->startFrame += frames;
}
// --------------------------------------------------------------------------
void BodyPath::DrawTimeTagsFloor(IplImage* frame, CvScalar color, CNewVisionDoc* doc, CString tagName) {
	int RADIUS = 10;
	CommaDelimitedStringCollectionConverter ^cv = gcnew CommaDelimitedStringCollectionConverter();
	CommaDelimitedStringCollection ^startstop = (CommaDelimitedStringCollection ^)cv->ConvertFromString(gcnew String(GetTagValue(tagName)));
	//CommaDelimitedStringCollection ^startstop = GetUserSettingsCommaList(gcnew String(tagName));
	for (int i=0;i<startstop->Count-1;i+=2) {
		int eventStartFrame = Int32::Parse(startstop[i]);
		int eventStopFrame = Int32::Parse(startstop[i+1]);
		String ^start = doc->trackermodel.GetDateTime(eventStartFrame).ToString("dd-MMM-yy HH:mm:ss", DateTimeFormatInfo::InvariantInfo);
		String ^stop = doc->trackermodel.GetDateTime(eventStopFrame).ToString("dd-MMM-yy HH:mm:ss", DateTimeFormatInfo::InvariantInfo);
		// Catch invalid time tags
		if (eventStartFrame < startFrame || eventStopFrame > startFrame + GetUpperBound() ||
			eventStartFrame > eventStopFrame ) 
		{
				
		}
		else {
			// Write ZoneID

			// Draw a circle at the start of event
			Step stepStart = GetAt(eventStartFrame-startFrame);		
			CvPoint f = doc->floormodel.coordsReal2Floor(stepStart.p);
			//cvCircle(frame, f, RADIUS, color, CV_FILLED, CV_AA); 
			drawTextInCircle(double(this->id), f, frame, CV_RGB(255,255,255), color, 0.8f, 5);
		}
	}	
}
// --------------------------------------------------------------------------
void BodyPath::DrawTrackFloor(IplImage* frame, CvScalar color, CNewVisionDoc* doc, int sigma) {
	CArray<IntPair, IntPair&> list;
	ListVisibleSegments(list);
	// for each visible segment 
	for (int n=0; n < list.GetCount(); n++) {
		// === Ignore short tracks
		int MIN_TRACK_LENGTH = 2;
		if (list[n].id2 - list[n].id1 < MIN_TRACK_LENGTH)
			continue;

		// === Draw tracks
		for (int i = list[n].id1+1; i < list[n].id2 /*&& i <= doc->trackermodel.m_frameNumber*/; i++) {
			CvPoint2D32f point1 = this->GetAt(i-startFrame-1).p;//Smooth(i-1+startFrame, sigma);
			CvPoint2D32f point2 = this->GetAt(i-startFrame).p;//Smooth(i+startFrame, sigma);
			cvLine(frame, doc->floormodel.coordsReal2Floor(point1), doc->floormodel.coordsReal2Floor(point2), color, 1, CV_AA);
		}

		// === Draw dwelling balls
		bool dwelling = false;
		CvPoint2D32f fDwell;
		double dDwell;
		for (int i = list[n].id1; i < list[n].id2 /*&& i <= doc->trackermodel.m_frameNumber*/; i++) {
			CvPoint2D32f point = this->GetAt(i-startFrame).p;//Smooth(i+startFrame, sigma);
			CvPoint f = doc->floormodel.coordsReal2Floor(point);
			bool dwell = ComputeDwellingState(i, 
				doc->bodyactivitymodel.m_pathSmoothSigma, 
				doc->bodyactivitymodel.m_dwellTime, 
				doc->bodyactivitymodel.m_dwellAreaRadius);

			// just started dwelling now
			if (dwell && !dwelling) {
				fDwell = cvPointTo32f(f);
				dDwell = 1;
				dwelling = true;
			}
			// continuing to dwell
			else if (dwell && dwelling) {
				fDwell.x += (f.x);
				fDwell.y += (f.y);
				dDwell++;
			}
			// just finished dwelling 
			if(!dwell && dwelling) {
				fDwell.x /= dDwell;
				fDwell.y /= dDwell;
				int r = min(max(1, round(DWELL_POINT_SCALE * dDwell / doc->trackermodel.m_frameRate)), DWELL_POINT_MAX_R);
				cvCircle(frame, cvPointFrom32f(fDwell), r, DWELL_POINT_COLOR, CV_FILLED, CV_AA); 
				cvCircle(frame, cvPointFrom32f(fDwell), r, color, 1, CV_AA); 
				dwelling = false;
			}
		}

		////// ======= Use tracks originating only inside the interest region
		////CvPoint2D32f imgpoint = doc->cameramodel.coordsReal2Image(cvPoint3D32f(point.x, point.y, 0));
		////if (i == 0 && !doc->doormodel.IsInsideInterestRegion(imgpoint))
		////	return;

	}
}
// --------------------------------------------------------------------------
void BodyPath::GenerateTrackHeatmap(CNewVisionDoc* doc, IplImage* img, int f1, int f2, int freq, int sigma) {
	if (f1 == -1) {
		f1 = startFrame;
		f2 = startFrame+GetCount();
	}
	if (img == NULL)
		img = doc->m_HeatmapData.map;

	f1 = max(f1-startFrame, 0);
	f2 = min(f2-startFrame, GetCount());

	for (int i = f1; i < f2; i += freq) {
			CvPoint2D32f point = Smooth(startFrame+i, sigma);
			Step s = this->GetAt(i);//Smooth(i+startFrame, sigma);
			CvPoint f = doc->floormodel.coordsReal2Floor(s.p);
			CvSize axes = doc->floormodel.sizeReal2Floor(cvSize2D32f(s.width/2, s.width/2)); 
			cvZero(motionmaptemp);
			if (axes.height < 0 || axes.width < 0) {
				continue;
			}
			cvEllipse(motionmaptemp, f, axes, 0, 0, 360, cvScalar(1), CV_FILLED);
			cvAcc(motionmaptemp, img);
	}
}
// --------------------------------------------------------------------------
/**
 * Create a heatmap with average dwell times (in frames)
 * \param motionmap 
 output heatmap
 * \param doc 
 */
void BodyPath::GenerateDwellHeatmap(CNewVisionDoc* doc) {
	CArray<IntPair, IntPair&> list;
	ListVisibleSegments(list);
	// for each visible segment 
	for (int n=0; n < list.GetCount(); n++) {
		bool dwelling = false;
		CArray<CvPoint2D32f> locations;
		for (int i = list[n].id1; i < list[n].id2; i++) {
			bool dwell = ComputeDwellingState(i, 
				doc->bodyactivitymodel.m_pathSmoothSigma, 
				doc->bodyactivitymodel.m_dwellTime, 
				doc->bodyactivitymodel.m_dwellAreaRadius);
			CvPoint2D32f point = this->GetAt(i-startFrame).p;
			// just started dwelling now
			if (dwell && !dwelling) {
				locations.RemoveAll();
				dwelling = true;
			}
			// dwelling
			if (dwell)
				locations.Add(point);
			// just finished dwelling or end of segment
			if((!dwell || i == list[n].id2-1) && dwelling) {
				double r = this->GetWidth()/2;
				CvSize axes = doc->floormodel.sizeReal2Floor(cvSize2D32f(r, r)); 
				if (axes.width > 0 && axes.height > 0) {
					for (int l=0;l<locations.GetCount();l++) {
						CvPoint fDwell = doc->floormodel.coordsReal2Floor(locations[l]);
						cvZero(motionmaptemp);
						// Total dwell duration
						cvEllipse(motionmaptemp, fDwell, axes, 0, 0, 360, cvScalar(locations.GetCount()), CV_FILLED);
						cvAcc(motionmaptemp, doc->m_HeatmapData.map);
						cvZero(motionmaptemp);
						// Dwell event counter
						cvEllipse(motionmaptemp, fDwell, axes, 0, 0, 360, cvScalar(1), CV_FILLED);
						cvAcc(motionmaptemp, doc->m_HeatmapData.count);
					}
				}
				dwelling = false;
			}
		}
	}
}
// --------------------------------------------------------------------------
CvPoint2D32f BodyPath::Smooth(int nFrame, int sigma) {
	if (nFrame-startFrame < 0)
		nFrame = startFrame;
	if (nFrame-startFrame > this->GetUpperBound())
		nFrame = startFrame + this->GetUpperBound();
	int lo = max(nFrame-sigma-startFrame, 0), hi = min(nFrame+sigma-startFrame, this->GetUpperBound());
	CvPoint2D32f point = cvPoint2D32f(0,0);
	if (sigma == 0)
		return this->GetAt(nFrame-startFrame).p;
	float norm = 0;
    for (int i=lo;i<=hi;i++) {
		if (!this->GetAt(i).visible)
			continue;
		CvPoint2D32f p = this->GetAt(i).p;
		float w = (float)probNormal(i, nFrame-startFrame, sigma);
		norm += w;
		point.x += p.x*w; point.y += p.y*w;
	}
	if (norm == 0)
		return this->GetAt(nFrame-startFrame).p;
	point.x /= norm; point.y /= norm;
	return point;
}
// --------------------------------------------------------------------------
double BodyPath::GetMotionAngle( int nFrame, int sigma )
{
	CvPoint2D32f point1 = Smooth(nFrame, sigma);
	CvPoint2D32f point2 = Smooth(nFrame+1, sigma);
	if (point1.x == point2.x && point1.y == point2.y)
		return 0;
	//float temp = point1.y;
	//point1.y = point2.y;
	//point2.y = temp;
	double a = angle(point1, point2);
	return a;
}
// --------------------------------------------------------------------------
CvPoint2D32f BodyPath::GetMotionVector( int nFrame, int sigma )
{
	CvPoint2D32f point1 = Smooth(nFrame, sigma);
	CvPoint2D32f point2 = Smooth(nFrame+1, sigma);
	return cvPoint2D32f(point2.x-point1.x, point2.y-point1.y);
}
// --------------------------------------------------------------------------
CvPoint2D32f BodyPath::GetMotionVector( int nFrame1, int nFrame2, int sigma )
{
	CvPoint2D32f point1 = Smooth(nFrame1, sigma);
	CvPoint2D32f point2 = Smooth(nFrame2, sigma);
	return cvPoint2D32f(point2.x-point1.x, point2.y-point1.y);
}
// --------------------------------------------------------------------------
int BodyPath::GetLastVisibleFrame() {
	int last = startFrame;
	for (int i=0; i < this->GetCount(); i++) {
		if (this->GetAt(i).visible)
			last = startFrame+i;
	}
	return last;
}
// --------------------------------------------------------------------------
int BodyPath::CountVisibleFrames(int f1, int f2) {
	if (f1 == -1) {
		f1 = startFrame;
		f2 = startFrame+GetCount();
	}
	int count = 0;
	for (int i=f1-startFrame; i < f2-startFrame; i++) {
		if (this->GetAt(i).visible)
			count++;
	}
	return count;
}
// --------------------------------------------------------------------------
double BodyPath::GetDistanceCovered(int f1, int f2) {
	double distance = 0;
	if (f1 == -1)
		f1 = startFrame;
	if (f2 == -1)
		f2 = startFrame+GetCount();

	for (int i = f1+1; i < f2;i++) {
		Step s1 = GetAt(i-f1-1), s2 = GetAt(i-f1);
		if (s1.visible && s2.visible) {
			distance += d(s1.p, s2.p); // add only visible vertices, assume that invisible part were straight lines
		}
	}
	return distance;
}
// --------------------------------------------------------------------------
double BodyPath::GetAreaCovered(int f1, int f2) {
	// create OpenCv contour corresponding to the path outline
	CvSeq* contour = cvCreateSeq( CV_SEQ_KIND_CURVE | CV_32SC2, sizeof(CvContour), sizeof(CvPoint), OpenCVmemory );
	CvSeqWriter writer;
	cvStartAppendToSeq(contour, &writer);
	CvPoint pt;
	if (f1 == -1) {
		f1 = startFrame;
		f2 = startFrame+GetCount();
	}
	for (int i = f1; i < f2;i++) {
		Step s = GetAt(i-f1);
		if (s.visible) // add only visible vertices, assume that invisible part were straight lines
			CV_WRITE_SEQ_ELEM( cvPointFrom32f(s.p), writer );
	}
	cvEndWriteSeq( &writer );
	// calculate the absolute value of contour area
	return fabs(cvContourArea(contour));
}
// --------------------------------------------------------------------------
double BodyPath::GetSinuosity(int f1, int f2, int smooth) {
	double dAngle = 0;
	int countAngles = 0;
	if (f1 == -1) {
		f1 = startFrame;
		f2 = startFrame+GetCount();
	}
	for (int i = f1+1; i < f2;i++) {
		Step s1 = GetAt(i-f1-1), s2 = GetAt(i-f1);
		if (s1.visible && s2.visible) {
			// BUG: change to dot product!!!
			//dAngle += abs(GetMotionAngle(i-1, smooth) - GetMotionAngle(i, smooth)); 
			
			CvPoint2D32f v1 = GetMotionVector(i-1, smooth), v2 = GetMotionVector(i, smooth);
			if (d(v1) != 0 && d(v2) != 0) { 
				double dopo = dotProduct(v1, v2)/d(v1)/d(v2);
				dopo = max(dopo, -1); dopo = min(dopo, 1); // chop chop!
				dAngle += abs(acos(dopo)); 
			}

			countAngles++; 
			if (Double::IsNaN(dAngle) || Double::IsInfinity(dAngle) || Double::IsNegativeInfinity(dAngle)) {
				/*doc->Message();*/
				int a = 4; a++;
			}
		}
	}
	if (countAngles > 0) 
		return rad2deg(dAngle/countAngles);
	else 
		return 0;
}
// --------------------------------------------------------------------------
String^ BodyPath::ListVisibleSegments() {
	String^ msg = "";
	bool visibleNow = GetAt(0).visible;
	int i=startFrame;
	if (visibleNow) 
		msg += String::Format("{0}", i);
	for (; i < startFrame+GetCount();i++) {
		if (GetAt(i-startFrame).visible && !visibleNow) {
			msg += String::Format("{0}", i);
			visibleNow = true;
		}
		else if (!GetAt(i-startFrame).visible && visibleNow) {
			msg += String::Format(" - {0}\n", i-1);
			visibleNow = false;
		}
	}
	if (visibleNow) 
		msg += String::Format(" - {0}\n", i-1);
	return msg;
}
// --------------------------------------------------------------------------
void BodyPath::ListVisibleSegments(CArray<IntPair, IntPair&> &list) {
	bool visibleNow = false;
	int i1 = 0;
	for (int i = startFrame; i < startFrame+GetCount();i++) {
		// became visible
		if (GetAt(i-startFrame).visible && !visibleNow) {
			i1 = i;
			visibleNow = true;
		}
		// became invisible or the end of path
		if ((!GetAt(i-startFrame).visible || i == startFrame+GetUpperBound()) && visibleNow) {
			list.Add(IntPair(i1, i-1));
			visibleNow = false;
		}
	}
}
// --------------------------------------------------------------------------
/*!
 * \brief
 * ListZoneEvents - returns a list of zone events for the BodyPath
 * 
 * \param z
 * Description of parameter z.
 * 
 * \param list
 * a list of IntPair<startFrame, stopFrame> zone events for the BodyPath
 * 
 * \param outMax
 * Description of parameter outMax.
 * 
 * \throws <exception class>
 * Description of criteria for throwing this exception.
 * 
 * Write detailed description for ListZoneEvents here.
 * 
 * \remarks
 * Write remarks for ListZoneEvents here.
 * 
 * \see
 * Separate items with the '|' character.
 */
void BodyPath::ListZoneEvents(FloorRegion *z, CArray<IntPair, IntPair&> &list, int outMax) {
	// "outMax": allow some frames out of zone to prevent "jittery events"
	int outCount = 0;
	bool inZone = false;
	int start = 0, stop = 0;
	for (int i = startFrame; i < startFrame+GetCount();i++) {
		// wasn't in the zone got into the zone (only visible body can do that)
		if ( !inZone && GetAt(i-startFrame).visible && z->IsRealPointIn(GetAt(i-startFrame).p) ) { 
			start = i;
			inZone = true;
			outCount = 0;
		}
		// was in zone, now got out of the zone (or became invisible while inside the zone)
		if (inZone && (!GetAt(i-startFrame).visible || !z->IsRealPointIn(GetAt(i-startFrame).p )) ) {
			stop = i;
			if (outCount < outMax)
				outCount++; // allow some time to re-enter;
			else { // was out for too long
				// present in the zone for a significant period of time
				if ( stop-start > outMax) {
					inZone = false;
					list.Add(IntPair(start, stop-outCount));
				}
			}
		}
	}
}
// --------------------------------------------------------------------------
int BodyPath::BridgeInvisibleGaps(int maxLength) {
	bool visibleNow = true;
	int startGap = startFrame;
	int i=startFrame;
	int countGaps = 0;
	for (; i < startFrame+GetCount();i++) {
		if (GetAt(i-startFrame).visible && !visibleNow) {
			visibleNow = true;
			if (i - startGap < maxLength) { // in gap in shorter than maxLength
				countGaps++;
				for (int n=startGap; n < i;n++)
					GetAt(n-startFrame).visible = true;
			}
		}
		else if (!GetAt(i-startFrame).visible && visibleNow) {
			visibleNow = false;
			startGap = i;
		}
	}
	return countGaps;
}
// --------------------------------------------------------------------------
void BodyPath::ConvertToTracklets( CArray<Tracklet*>& tarray, int minLength )
{
	CArray<IntPair, IntPair&> list;
	ListVisibleSegments(list);
	// for each visible segment 
	for (int n=0; n < list.GetCount(); n++) {
		// === Ignore short tracks
		if (list[n].id2 - list[n].id1 < minLength)
			continue;

		Tracklet* t = new Tracklet();
		tarray.Add(t);
		// === Create new tracklet
		for (int i = list[n].id1; i < list[n].id2 ; i++)
			t->Add(this->GetAt(i-startFrame-1).p);
	}
}
// --------------------------------------------------------------------------
void BodyPath::ConvertToVexels( CArray<INode*> &varray, int step/*=1*/, int minSpeedPerFrame/*=0*/, int sigma/*=0*/ )
{
	CArray<IntPair, IntPair&> list;
	ListVisibleSegments(list);
	// for each visible segment 
	for (int n=0; n < list.GetCount(); n++) {
		// === Create new vexels
		double distance = 0, nFrames = 0;
		for (int i = list[n].id1; i < list[n].id2-1 ; i++) {
			distance += d(GetAt(i-startFrame).p, GetAt(i+1-startFrame).p); 
			nFrames++;
			// sample every 'step' frames
			if (i%step) {
				// if moving at the minimum speed (exclude first step) 
				if (distance/nFrames > minSpeedPerFrame) {
					Vexel *v = new Vexel();
					int midFrame = (int)(i-0.5*(nFrames-1));
					v->p = Smooth(midFrame, sigma);
					v->v = GetMotionVector(i-(nFrames-1), i, sigma);
					varray.Add(v);
				}
				distance = 0;
				nFrames = 0;
			}
		}
	}
}

