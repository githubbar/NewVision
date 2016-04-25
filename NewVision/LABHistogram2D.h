#if !defined (_LABHistogram2D_)
#define _LABHistogram2D_
#pragma once

#include "cv.h"      
#include "highgui.h" 

class LABHistogram2D {
	public:
		static const int	   a_bins = 30;
		static const int	   b_bins = 30;
		static const int	   a_min = 0;
		static const int	   b_min = 0;
		static const int	   a_max = 180;
		static const int	   b_max = 255;

		CvHistogram* h;

		       LABHistogram2D(void);
		      ~LABHistogram2D(void);
			   LABHistogram2D(const LABHistogram2D& that);
		LABHistogram2D& operator=(const LABHistogram2D& that);

		System::String^ ToString();
		void   			Serialize( CArchive& ar );
		void   			Clear();
		double 			GetValue(double a, double b);
		double 			GetValue(CvScalar bgr);
		void   			AddValue(CvScalar bgr);
		void   			Draw(IplImage* frame, int scale=0, CvPoint p=cvPoint(0,0));
		void   			Draw(CDC* pDC);
		double 			Compare(const LABHistogram2D* that);
		double 			GetNorm();
};


#endif // !defined( _LABHistogram2D_ )