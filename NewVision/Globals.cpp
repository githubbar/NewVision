#include "StdAfx.h"
#include "Globals.h"
#include "EditableTooltip.h"
#include "NewVisionDoc.h"
#include "BodyPath.h"
#include "FloorRegion.h"
#include "ClusteringMachine.h"
#include "SwarmEvent.h"
#include "SwarmActivity.h"

#using <mscorlib.dll>
#using <System.dll>

using namespace System;
using namespace System::Diagnostics;
using namespace System::IO;

int NTEMPERATURES = 20;
OpenCVExtensions::Pixel HEAT_COLOR[] = {
	Pixel(  0,  0,255),
	Pixel(  0, 51,255),
	Pixel(  0,102,255),
	Pixel(  0,153,255),
	Pixel(  0,204,255),

	Pixel(  0,255,255),
	Pixel(  0,255,204),
	Pixel(  0,255,153),
	Pixel(  0,255,102),
	Pixel(  0,255, 51),

	Pixel(  0,255,  0),
	Pixel( 51,255,  0),
	Pixel(102,255,  0),
	Pixel(153,255,  0),
	Pixel(204,255,  0),

	Pixel(255,204,  0),
	Pixel(255,153,  0),
	Pixel(255,102,  0),
	Pixel(255, 51,  0),
	Pixel(255,  0,  0)
};

// Global variables, including temporary OpenCV buffers (clean up every frame)
CvMemStorage* OpenCVmemory;
IplImage *imgFrame, *imgOverlayInfo, *imgPreviousFrame, *imgVideo, *imgVideoThermal, *imgThermal, *sceneImg, *activityImg, *FGMap, *FGMapDouble, *blobImg, *dilated, *rec, *floorImg, *motionmap, *motionmapscaled, *motionmaptemp;
IplImage *mask, *lab, *weight, *l_plane, *a_plane, *b_plane;
IplImage *zbuffer, *body_zbuffer, *fbuffer, *fg, *bg, *temp, *temp_bg, *ifg, *ibg, *biggy;
IplImage *body_mask, *blob_mask;
IplImage* floormap;
IplImage* obstacle_disp;
IplImage* body_icon;
IplImage* door_mask;
IplImage* distance_mask;
IplImage *velx, *vely;
IplImage *temp32; 
CvRNG rnd_seed;
CProgressDlg* progressDlg;
clock_t start;
CCritSec dataCriticalSection, recordCriticalSection;
CEvent	colorFrameReady(FALSE, FALSE), thermalFrameReady(FALSE, FALSE), colorTrackerReady(FALSE, FALSE), thermalTrackerReady(FALSE, FALSE);

int								m_recordVideoView, 
								m_recordSceneView, 
								m_recordActivityView, 
								m_recordFloorView;     
struct CvVideoWriter{};
CvVideoWriter					*aviWriterInputView, 
								*aviWriterOutputView, 
								*aviWriterActivityView, 
								*aviWriterFloorView;   // video writers

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
CommaDelimitedStringCollection^ GetUserSettingsCommaList(String^ sname) {
	System::Configuration::KeyValueConfigurationCollection ^settings
		= ConfigurationManager::OpenExeConfiguration(ConfigurationUserLevel::None)->AppSettings->Settings;
	KeyValueConfigurationElement ^tags = settings[sname];
	if (!tags) {
		AfxMessageBox(CString("XML file with " + sname + " definitions not found!"));
		return nullptr;
	}
	CommaDelimitedStringCollectionConverter ^cv = gcnew CommaDelimitedStringCollectionConverter();
	CommaDelimitedStringCollection ^values2names = (CommaDelimitedStringCollection ^)cv->ConvertFromString(tags->Value);
	return values2names;
}
// --------------------------------------------------------------------------
// ------------------------------------------------------------------------
void drawText(CString text, CvPoint p, IplImage* frame, CvScalar t, CvScalar bg, float size) {
	CvFont  font;
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, size, size);
	CvSize sz;
	int baseline;
	cvGetTextSize(text, &font, &sz, &baseline );
	//cvRectangle(frame, cvPoint(p.x, p.y-1), cvPoint(p.x+sz.width, p.y+sz.height+1), bg, CV_FILLED, CV_AA);
	cvPutText(frame, text, cvPoint(p.x, p.y+sz.height), &font, t);
};
// ------------------------------------------------------------------------
void drawText(double i, CvPoint p, IplImage* frame, CvScalar t, CvScalar bg, float size) {
	CString text;
	if ((double)((int)i) == i) 
		text.Format("%-d", (int)i);
	else
		text.Format("%-.2f", i);
	drawText(text, p, frame, t, bg, size);
};
// ------------------------------------------------------------------------
void drawTextInRectangle(CString text, CvPoint p, IplImage* frame, CvScalar t, CvScalar bg, float size) {
	CvFont  font;
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, size, size);
	CvSize sz;
	int baseline;
	const OFFSET = 3;
	cvGetTextSize(text, &font, &sz, &baseline );
	cvRectangle(frame, cvPoint(p.x - OFFSET, p.y - OFFSET), 
		cvPoint(p.x+sz.width+OFFSET, p.y+sz.height+OFFSET), bg, CV_FILLED, CV_AA);
	cvPutText(frame, text, cvPoint(p.x, p.y+sz.height), &font, t);
}
// ------------------------------------------------------------------------
void drawTextInRectangle(double i, CvPoint p, IplImage* frame, CvScalar t, CvScalar bg, float size) {
	CString text;
	if ((double)((int)i) == i) 
		text.Format("%-d", (int)i);
	else
		text.Format("%-.2f", i);
	drawTextInRectangle(text, p, frame, t, bg, size);
}
// ------------------------------------------------------------------------
void drawTextInRectangle( double i, CvPoint p, CDC* pDC, float size )
{
	CString text;
	if ((double)((int)i) == i) 
		text.Format("%-d", (int)i);
	else
		text.Format("%-.2f", i);
	const int ROUND = 5;
	CRect rect;
	pDC->DrawText(text, &rect, DT_CALCRECT);
	pDC->RoundRect(&rect, CPoint(ROUND, ROUND));
	pDC->DrawText(text, &rect, DT_NOCLIP);
	

}
// ------------------------------------------------------------------------
void drawTextInCircle(CString text, CvPoint p, IplImage* frame, CvScalar t, CvScalar bg, float size, int min_radius) {
	CvFont  font;
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, size, size);
	CvSize sz;
	int baseline;
	if (min_radius) {
		sz.width = min_radius;
		sz.height = min_radius;
	}
	else
		cvGetTextSize(text, &font, &sz, &baseline );
	cvCircle(frame, cvPoint(p.x+sz.width/2, p.y+sz.height/2), sz.width/2+2, bg, CV_FILLED, CV_AA);
	cvPutText(frame, text, cvPoint(p.x, p.y+sz.height), &font, t);
};
// ------------------------------------------------------------------------
void drawTextInCircle(double i, CvPoint p, IplImage* frame, CvScalar t, CvScalar bg, float size, int min_radius) {
	CString text;
	if ((double)((int)i) == i) 
		text.Format("%-d", (int)i);
	else
		text.Format("%-.2f", i);
	drawTextInCircle(text, p, frame, t, bg, size, min_radius);
};
// ------------------------------------------------------------------------
void drawEditableToolTip(CWnd* pParent, CString& text, CRect rect) {
		CEditableTooltip* enter = new CEditableTooltip(pParent, rect, text);
		if (enter->DoModal() == IDOK)
			enter->GetWindowText(text);
		enter->CloseWindow();
		delete enter;
}
// ------------------------------------------------------------------------
void drawStaticToolTip(CDC* pDC, CString& text, CRect rect) {
		if (text.IsEmpty())
			return;
		CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
		CBrush brush(RGB(255, 255, 231));
		CPen* pOldPen = pDC->SelectObject(&pen);
		CBrush* pOldBrush= pDC->SelectObject(&brush);

		pDC->RoundRect(rect, CPoint(5, 5));
		int oldBkMode = pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(text, &rect, DT_CENTER | DT_SINGLELINE);
		pDC->SetBkMode(oldBkMode);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
}
// ------------------------------------------------------------------------
CvScalar colorLighter(CvScalar color, int step) {
	return cvScalar(min(255, color.val[0]+step), min(255, color.val[1]+step), min(255, color.val[2]+step), color.val[3]);
}
// ------------------------------------------------------------------------
CvScalar colorDarker(CvScalar color, int step) {
	return cvScalar(color.val[0]-step, color.val[1]-step, color.val[2]-step, color.val[3]);
}
// ------------------------------------------------------------------------
CvScalar colorFromID(int id) {
	srand(id*100);
	return cvScalar(rand() % 256, rand() % 256, rand() % 256);
}
// ------------------------------------------------------------------------
void ShowProgressDialog(LPCTSTR title, int range) {
	progressDlg = new CProgressDlg();
	progressDlg->Create(IDD_PROGRESS_DIALOG);
	progressDlg->SetWindowText(title);
	progressDlg->m_progress.SetRange(0, range);
	progressDlg->ShowWindow(SW_SHOW);
}
// ------------------------------------------------------------------------
void SetProgressDialogRange(int start, int stop) {
	if (progressDlg) {
		progressDlg->m_progress.SetRange(start, stop);
		progressDlg->m_progress.SetPos(start);
	}
}
// ------------------------------------------------------------------------
void SetProgressDialogTitle(LPCTSTR title) {
	if (progressDlg)
		progressDlg->SetWindowText(title);
}
// ------------------------------------------------------------------------
void SetProgressDialogMessage(LPCTSTR msg) {
	if (progressDlg)
		progressDlg->SetMessage(msg);
}
// ------------------------------------------------------------------------
void IncProgressDialogPos(int incr) {
	if (progressDlg)
		progressDlg->m_progress.SetPos(progressDlg->m_progress.GetPos()+incr);
}
// ------------------------------------------------------------------------
void StepProgressDialog(LPCTSTR msg, int incr) {
	if (progressDlg) {
		progressDlg->m_progress.SetPos(progressDlg->m_progress.GetPos()+incr);
		if (msg != "")
			progressDlg->SetMessage(msg);
	}
	progressDlg->ShowWindow(SW_SHOW);
}
// ------------------------------------------------------------------------
void SetProgressDialogPos(int pos) {
	if (progressDlg)
		progressDlg->m_progress.SetPos(pos);
}
// ------------------------------------------------------------------------
void HideProgressDialog() {
	delete progressDlg;
	progressDlg = NULL;
}
// ------------------------------------------------------------------------
CvPoint cvPoint( CPoint a )
{
    CvPoint p;
    p.x = a.x;
    p.y = a.y;
    return p;
}
// ------------------------------------------------------------------------
CPoint cPoint( CvPoint a )
{
    CPoint p;
    p.x = a.x;
    p.y = a.y;
    return p;
}
// ------------------------------------------------------------------------
CPoint cPoint( CvPoint2D32f a )
{
    CPoint p;
    p.x = round(a.x);
    p.y = round(a.y);
    return p;
}
// ------------------------------------------------------------------------
CPoint cPoint( double x, double y )
{
    CPoint p;
    p.x = (LONG)x;
    p.y = (LONG)y;
    return p;
}
// ------------------------------------------------------------------------
CvPoint2D32f cvPoint2D32f( CPoint a )
{
    CvPoint2D32f p;
    p.x = (float)a.x;
    p.y = (float)a.y;
    return p;
}
// ------------------------------------------------------------------------
bool cvPointInRect(CvPoint p, CvRect r) {
	if (p.x >= r.x && p.x < r.x+r.width && p.y >= r.y && p.y < r.y+r.height)
		return true;
	else
		return false;
}
// ------------------------------------------------------------------------
bool cvPointInPoly(CvPoint2D32f p, CArray<CvPoint2D32f> &poly) {
	int N = poly.GetCount();
	bool c = false;
	for (int i = 0, j = N-1; i < N; j = i++) {
	if ((((poly[i].y < p.y) && (p.y < poly[j].y)) ||
		 ((poly[j].y < p.y) && (p.y < poly[i].y))) &&
		(p.x < (poly[j].x - poly[i].x) * (p.y - poly[i].y) / (poly[j].y - poly[i].y) + poly[i].x))

	  c = !c;
	}
	return c;
}
// ------------------------------------------------------------------------
int round(double a) {
	if (a > 0)
		return int(a + 0.5);
	else 
		return int(a - 0.5);
}
// ------------------------------------------------------------------------
int64 round64(double a) {
	if (a > 0)
		return int64(a + 0.5);
	else 
		return int64(a - 0.5);
}
// --------------------------------------------------------------------------
bool outOfBounds(CvPoint2D32f p, IplImage* frame) {
	if (p.x < 0 || p.x > frame->width || p.y < 0 || p.y > frame->height)
		return true;
	return false;
}
// ------------------------------------------------------------------------
double d(CvPoint2D32f point, FloorRegion *z, int* index) {
	double minDist = DBL_MAX;
	int minI = 0;
	for (int i=0;i<z->p.GetCount();i++) {
		double dNow = d(z->p[i], point);
		if (dNow <= minDist) {
			minDist = dNow;
			minI = i;
		}
	}
	if (index != NULL)		
		*index = minI;

	return minDist;
}
// ------------------------------------------------------------------------
double d(CvPoint p1, CvPoint p2) {
	return sqrt(d_2(p1,p2));
}
// ------------------------------------------------------------------------
double d(CvPoint3D32f p1, CvPoint3D32f p2) {
	return sqrt(d_2(p1,p2));
}
// ------------------------------------------------------------------------
double d(CvPoint2D32f p1, CvPoint2D32f p2) {
	return sqrt(d_2(p1,p2));
}
// ------------------------------------------------------------------------
double d(CvPoint3D32f p1, CvPoint3D32f p2, CvPoint3D32f sigma){
	return sqrt(d_2(p1,p2,sigma));
}
// ------------------------------------------------------------------------
double d_2(CvPoint p1, CvPoint p2) {
	return (double)((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}
// ------------------------------------------------------------------------
double d_2(CvPoint2D32f p1, CvPoint2D32f p2) {
	return (double)((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}
// ------------------------------------------------------------------------
double d_2(CvPoint3D32f p1, CvPoint3D32f p2) {
	return (double)((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)+(p1.z-p2.z)*(p1.z-p2.z));
}
// ----------------------------------------------------------
double d_2(CvPoint3D32f p1, CvPoint3D32f p2, CvPoint3D32f sigma) {
	CvPoint3D32f sigma2 = cvPoint3D32f(sigma.x*sigma.x, sigma.y*sigma.y, sigma.z*sigma.z);
	return (double)((p1.x-p2.x)*(p1.x-p2.x)/sigma2.x+(p1.y-p2.y)*(p1.y-p2.y)/sigma2.y+(p1.z-p2.z)*(p1.z-p2.z)/sigma2.z);
}
// ----------------------------------------------------------
double d_2(CvPoint2D32f p1, CvPoint2D32f p2, CvPoint2D32f sigma) {
	CvPoint2D32f sigma2 = cvPoint2D32f(sigma.x*sigma.x, sigma.y*sigma.y);
	return (double)((p1.x-p2.x)*(p1.x-p2.x)/sigma2.x+(p1.y-p2.y)*(p1.y-p2.y)/sigma2.y);
}
// --------------------------------------------------------------------------
double dotProduct(CvPoint3D32f p1, CvPoint3D32f p2) {
	return p1.x*p2.x+p1.y*p2.y+p1.z*p2.z;
}
// --------------------------------------------------------------------------
double dotProduct(CvPoint2D32f p1, CvPoint2D32f p2) {
	return p1.x*p2.x+p1.y*p2.y;
}
// --------------------------------------------------------------------------
double d_angle(double a1, double a2) {
	// a1 and a2 are between 0 and 2PI
	a1 = fmod(a1, 2*PI);
	a2 = fmod(a2, 2*PI);

	if (a1 - a2 > PI)
		return abs(a2-a1-2*PI);
	else if (a2 - a1 > PI)
		return abs(a1-a2-2*PI);
	else
		return abs(a2-a1);
}
// --------------------------------------------------------------------------
double angle(CvPoint2D32f p1, CvPoint2D32f p2) {
	return atan2(p2.y-p1.y, p2.x-p1.x);
}
// --------------------------------------------------------------------------
double angle(CvPoint3D32f p1, CvPoint3D32f p2) {
	return atan2(p2.y-p1.y, p2.x-p1.x);
}
// ----------------------------------------------------------
double d_contour2contour(CvSeq* c1, CvSeq* c2) {
	if(!CV_IS_SEQ_POLYLINE( c1 ) || !CV_IS_SEQ_POLYLINE( c2 ))
		return -1;
	int count1 = c1->total;
	int count2 = c2->total;
	CvSeqReader reader1;
	CvSeqReader reader2;
	cvStartReadSeq(c1, &reader1, 0);
	cvStartReadSeq(c2, &reader2, 0);
	CvPoint pt1, pt2;
	CV_READ_SEQ_ELEM( pt1, reader1 );
	double minDist = HUGE_VAL;
	for(int i1 = 0; i1 < count1; i1++ ) {
		CV_READ_SEQ_ELEM( pt1, reader1 );
		for(int i2 = 0; i2 < count2; i2++ )
			CV_READ_SEQ_ELEM( pt2, reader2 );
		double dist = d_2(pt1, pt2);
		if (dist < minDist)
			minDist = dist;
	}
	return sqrt(minDist);
}
// --------------------------------------------------------------------------
double d2line(CvPoint2D32f p, CvPoint2D32f p1, CvPoint2D32f p2) {
	double denom = sqrt((p2.x-p1.x)*(p2.x-p1.x)+(p2.y-p1.y)*(p2.y-p1.y));
	if (denom == 0)
		return d(p, p1);
	return abs((p2.x-p1.x)*(p1.y-p.y) - (p1.x-p.x)*(p2.y-p1.y))/denom;
}
// --------------------------------------------------------------------------
double d2lineSegment(CvPoint2D32f p, CvPoint2D32f p1, CvPoint2D32f p2) {
	double denom = (p2.x-p1.x)*(p2.x-p1.x)+(p2.y-p1.y)*(p2.y-p1.y);
	if (denom == 0)
		return d(p, p1);
	double u = ((p.x-p1.x)*(p2.x-p1.x) + (p.y-p1.y)*(p2.y-p1.y))/denom;
	if (u < 0) 
		return d(p,p1);
	if (u > 1)
		return d(p,p2);

	// projection of p onto line [p1 p2]
	CvPoint2D32f pProj = cvPoint2D32f(p1.x+u*(p2.x-p1.x), p1.y+u*(p2.y-p1.y));
	return d(p, pProj);

}
// --------------------------------------------------------------------------
void pointAtD2line(double delta, CvPoint2D32f p1, CvPoint2D32f p2, CvPoint2D32f &b1, CvPoint2D32f &b2) {
	// Find two points b1, b2 at distance delta from p1 (p1,p2 is a line)
	double k = (p2.y-p1.y)/(p2.x-p1.x);
	double l = d(p2, p1);
	// find x' = x3 - x1
	b1.x = p1.x + delta*(p2.y-p1.y)/l;
	b1.y = p1.y - delta*(p2.x-p1.x)/l;
	
	b2.x = p1.x - delta*(p2.y-p1.y)/l;
	b2.y = p1.y + delta*(p2.x-p1.x)/l;
}
// ------------------------------------------------------------------------
double probNormal(double x, double mean, double std) {
	return 1.0/(std*sqrt(2*PI))*exp(-(x-mean)*(x-mean)/(2*std*std));
}
// ------------------------------------------------------------------------
double probNormalTruncated(double x, double mean, double std, double lo, double hi, double scale) {
	if (x < lo || x > hi )
		return 0;
	return exp(-scale*(x-mean)*(x-mean)/(std*std));
}
// ------------------------------------------------------------------------
double deg2rad(double angle) {
	return angle*PI/180;
}
// ------------------------------------------------------------------------
double rad2deg(double angle) {
	return angle*180/PI;
}
// ------------------------------------------------------------------------
double meters2feet(double meters) {
	return meters*FEET_IN_METER;
}
// ------------------------------------------------------------------------
double metersSQ2feetSQ(double smeters) {
	return smeters*SQFEET_IN_SQMETER;
}
// ------------------------------------------------------------------------
double log2(double a) {
	return log10(a)*log10((double)
		2);
}
// ------------------------------------------------------------------------
double sign(double a) {
	if (a>0)
		return 1;
	else if (a==0)
		return 0;
	else 
		return -1;
}
// ------------------------------------------------------------------------
double sigmoid(double t, double shift, double scale) {
	return 1 / (1+exp(-(t-shift)/scale));
}
// ------------------------------------------------------------------------
int min(int a, int b) {
	if (a<b) return a;
	else return b;
}

int max(int a, int b){
	if (a>b) return a;
	else return b;
}

// ------------------------------------------------------------------------
void RGB2Lab(double R, double G, double B, double* LAB) {
	IplImage* src = cvCreateImage(cvSize(1,1), IPL_DEPTH_8U, 3);
	IplImage* lab = cvCreateImage(cvSize(1,1), IPL_DEPTH_8U, 3);
	Pixel p((int)R, (int)G, (int)B);
	Image::setPixel(src, &p, 0, 0);
	cvCvtColor(src, lab, CV_BGR2Lab);
	Image::getPixel(lab, &p, 0, 0);
	cvReleaseImage(&src);
	cvReleaseImage(&lab);
	LAB[0] = p.B;
	LAB[1] = p.G;
	LAB[2] = p.R;
/*
	// rgb to xyz
	double x = 0.433910*r/255 + 0.376220*g/255 + 0.189860*b/255;
	double y = 0.212649*r/255 + 0.715169*g/255 + 0.072182*b/255;   
	double z = 0.017756*r/255 + 0.109478*g/255 + 0.872915*b/255;
	if (y>0.008856)
		lab[0] = 116*pow(y, 1./3);
	else
		lab[0] = 903.3*y;

	class f_
    {
    public:
      double operator()( double t )
      {
		  if (t >0.008856)
			return pow(t, 1./3);
		  else 
			return 7.787*t+16./116;
      }
    } f;

	lab[1] = 500*(f(x)-f(y));
	lab[2] = 200*(f(y)-f(z));
*/
}

// --------------------------------------------------------------------------
void CreateVideoWriters(CString fileName, double frameRate) {
	if (m_recordVideoView) {
		aviWriterInputView = cvCreateVideoWriter(
						(LPCTSTR)(fileName+"_VIDEO.avi"), 
						-1, 
						frameRate,
						cvSize(imgFrame->width, imgFrame->height) );
	}
	if (m_recordSceneView) {
		aviWriterOutputView = cvCreateVideoWriter(
						(LPCTSTR)(fileName+"_SCENE.avi"), 
						-1, 
						frameRate,
						cvSize(imgFrame->width, imgFrame->height) );
	}
	if (m_recordActivityView) {
		aviWriterActivityView = cvCreateVideoWriter(
						(LPCTSTR)(fileName+"_ACTIVITY.avi"), 
						-1, 
						frameRate,
						cvSize(imgFrame->width, imgFrame->height) );
	}
	if (m_recordFloorView) {
		aviWriterFloorView = cvCreateVideoWriter(
						(LPCTSTR)(fileName+"_FLOOR.avi"), 
						-1, 
						frameRate,
						cvSize(floorImg->width, floorImg->height) );
	}
}
// --------------------------------------------------------------------------
template <> void AFXAPI SerializeElements <FloorRegion*> ( CArchive& ar, FloorRegion** p, INT_PTR nCount )
{
    int i; 
    if (ar.IsStoring()) 
		for (i=0; i<nCount; i++) {
			p[i]->Serialize(ar); 
		}
    else 
		for (i=0; i<nCount; i++) { 
			p[i] = new FloorRegion();
            p[i]->Serialize(ar); 
		}
}
// --------------------------------------------------------------------------
template <> void AFXAPI SerializeElements <BodyPath*> ( CArchive& ar, BodyPath** p, INT_PTR nCount )
{
    int i; 
	SetProgressDialogRange(0, nCount);
    if (ar.IsStoring()) 
		for (i=0; i<nCount; i++) {
			SetProgressDialogPos(i);
			p[i]->Serialize(ar); 
		}
    else 
		for (i=0; i<nCount; i++) { 
			SetProgressDialogPos(i);
			p[i] = new BodyPath();
            p[i]->Serialize(ar); 
		}
}
// --------------------------------------------------------------------------
template <> void AFXAPI SerializeElements <Step> ( CArchive& ar, Step* p, INT_PTR nCount ) {
 	for (int i=0; i<nCount; i++)
			p[i].Serialize(ar); 
}
// ------------------------------------------------------------------------
template <> void AFXAPI SerializeElements <CvPoint2D32f> ( CArchive& ar, CvPoint2D32f* p, INT_PTR nCount )
{
	for ( int i = 0; i < nCount; i++, p++ ) {
		if( ar.IsStoring())
			ar << p->x << p->y;
		else
			ar >> p->x >> p->y;
	}
}
// ------------------------------------------------------------------------
template <> void AFXAPI SerializeElements <CvPoint> ( CArchive& ar, CvPoint* p, INT_PTR nCount )
{
	for ( int i = 0; i < nCount; i++, p++ ) {
		if( ar.IsStoring())
			ar << p->x << p->y;
		else
			ar >> p->x >> p->y;
	}
}
// ------------------------------------------------------------------------
template <> void AFXAPI SerializeElements <CLine> ( CArchive& ar, CLine* l, INT_PTR nCount )
{
	for ( int i = 0; i < nCount; i++, l++ ) {
		if( ar.IsStoring())
			ar << l->p1.x << l->p1.y << l->p2.x << l->p2.y;
		else
			ar >> l->p1.x >> l->p1.y >> l->p2.x >> l->p2.y;
	}
}
// --------------------------------------------------------------------------
template <> void AFXAPI SerializeElements <SwarmEvent*> ( CArchive& ar, SwarmEvent** p, INT_PTR nCount )
{
    int i; 
	SetProgressDialogRange(0, nCount);
    if (ar.IsStoring()) 
		for (i=0; i<nCount; i++) {
			SetProgressDialogPos(i);
			p[i]->Serialize(ar); 
		}
    else 
		for (i=0; i<nCount; i++) { 
			SetProgressDialogPos(i);
			p[i] = new SwarmEvent();
            p[i]->Serialize(ar); 
			if (p[i]->id > SwarmEvent::last_id)
				SwarmEvent::last_id = p[i]->id;
		}
}

// --------------------------------------------------------------------------
template <> void AFXAPI SerializeElements <SwarmActivity*> ( CArchive& ar, SwarmActivity** p, INT_PTR nCount )
{
    int i; 
	SetProgressDialogRange(0, nCount);
    if (ar.IsStoring()) 
		for (i=0; i<nCount; i++) {
			SetProgressDialogPos(i);
			p[i]->Serialize(ar); 
		}
    else 
		for (i=0; i<nCount; i++) { 
			SetProgressDialogPos(i);
			p[i] = new SwarmActivity();
            p[i]->Serialize(ar); 
			if (p[i]->id > SwarmActivity::last_id)
				SwarmActivity::last_id = p[i]->id;
		}
}
// --------------------------------------------------------------------------
template <> void AFXAPI SerializeElements <Record*> ( CArchive& ar, Record** p, INT_PTR nCount )
{
	int i; 
	SetProgressDialogRange(0, nCount);
	if (ar.IsStoring()) 
		for (i=0; i<nCount; i++) {
			SetProgressDialogPos(i);
			p[i]->Serialize(ar); 
		}
	else 
		for (i=0; i<nCount; i++) { 
			SetProgressDialogPos(i);
			p[i] = new Record();
			p[i]->Serialize(ar); 
		}
}
// ------------------------------------------------------------------------
template<> 
UINT AFXAPI HashKey<IntPair&> (IntPair& key)
{
	return( UINT( key.id1 ) ^ UINT( key.id2 ) );
}
// ------------------------------------------------------------------------
template<>
BOOL AFXAPI CompareElements<IntPair , IntPair> (const IntPair* p1, const IntPair* p2)
{
	if ( p1->id1 == p2->id1 && p1->id2 == p2->id2 ) 
		return true;
	else
		return false;
}
// ------------------------------------------------------------------------
#ifdef _DEBUG
	#define new DEBUG_NEW
	//#define _CRTDBG_MAP_ALLOC
	CMemoryState oldMemState, newMemState, diffMemState;
	void bug() {
		AfxEnableMemoryTracking(TRUE);
		//afxMemDF = allocMemDF | checkAlwaysMemDF;
		oldMemState.Checkpoint();
	}
	void gub() {
		newMemState.Checkpoint();
		if( diffMemState.Difference( oldMemState, newMemState ) )
		{
			diffMemState.DumpAllObjectsSince();
		}
		AfxEnableMemoryTracking(FALSE);
		//_CrtDumpMemoryLeaks();
		//_CrtMemDumpAllObjectsSince(NULL);
	}

	void tic() {
	   start = clock();
	}

	void toc() {
		clock_t finish = clock();
		double duration = (double)(finish - start);
		echo(String::Format("It took {0} milliseconds", duration));
	}
#endif
