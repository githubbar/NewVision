#if !defined( TRACKERMODEL_H )
#define TRACKERMODEL_H 

#pragma once
#include "cv.h"      
#include "highgui.h" 
#include "afxtempl.h"
#include "afxmt.h"
#include "math.h"
#include "time.h"
#include "Model.h"
#include "SequenceProcessor.h"
#include "SmartPtrArray.h"
#include "PersistantFormats.h"

#define SafeRelease(p) { if( (p) != 0 ) { (p)->Release(); (p)= 0; } }
#define CAP_FIRST_FRAME "CaptureFirstFrameEvent"
#define CAP_FRAME "CaptureFrameEvent"

// "TrackerModel" is a thread
enum RecordMode {LEFT=0, LEFTRIGHT=1, FLOOR=2};
class BodyPath;

class TrackerModel : public Model, public SequenceProcessor {
	DECLARE_SERIAL( TrackerModel )
public:
	// -------------- Methods -----------------------------------------------------------
	TrackerModel(); 
	~TrackerModel();

	bool	Initialize();
	void	DeInitialize();
	bool	IsInitialized() {return m_initialized;};
	void	ReleaseVideoWriters();
	void	RunTracker();

	void	NextFrame();
	void 	RecordFrame();
	void 	ShowTimeStamp(IplImage* frame);
	int  	Seconds2Frames(double sec);
	double	Frames2Seconds(int f);

	System::DateTime GetDateTime(int nFrame);
	System::TimeSpan GetTimeSpan(int nFrame);
private:
	CWinThread *m_pThread;
};

#endif // !defined( TRACKERMODEL_H )