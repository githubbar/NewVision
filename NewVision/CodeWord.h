#if !defined( CODEWORD_H )
#define CODEWORD_H

#pragma once
#include "cv.h"      
#include "Image.h"
#include <time.h>
#include <afxtempl.h>

using namespace OpenCVExtensions;

class CodeWord : public CObject {
	DECLARE_SERIAL( CodeWord )
public:
	unsigned char R, G, B;
	int i_max, i_min; // maximum and minimun pixel intensity
	int age;          // number of frames this codeword has occurred in 
	int stale;        // number of consecutive frames this codeword has not occurred for
	int first, last;  // first and last frame numbers this codeword was seen in

	CodeWord(void);
	CodeWord(Pixel* p, int frame);
	CodeWord(const CodeWord& c);
	CodeWord& operator=(const CodeWord &c);
	bool Match(Pixel* p, int frame, double t_cd, double alpha, double beta, int minAge=0);
	void Update(Pixel* p, int frame);
	void Serialize( CArchive& archive );
};

template <> void AFXAPI SerializeElements <CodeWord> ( CArchive& ar, CodeWord* pData, INT_PTR nCount );
#endif // !defined( CODEWORD_H )