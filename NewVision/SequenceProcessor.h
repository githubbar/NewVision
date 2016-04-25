#if !defined( SEQUENCEPROCESSOR_H )
#define SEQUENCEPROCESSOR_H

#pragma once
#include "cv.h"      
#include "samplegrabber.h"
//#include "LadybugStreamReader.h"
//#include "ladybuggeom.h"
//#include "ladybugopengl.h"

#define INVISIBLE_COLOR RGB(0, 0, 1)
#define CV_INVISIBLE_COLOR CV_RGB(0, 0, 1)
#define DELAY INFINITE

class Source;
class ThermalGrabber;
class ColorGrabber;
enum TrackerMode {RGBFILE=0, CAMERA=1, TRGBFILE=2, PGRFILE=3};
// ==============================================================================
class SequenceProcessor {
	
public:
		int PIN_COLOR, PIN_THERMAL;
		ThermalGrabber			*thermalGrabber;
		ColorGrabber			*colorGrabber; 
		IGraphBuilder			*pGraph;
		IBaseFilter				*pVmr;
		IBaseFilter				*pGrabberF;
		IBaseFilter				*pGrabberFT;
		IVMRWindowlessControl9  *pWC;
		IVMRMixerControl9		*pMix;
		IVMRFilterConfig9       *pConfig;
		IVMRMixerBitmap9        *pMixerImage;
		VMR9AlphaBitmap			m_bmpInfo;
		CRect					m_rect;
		VMR9NormalizedRect      m_desRect;
		CDC						m_compatibleDCvideo;
		CDC						m_compatibleDCfloor;
		

		IMediaControl			*pMC;
		IMediaEventEx		    *pME;
		IMediaSeeking			*pMSthermal, *pMSColor;
		IMediaFilter			*pMediaFilter;
		SequenceProcessor();
		~SequenceProcessor();
		// Persistent variables 
		bool			m_initialized;
		int				m_mode;	
		CString			m_sourcename; 			// input source name (file or camera) 
		CString			m_sourcenameT;			// thermal input source name (file or camera) 
		HWND			m_screenname; 			// output window handle
		int				m_frameStart; 			// start from frame ##
		int				m_frameStop;  			// stop from frame ##
		bool			m_record;     			// reflects recording state
		int				m_isSettingPositions; 	// is a 3-state bool 
											  	// 0=normal run; 
											  	// 1=SetPositions initiated callback for the first time; 
											  	// 2=SetPositions initiated callback for the second time
		double			m_frameRate;       		// frames per second
		REFERENCE_TIME	m_frameNumber;     		// current frame number
		REFERENCE_TIME	m_frameTotal;      		// total frames in the sequence
		int				m_frameLastProcessed;
		CTime			m_startTime;
		AM_MEDIA_TYPE	m_mediaType;
		OAFilterState	m_savedState;
		BITMAPINFO		*bmi;
		HBITMAP 		m_bitmap;
		HBITMAP 		m_floorBitmap;
		HANDLE			m_hLogFile;				// DirectX debug log file
		

		LONG			cx, cy;
		LONG			fcx, fcy;

		bool			Initialize();
		bool			InitPGR();
		bool			InitThermal();
		bool			InitColor();

		void			DeInitialize();
		void			Pause();
		void			Stop();
		void			StopWhenReady();
		void			Run();
		void			PushState();
		void			PopState();
		void			GetState(LONG msTimeout, OAFilterState& state);
		void			SetState(LONG msTimeout, OAFilterState& state);
		void			SetPosition(REFERENCE_TIME pos=-1);
		void			SetRate(double rate);
		REFERENCE_TIME	GetCurrentPosition();
		bool			IsPaused();
		bool			IsRunning();
		bool			IsStopped();
		bool			DisplaySeekingCapabilities();	
		void			Clear();
		CDC*			GetOverlayDC();
		void			IplImage2Overlay(IplImage* ipl, HBITMAP hBmp);
		void			Overlay2IplImage(HBITMAP hBmp, IplImage* ipl);
		bool			SetBitmapImage(HBITMAP hBmp, LONG cx, LONG cy);
		HRESULT			RepaintPosterFrame();
		HRESULT			ShowPosterFrame(HWND  hwnd, HDC  hdc);
		void			SetVideoWindow(HWND  hwnd);
		void			SetVideoPosition(LPRECT dst, bool maintainAspectRatio);
		void			SetVideoPosition(LPRECT src, LPRECT dst, bool maintainAspectRatio);
		void			SetTransparency(int mTransLevel);
		HRESULT			SaveGraphFile(CString filename);
};

// ==============================================================================
class ThermalGrabber: public ISampleGrabberCB {
public:
	REFERENCE_TIME	m_frameNumber;     // current frame number
	SequenceProcessor* sp;

	ThermalGrabber(SequenceProcessor* sp) {
		this->sp = sp;
	}
	// ----------- ISampleGrabberCB overrides --------------
	STDMETHODIMP_(ULONG) AddRef() { return 2; }
	STDMETHODIMP_(ULONG) Release() { return 1; }
	STDMETHOD( QueryInterface )( REFIID iid, LPVOID *ppv ) {
		if( iid == IID_ISampleGrabberCB || iid == IID_IUnknown ) { 
			*ppv = (void *) static_cast<ISampleGrabberCB*>( this );
			return NOERROR;
		} 
		return E_NOINTERFACE;
	}
	virtual STDMETHODIMP BufferCB(double SampleTime, BYTE * pData, long lBufferSize );
	virtual STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample) { return E_NOTIMPL; }
};
// ==============================================================================
class ColorGrabber: public ISampleGrabberCB {
public:
	REFERENCE_TIME	m_frameNumber;     // current frame number
	SequenceProcessor* sp;

	ColorGrabber(SequenceProcessor* sp) {
		this->sp = sp;
	}

	// ----------- ISampleGrabberCB overrides --------------
	STDMETHODIMP_(ULONG) AddRef() { return 2; }
	STDMETHODIMP_(ULONG) Release() { return 1; }
	STDMETHOD( QueryInterface )( REFIID iid, LPVOID *ppv ) {
		if( iid == IID_ISampleGrabberCB || iid == IID_IUnknown ) { 
			*ppv = (void *) static_cast<ISampleGrabberCB*>( this );
			return NOERROR;
		} 
		return E_NOINTERFACE;
	}
	virtual STDMETHODIMP BufferCB(double dblSampleTime, BYTE * pBuffer, long lBufferSize );
	virtual STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample) { return E_NOTIMPL; }

}; 
#endif // !defined( SEQUENCEPROCESSOR_H )

