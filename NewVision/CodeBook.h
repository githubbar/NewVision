#if !defined( CODEBOOK_H )
#define CODEBOOK_H 
#pragma once
#include "CodeWordColor.h"
#include "CodeWordThermal.h"

class CodeBook {

public:
	CArray<CodeWordColor> color;
	CArray<CodeWordThermal> imgThermal;

	CodeBook(void);
	double MatchColorProbabilistic(Pixel* p, int frame, double t_cd, double alpha, double beta, int minAge=0);
	bool MatchColor(Pixel* p, int frame, double t_cd, double alpha, double beta, int minAge=0);
	bool MatchThermalByFactor(int p, int frame, double alpha, double beta, int minAge=0);
	bool MatchThermalByDeviation(int p, int frame, int m_ThermalMin, int m_ThermalMax, int minAge=0);
	double MatchThermalByDeviationProbabilistic(int p, int frame, int m_ThermalMin, int m_ThermalMax, int minAge=0);
	void UpdateColor(Pixel* p, int frame, double t_cd, double alpha, double beta);
	void UpdateThermal(int p, int frame, int m_ThermalMin, int m_ThermalMax);
	int CountOlderThan(int a);
	void GetRGB(Pixel* p);
	void RemoveStale(int cutoff);
};

template <> void AFXAPI SerializeElements <CodeBook> ( CArchive& ar, CodeBook* pData, INT_PTR nCount );

#endif // !defined( CODEBOOK_H )