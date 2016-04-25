#if !defined( CodeWordColor_H )
#define CodeWordColor_H

#pragma once
#include "cv.h"      
#include "Image.h"
#include <time.h>
#include <afxtempl.h>

#define MAX_COLOR_DISTORTION 441.67

using namespace OpenCVExtensions;

class CodeWordColor : public CObject {
	DECLARE_SERIAL( CodeWordColor )
public:
	unsigned char R, G, B;
	int i_max, i_min; // maximum and minimun pixel intensity
	int age;          // number of frames this CodeWordColor has occurred in 
	int stale;        // number of consecutive frames this CodeWordColor has not occurred for
	int first, last;  // first and last frame numbers this CodeWordColor was seen in

			CodeWordColor(void);
			CodeWordColor(Pixel* p, int frame);
			CodeWordColor(const CodeWordColor& c);
			CodeWordColor& operator=(const CodeWordColor &c);
	double	MatchProbabilistic(Pixel* p, int frame, double t_cd, double alpha, double beta, int minAge=0);
	bool	Match(Pixel* p, int frame, double t_cd, double alpha, double beta, int minAge=0);
	void	Update(Pixel* p, int frame);
	void	Serialize( CArchive& archive );
};

template <> void AFXAPI SerializeElements <CodeWordColor> ( CArchive& ar, CodeWordColor* pData, INT_PTR nCount );
#endif // !defined( CodeWordColor_H )