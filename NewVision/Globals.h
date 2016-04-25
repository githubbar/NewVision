#if !defined( MYOPENCVFUNCTIONS_H )
#define MYOPENCVFUNCTIONS_H 
#include "cv.h"      
#include "highgui.h" 
#include "ProgressDlg.h" 
#include "Image.h"
#include "FloorRegion.h"
#include <afxmt.h>
#include <afxtempl.h>

#using <mscorlib.dll>
#using <system.dll>
#using <system.configuration.dll>

using namespace OpenCVExtensions;
using namespace System;
using namespace System::Diagnostics;
using namespace System::IO;
using namespace System::Globalization;
using namespace System::Configuration;



// ---- control which parts of the project to build (to decrease link time)
#define PI  3.14159265358979323846
#define FEET_IN_METER 3.2808399
#define SQFEET_IN_SQMETER 10.7639104
class CCritSec;
// ------------------------------------------------------------------------
#if DEBUG_LEVEL > 0
	#define echo(s) Debug::WriteLine(s)
	#define ech(s) Debug::Write(s)
#else
	#define echo(s) true
	#define ech(s) true
#endif 
// ------------------------------------------------------------------------
#ifndef COLOR
static CvScalar COLOR[] = {
		CV_RGB(0,0,255),
		CV_RGB(0,255,0),
		CV_RGB(0,255,255),
        CV_RGB(255,0,0),
        CV_RGB(255,0,255),
		CV_RGB(255,255,0),
		CV_RGB(0,0,128),
		CV_RGB(0,128,0),
		CV_RGB(0,128,128),
        CV_RGB(128,0,0),
        CV_RGB(128,0,128),
		CV_RGB(128,128,0),
		CV_RGB(0,0,196),
		CV_RGB(0,196,0),
		CV_RGB(196,0,0),
		CV_RGB(64,0,196),
		CV_RGB(0,64,128),
		CV_RGB(0,196,64)
        };
#endif


extern OpenCVExtensions::Pixel HEAT_COLOR[];
extern int NTEMPERATURES;
// ------------------------------------------------------------------------

 // temporary OpenCV buffers (clean up OpenCVmemory every frame)
extern CvMemStorage* OpenCVmemory;
//extern IplImage* frame;
extern IplImage *imgFrame, *imgOverlayInfo, *imgPreviousFrame, *imgThermal, *imgVideo, *imgVideoThermal, *sceneImg, *activityImg, *FGMap, *FGMapDouble, *blobImg, *dilated, *rec, *floorImg, *motionmap, *motionmapscaled, *motionmaptemp;
extern IplImage *mask, *lab, *weight, *l_plane, *a_plane, *b_plane;
extern IplImage *zbuffer, *body_zbuffer, *fbuffer, *fg, *bg, *temp_bg, *temp, *ibg, *ifg, *biggy;
extern IplImage* body_mask, *blob_mask;
extern IplImage* floormap;
extern IplImage* obstacle_disp;
extern IplImage* body_icon;
extern IplImage* door_mask;
extern IplImage* distance_mask;
extern IplImage *velx, *vely; 
extern IplImage *temp32; 
extern CvRNG rnd_seed;
extern CProgressDlg* progressDlg;
extern CCritSec dataCriticalSection, recordCriticalSection;
extern CEvent	colorFrameReady, thermalFrameReady, colorTrackerReady, thermalTrackerReady;
// ------------------------------------------------------------------------
CommaDelimitedStringCollection^ GetUserSettingsCommaList(String^ sname);
// -----------    Mathematical functions  ---------------------------------
int round(double a);
int64 round64(double a);
double deg2rad(double angle);
double rad2deg(double angle);
double metersSQ2feetSQ(double smeters);
double meters2feet(double meters);
double probNormal(double x,	double mean, double std);
double probNormalTruncated(double x, double mean, double std, double lo, double hi, double scale);
double log2(double a);
double sign(double a);
//template <typename T> void swap(T &a, T &b);
//template <typename T>T median(T a[], int sz);
double sigmoid(double t, double shift=0, double scale=1);

// ------------------------------------------------------------------------
// -------------- Recording control ---------------------------------------
extern int						m_recordVideoView, 
								m_recordSceneView, 
								m_recordActivityView, 
								m_recordFloorView;     
extern CvVideoWriter			*aviWriterInputView, 
								*aviWriterOutputView, 
								*aviWriterActivityView, 
								*aviWriterFloorView;   // video writers
// --------------------------------------------------------------------------
void CreateVideoWriters(CString fileName, double frameRate);
// ------------------------------------------------------------------------
// -----------------    Drawing functions ---------------------------------
void drawText(CString text, CvPoint p, IplImage* frame, CvScalar t, CvScalar bg, float size);
void drawText(double i, CvPoint p, IplImage* frame, CvScalar t, CvScalar bg, float size);
void drawTextInCircle(CString text, CvPoint p, IplImage* frame, CvScalar t, CvScalar bg, float size, int min_radius=0);
void drawTextInCircle(double i, CvPoint p, IplImage* frame, CvScalar t, CvScalar bg, float size, int min_radius=0);
void drawTextInRectangle(CString text, CvPoint p, IplImage* frame, CvScalar t, CvScalar bg, float size);
void drawTextInRectangle(double i, CvPoint p, IplImage* frame, CvScalar t, CvScalar bg, float size);
void drawTextInRectangle(double i, CvPoint p, CDC* pDC, float size);
void drawEditableToolTip(CWnd* pParent, CString& text, CRect rect);
void drawStaticToolTip(CDC* pDC, CString& text, CRect rect);
CvScalar colorLighter(CvScalar color, int step=10);
CvScalar colorDarker(CvScalar color, int step=10);
CvScalar colorFromID(int id);
// ------------------------------------------------------------------------
// --------    Dialogs ----------------------------------------------------
void ShowProgressDialog(LPCTSTR title, int range=10);
void SetProgressDialogTitle(LPCTSTR title);
void SetProgressDialogMessage(LPCTSTR msg);
void SetProgressDialogRange(int start, int stop);
void IncProgressDialogPos(int ipos);
void SetProgressDialogPos(int pos);
void StepProgressDialog(LPCTSTR msg="", int incr=1);
void HideProgressDialog();
// ------------------------------------------------------------------------
void RGB2Lab(double R, double G, double B, double* LAB);


// --------    Geometry functions  ---------------------------------
CvPoint       cvPoint( CPoint a );
CPoint        cPoint( CvPoint a );
CPoint        cPoint( CvPoint2D32f a );
CPoint        cPoint( double x, double y );
CvPoint2D32f  cvPoint2D32f( CPoint a );
struct IntPair { 
	IntPair() {;};
	IntPair(int i1, int i2) {id1 = i1; id2 = i2;};
	int id1; int id2; 
};
bool cvPointInRect(CvPoint p, CvRect r);
bool cvPointInPoly(CvPoint2D32f p, CArray<CvPoint2D32f> &poly);
bool outOfBounds(CvPoint2D32f p, IplImage* frame);
double d(CvPoint2D32f point, FloorRegion *z, int* index=NULL);
double d(CvPoint p1, CvPoint p2=cvPoint(0,0));
double d(CvPoint2D32f p1, CvPoint2D32f p2=cvPoint2D32f(0,0));
double d(CvPoint3D32f p1, CvPoint3D32f p2=cvPoint3D32f(0,0,0));
double d(CvPoint3D32f p1, CvPoint3D32f p2, CvPoint3D32f sigma);
double d_2(CvPoint p1, CvPoint p2=cvPoint(0,0));
double d_2(CvPoint2D32f p1, CvPoint2D32f p2=cvPoint2D32f(0,0));
double d_2(CvPoint3D32f p1, CvPoint3D32f p2=cvPoint3D32f(0,0,0));
double d_2(CvPoint3D32f p1, CvPoint3D32f p2, CvPoint3D32f sigma);
double d_2(CvPoint2D32f p1, CvPoint2D32f p2, CvPoint2D32f sigma);
double d_angle(double a1, double a2);
double angle(CvPoint2D32f p1, CvPoint2D32f p2=cvPoint2D32f(0,0));
double angle(CvPoint3D32f p1, CvPoint3D32f p2=cvPoint3D32f(0,0,0));
double dotProduct(CvPoint3D32f p1, CvPoint3D32f p2);
double dotProduct(CvPoint2D32f p1, CvPoint2D32f p2);
double d_contour2contour(CvSeq* c1, CvSeq* c2);
double d2line(CvPoint2D32f p, CvPoint2D32f p1, CvPoint2D32f p2);
double d2lineSegment(CvPoint2D32f p, CvPoint2D32f p1, CvPoint2D32f p2);
void pointAtD2line(double d, CvPoint2D32f p1, CvPoint2D32f p2, CvPoint2D32f &b1, CvPoint2D32f &b2);
// ------------------------------------------------------------------------
// define In() operator for CArray
template<class T> bool In(CArray<T>& a, T n) {
	for (int i=0;i < a.GetCount();i++)
		if (a[i] == n)
			return true;
	return false;
}
// ------------------------------------------------------------------------
// define Find() operator for CArray
template<class T> int Find(CArray<T>& a, T n) {
	for (int i=0;i < a.GetCount();i++)
		if (a[i] == n)
			return i;
	return -1;
}

// -- Debugging functions -------------------------------------------------
void bug();
void gub();
void tic();
void toc();


// ---------- Array utilities ---------------------------------------------
// --------------------------------------------------------------------------
template <typename T>
T median(T a[], int sz) {
	quicksort(a, 0, sz-1);
	if ((sz % 2) != 0 || sz == 0)
		return a[sz/2];
	else
		return (a[sz/2]+a[sz/2+1])/2;

}
// --------------------------------------------------------------------------
template <typename T>
T medianOf5(T aTmp[]) {
	// find the median of medians
	if (aTmp[1] < aTmp[0]) 
		swap(aTmp[0], aTmp[1]);
	if (aTmp[2] < aTmp[0])
		swap(aTmp[0], aTmp[2]); 
	if (aTmp[3] < aTmp[0])
		swap(aTmp[0], aTmp[3]);
	if (aTmp[4] < aTmp[0])
		swap(aTmp[0], aTmp[4]);

	if (aTmp[2] < aTmp[1])
		swap(aTmp[1], aTmp[2]);
	if (aTmp[3] < aTmp[1])
		swap(aTmp[1], aTmp[3]);
	if (aTmp[4] < aTmp[1])
		swap(aTmp[1], aTmp[4]);

	if (aTmp[3] < aTmp[2])
		swap(aTmp[2], aTmp[3]);
	if (aTmp[4] < aTmp[2])
		swap(aTmp[2], aTmp[4]);
	return aTmp[2];
}
// ------------------------------------------------------------------------
template <typename T> 
void swap(T &a, T& b) {
	T t = a;
	a = b;
	b = t;
}
// --------------------------------------------------------------------------
template <typename T>
void select(T a[], int size, int k) {
	int newMOMIdx = partition(a, size, k);
	if (k < newMOMIdx)
		select(a, newMOMIdx, k);
	else if (k > newMOMIdx)
		select(a + newMOMIdx + 1, size - newMOMIdx - 1, k - newMOMIdx - 1);
}
// --------------------------------------------------------------------------
template <typename T>
void quicksort(T a[], int top, int bottom)
{
	// top = subscript of beginning of vector being considered
	// bottom = subscript of end of vector being considered
	// this process uses recursion - the process of calling itself
	int middle;
	if (top < bottom)
	{
		middle = partition(a, top, bottom);
		quicksort(a, top, middle);   // sort top partition
		quicksort(a, middle+1, bottom);    // sort bottom partition
	}
	return;
}
// --------------------------------------------------------------------------
//Function to determine the partitions
// partitions the array and returns the middle index (subscript)
template <typename T>
int partition(T a[], int top, int bottom) {
	T x = a[top];
	int i = top - 1;
	int j = bottom + 1;
	T temp;
	do
	{
		do     
		{
			j--;
		} while (x >a[j]);

		do  
		{
			i++;
		} while (x <a[i]);

		if (i < j)
		{ 
			temp = a[i];    // switch elements at positions i and j
			a[i] = a[j];
			a[j] = temp;
		}
	} while (i < j);    
	return j;           // returns middle index
}
#endif // !defined( MYOPENCVFUNCTIONS_H )