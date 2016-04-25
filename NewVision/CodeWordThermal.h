#if !defined( CodeWordThermal_H )
#define CodeWordThermal_H

#pragma once
#include "cv.h"      
#include "Image.h"
#include <time.h>
#include <afxtempl.h>

using namespace OpenCVExtensions;

class CodeWordThermal : public CObject {
	DECLARE_SERIAL( CodeWordThermal )
public:
	int i_max, i_min; // maximum and minimun imgThermal pixel intensity
	int age;          // number of frames this CodeWordThermal has occurred in 
	int stale;        // number of consecutive frames this CodeWordThermal has not occurred for
	int first, last;  // first and last frame numbers this CodeWordThermal was seen in

			CodeWordThermal(void);
			CodeWordThermal(int p, int frame);
			CodeWordThermal(const CodeWordThermal& c);
			CodeWordThermal& operator=(const CodeWordThermal &c);
	bool	MatchByFactor(int p, int frame, double alpha, double beta, int minAge=0);
	bool	MatchByDeviation(int p, int frame, int m_ThermalMin, int m_ThermalMax, int minAge=0);
	double	MatchProbabilistic(int p, int frame, int m_ThermalMin, int m_ThermalMax, int minAge=0);
	void	Update(int p, int frame);
	void	Serialize( CArchive& archive );
};

template <> void AFXAPI SerializeElements <CodeWordThermal> ( CArchive& ar, CodeWordThermal* pData, INT_PTR nCount );
#endif // !defined( CodeWordThermal_H )