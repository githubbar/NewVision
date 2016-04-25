#include "StdAfx.h"
#include "LABHistogram2D.h"
#include "math.h"
#include "Globals.h"
// ------------------------------------------------------------------------
LABHistogram2D::LABHistogram2D(void) {
	/* hue varies from 0 (~0°red) to 180 (~360°red again) */
	/* saturation varies from 0 (black-gray-white) to 255 (pure spectrum color) */
    int hist_size[] = {a_bins, b_bins};
	float a_ranges[] = { (float)a_min, (float)a_max }; /* A varies from a_min to a_max */
	float b_ranges[] = { (float)b_min, (float)b_max }; /* B varies from b_min to b_max */
	float* ranges[] = { a_ranges, b_ranges };
 	h = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	cvClearHist(h);
}
// ------------------------------------------------------------------------
LABHistogram2D::LABHistogram2D(const LABHistogram2D& that) {
	cvCopyHist(that.h, &this->h);
}
// ------------------------------------------------------------------------
LABHistogram2D& LABHistogram2D::operator=(const LABHistogram2D& that) {
	cvCopyHist(that.h, &this->h);
	return *this;
}
// ------------------------------------------------------------------------
LABHistogram2D::~LABHistogram2D() {
	cvReleaseHist(&h);
}
// ------------------------------------------------------------------------
void LABHistogram2D::Clear() {
	cvClearHist(h);
}
// ------------------------------------------------------------------------
double LABHistogram2D::GetValue(double a, double b) {
	if (a < a_min || a >= a_max || b < b_min || b >= b_max)
		return 0;
	int A = (int)((a-a_min)*a_bins/(a_max-a_min));
	int B = (int)((b-b_min)*b_bins/(b_max-b_min));
	return cvQueryHistValue_2D(h, A, B);
}
// ------------------------------------------------------------------------
double LABHistogram2D::GetValue(CvScalar bgr) {
	double lab[3];
	RGB2Lab(bgr.val[2], bgr.val[1], bgr.val[0], lab);
	return GetValue(lab[1], lab[2]);
}
// ------------------------------------------------------------------------
void LABHistogram2D::AddValue(CvScalar bgr) {
	double lab[3];
	RGB2Lab(bgr.val[2], bgr.val[1], bgr.val[0], lab);
	if (lab[1] < a_min || lab[1] >= a_max || lab[2] < b_min || lab[2] >= b_max)
		return;
	int A = (int)((lab[1]-a_min)*a_bins/(a_max-a_min));
	int B = (int)((lab[2]-b_min)*b_bins/(b_max-b_min));
	float* value = cvGetHistValue_2D(h, A, B);
	(*value)++;
}
// ------------------------------------------------------------------------
void LABHistogram2D::Serialize( CArchive& ar )
{
    if( !CV_IS_HIST(h))
        ASSERT(false);
	
	int size[CV_MAX_DIM];
    int dims = cvGetDims( this->h->bins, size );
	int total = 1;
	for(int i = 0; i < dims; i++ )
		total *= size[i];

	//// TEMP:
	//total = 30*30;
	float *ptr = 0;
    cvGetRawData( this->h->bins, (uchar**)&ptr);
	if (ar.IsStoring()) {
		for(int i = 0; i < total; i++ )
			ar << ptr[i];
	}
	else {
		for(int i = 0; i < total; i++ )
			ar >> ptr[0];
	}
	
}
// ------------------------------------------------------------------------
void LABHistogram2D::Draw(IplImage* frame, int scale, CvPoint p) {
	int iscale = scale, jscale = scale;
	if (scale == 0) {
		iscale = frame->width/a_bins;
		jscale = frame->height/b_bins;
	}
	else {
		iscale = scale; jscale = scale;
	}

	float min_val = 0, max_val = 0;
	cvGetMinMaxHistValue( h, &min_val, &max_val, 0, 0 );
	for(int i=0; i<a_bins;i++ )
	{
		for(int j=0; j<b_bins;j++ )
		{
			float bin_val = cvQueryHistValue_2D( h, i, j );
			int index = (int)(NTEMPERATURES*(bin_val-min_val-1)/(max_val-min_val));
			cvRectangle( frame, cvPoint( p.x+i*iscale, p.y+j*jscale ),
						cvPoint( (i+1)*iscale - 1 + p.x, (j+1)*jscale - 1 + p.y),
						CV_RGB(HEAT_COLOR[index].R, HEAT_COLOR[index].G, HEAT_COLOR[index].B), CV_FILLED );
		}
	}
}
// ------------------------------------------------------------------------
void LABHistogram2D::Draw(CDC* pDC) {
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&pen);

	float min_value = 0, max_value = 0;
	cvGetMinMaxHistValue( h, &min_value, &max_value, 0, 0 );
	CRect box;
	pDC->GetWindow()->GetWindowRect(&box);
	int iscale = box.Width()/a_bins;
	int jscale = box.Height()/b_bins;

	for(int i=0; i<a_bins;i++ )
	{
		for(int j=0; j<b_bins;j++ )
		{
			float bin_val = cvQueryHistValue_2D( h, i, j );
			int intensity = cvRound((bin_val-min_value)*255/(max_value-min_value));
			CBrush fill;
			fill.CreateSolidBrush(RGB(0,0,intensity));   
			CRect r(i*iscale, j*jscale, (i+1)*iscale - 1, (j+1)*jscale - 1);
			pDC->FillRect(&r, &fill);
		}
	}
	pDC->SelectObject(pOldPen);
}
// ------------------------------------------------------------------------

double LABHistogram2D::Compare(const LABHistogram2D* that) {
	if( !CV_IS_HIST(h) || !CV_IS_HIST(that->h) )
        ASSERT(false);
	
	int size1[CV_MAX_DIM], size2[CV_MAX_DIM];

    int dims1 = cvGetDims( this->h->bins, size1 );
    int dims2 = cvGetDims( that->h->bins, size2 );
	int total = 1;

    if( dims1 != dims2 )
        ASSERT(false);

	for(int i = 0; i < dims1; i++ )
	{
		if( size1[i] != size2[i] )
			ASSERT(false);
		total *= size1[i];
	}

	float *ptr1 = 0, *ptr2 = 0;
    cvGetRawData( this->h->bins, (uchar**)&ptr1 );
    cvGetRawData( that->h->bins, (uchar**)&ptr2 );
	float sum = 0, sum1 = 0, sum2 = 0;
    for(int i = 0; i < total; i++ )
    {
        float a = ptr1[i];
        float b = ptr2[i];
        sum += sqrt(a*b);
		sum1 += a; sum2 +=b;
    }

	// normalize both histograms so that all bins sum up to 1
	if (sum1 == 0 || sum2 == 0)
		return 1;
	return sum/sqrt(sum1*sum2);
}
// ------------------------------------------------------------------------
double LABHistogram2D::GetNorm() {
    if( !CV_IS_HIST(h))
        ASSERT(false);
	
	int size[CV_MAX_DIM];

    int dims = cvGetDims( this->h->bins, size );
	int total = 1;

	for(int i = 0; i < dims; i++ )
		total *= size[i];

	float *ptr = 0;
    cvGetRawData( this->h->bins, (uchar**)&ptr);
	float sum = 0;
    for(int i = 0; i < total; i++ )
		sum += ptr[i];

	return sum;
}
// ------------------------------------------------------------------------
System::String^ LABHistogram2D::ToString()
{
	if( !CV_IS_HIST(h))
		ASSERT(false);

	int size[CV_MAX_DIM];
	int dims = cvGetDims( this->h->bins, size );
	int total = 1;
	for(int i = 0; i < dims; i++ )
		total *= size[i];

	System::String ^s;
	float *ptr = 0;
	cvGetRawData( this->h->bins, (uchar**)&ptr);
	for(int i=0; i<a_bins;i++ )
	{
		for(int j=0; j<b_bins;j++ )
		{
			float bin_val = cvQueryHistValue_2D( h, i, j );
			s += System::String::Format("{0:f4}, ", bin_val);
		}
		s += "\n";
	}
	return s;
}
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------