#if !defined BACKGROUNDMODEL_H
#define BACKGROUNDMODEL_H
#pragma once
#include <time.h>
#include <afxtempl.h>
#include "cv.h"      
#include "Image.h"
#include "CodeBook.h"
#include "Model.h"

using namespace OpenCVExtensions;
enum BackgroundModelType {STATIC=0, ADAPTIVE=1};
// implements background model using codebook approach
// 
class BackgroundModel : public CArray<CodeBook>, public Model {
public:
	int m_CDmax;				   // maximal color distortion 
	double m_BDmin, m_BDmax;	   // min and max intensity distortions 
	int m_ThermalMin, m_ThermalMax;	   // -max and +max imgThermal variations

	int m_mode;                    // one of two different modes:
	int m_initStage;			   // 1 - use explicit learning stage

	int m_minAge;                  // minimum age of a codeword to be successfully matched
	int m_trackAfterAge;           // 2 - learn "on the fly" and start tracking after m_trackAfterAgePercentage 
	int m_trackAfterAgePercentage; // of codewords have reached m_trackAfterAge
	
	int m_removeStaleFreq;         // (for option 2) remove stale periodically
	int m_staleCutoff;             // consider stale when haven't seen in sequence for longer than m_StaleCutoff
	int m_updateFreq;              // update model every N frames
	
	double m_sensitivity;			// general sensitivity for probabilistic subtraction (>=1)
	int m_Erode;
	int m_ErodeSize;
	int m_Dilate;
	int m_DilateSize;

	bool m_initialized;

	BackgroundModel(void);
	~BackgroundModel(void);

	bool Initialize();
	void DeInitialize();
	bool IsInitialized() { return m_initialized; };
	void Update(int nFrame=-1, IplImage *mask=NULL); 
	void SubtractBackground(IplImage* FGMap, int nFrame);
	void SubtractBackgroundProbabilistic(IplImage* FGMapDouble, int nFrame);
	void Draw(IplImage* frame);
	void DrawHeat(IplImage* srcFrame, IplImage* tgtFrame);
	void GetSizes(IplImage* result);
	int	 GetModelSizeInCodewords();
	void RemoveStale();
	virtual void Serialize( CArchive& ar );
};
#endif // !defined( BACKGROUNDMODEL_H )