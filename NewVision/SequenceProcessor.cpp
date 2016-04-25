#include "StdAfx.h"
#include "SequenceProcessor.h"
#include "Filters.h"
#include "Globals.h"
#include "NewVision.h"
#include "MainFrm.h"
#include "NewVisionView.h"
//#include "../PGRSourceFilter/PGRSourceFilter.h"
extern CLSID CLSID_PGRSourceFilter;
CLSID CLSID_PGRSourceFilter = {0x92e9bf2a, 0x4385, 0x4a08, {0x82, 0x36, 0x39, 0x74, 0x78, 0xeb, 0x34, 0xb8} };

#using <System.dll>
#using <mscorlib.dll>
using namespace System;
using namespace System::IO;
using namespace System::Diagnostics;

double TIME_EPSILON = 1e-7;

//----------------------------------------------------------------------------
STDMETHODIMP ThermalGrabber::BufferCB(double SampleTime, BYTE * pData, long lBufferSize ) {
	CSingleLock lock(&thermalTrackerReady);
	lock.Lock();
	m_frameNumber = round(sp->m_frameRate*SampleTime);
	//echo(String::Format("T frame = {0}", m_frameNumber));
	VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) sp->m_mediaType.pbFormat;
	int cxImage    = pvi->bmiHeader.biWidth;
	int cyImage    = abs(pvi->bmiHeader.biHeight);
	int stride     = (cxImage * sizeof( RGBTRIPLE ) + 3) & -4;
	// Copy the data
	cvSetImageData( imgVideoThermal, pData, stride );
	cvCvtColor(imgVideoThermal, imgThermal, CV_RGB2GRAY);

	if (pvi->bmiHeader.biHeight > 0)
		cvFlip(imgThermal);
	thermalFrameReady.SetEvent();
	return S_OK;	
}
//----------------------------------------------------------------------------
STDMETHODIMP ColorGrabber::BufferCB(double SampleTime, BYTE * pData, long lBufferSize ) {
	CSingleLock lock(&colorTrackerReady); 
	lock.Lock();
	m_frameNumber = round(sp->m_frameRate*SampleTime);
	sp->m_frameNumber = m_frameNumber;
	VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) sp->m_mediaType.pbFormat;
	int cxImage    = pvi->bmiHeader.biWidth;
	int cyImage    = abs(pvi->bmiHeader.biHeight);
	int stride     = (cxImage * sizeof( RGBTRIPLE ) + 3) & -4;
	// Copy the data
	cvSetImageData( imgVideo, pData, stride );
	if (sp->m_mode == PGRFILE)
		cvMirror(imgVideo, NULL, 1);
	cvCopy(imgVideo, imgFrame);
	
	if (pvi->bmiHeader.biHeight > 0)
		cvFlip(imgFrame);
	
	colorFrameReady.SetEvent();
	return S_OK;
}
//----------------------------------------------------------------------------
SequenceProcessor::SequenceProcessor() {
	m_isSettingPositions = 0;
	pGraph			= NULL;
	pVmr			= NULL;
	pWC				= NULL;
	pME				= NULL;
	pMSColor				= NULL;
	pMSthermal		= NULL;
	pMC				= NULL;
	pMixerImage		= NULL;
	pConfig			= NULL;
	bmi				= NULL;
}
//----------------------------------------------------------------------------
SequenceProcessor::~SequenceProcessor() {
}
//----------------------------------------------------------------------------
void SequenceProcessor::DeInitialize() {
	m_sourcename = ""; 
	m_screenname = NULL;
	m_frameStart = 0;
	m_frameStop = 0;
	//ISampleGrabber *pGrabber;
	//pGrabberF->QueryInterface(IID_ISampleGrabber, (void**)&pGrabber);
	//pGrabber->SetCallback(NULL,0);
	m_compatibleDCvideo.Detach();
	m_compatibleDCfloor.Detach();
	if (bmi)
		delete[] bmi;
	Clear();
	SafeRelease(pMC);
	SafeRelease(pMSColor);
	SafeRelease(pWC);
	SafeRelease(pME);
	SafeRelease(pMixerImage);				
	SafeRelease(pVmr);
	SafeRelease(pGrabberF);
	if (m_mode == TRGBFILE) {
		SafeRelease(pMSthermal);
		SafeRelease(pGrabberF);
		delete thermalGrabber;
	}
	SafeRelease(pConfig);
	SafeRelease(pGraph);
	}
//----------------------------------------------------------------------------
bool SequenceProcessor::Initialize() {
	USES_CONVERSION;
	CNewVisionView *activeView = ((CMainFrame*)(theApp.m_pMainWnd))->GetActivePane();
	m_screenname = activeView->m_hWnd;

	if (FAILED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&pGraph))) {
        ::MessageBox( NULL, "Failed to create DirectX filter graph", "Error", MB_OK | MB_ICONINFORMATION );		
		  return false;
	}

	// create the VMR9 filter
	HRESULT res = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC,
	IID_IBaseFilter, (void**)&pVmr);

	// add the VMR9 filter to the Graph Manager
	pGraph->AddFilter(pVmr, L"Video");

	// get a pointer to the IVMRFilterConfig9 interface 
	pVmr->QueryInterface(IID_IVMRFilterConfig9, (void**)&pConfig);

	// make sure VMR9 is in windowless mode
	pConfig->SetRenderingMode(VMR9Mode_Windowless);

	// get a pointer to the IVMRWindowlessControl9 interface 
	pVmr->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pWC);

	// IMediaEventEx tells that an event has occurred (e.g. stopped paused)
	pGraph->QueryInterface(IID_IMediaEventEx, (void **)&pME);

	// Set video drawing and notification window
	SetVideoWindow(m_screenname);

	// Run source specific initialization
	switch(m_mode)
	{
		case PGRFILE: { 
			if (!InitPGR()) return false; break; 
		}
		case TRGBFILE: { 
			if (!InitThermal()) return false;
			if (!InitColor()) return false; break; 
		}
		case RGBFILE: { 
			if (!InitColor()) return false; break; 
		}
		case CAMERA: {}
	}

	// Turn off DirectShow clock
	pGraph->QueryInterface(IID_IMediaFilter, (void**)&pMediaFilter);
	pMediaFilter->SetSyncSource(NULL);

	// Set destination rectangle for the video
    pWC->GetNativeVideoSize(&cx, &cy, NULL, NULL);
	SetVideoPosition(new CRect(0,0,cx,cy), true);
	// Get a pointer to the IVMRMixerControl9 interface 
	pVmr->QueryInterface(IID_IVMRMixerControl9, (void**)&pMix);
	
	// Set alpha levels
	//	pMix->SetAlpha(0, 0.5); pMix->SetAlpha(1, 1);

	// Create a bitmap to display overlay information
	m_compatibleDCvideo.CreateCompatibleDC(activeView->GetDC());
//	BYTE        tmp[sizeof(BITMAPINFO)+255*4];
	bmi = (BITMAPINFO*)(new char[sizeof(BITMAPINFO)+255*4]);
	memset(bmi,0,sizeof(BITMAPINFO));
	bmi->bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biWidth    = cx;
	bmi->bmiHeader.biHeight   = -cy;
	bmi->bmiHeader.biPlanes   = 1;
	bmi->bmiHeader.biBitCount = 24;
	bmi->bmiHeader.biCompression = BI_RGB;
	bmi->bmiHeader.biSizeImage  = cx*cy*1;
	bmi->bmiHeader.biClrImportant  = 0;
	((DWORD*) bmi->bmiColors)[0] = 0x00FF0000; /* red mask  */
	((DWORD*) bmi->bmiColors)[1] = 0x0000FF00; /* green mask */
	((DWORD*) bmi->bmiColors)[2] = 0x000000FF; /* blue mask  */
	m_bitmap = ::CreateDIBSection(m_compatibleDCvideo.m_hDC, bmi, DIB_RGB_COLORS,NULL,0,0);
	//m_bitmap = ::CreateDIBitmap(m_compatibleDCvideo.m_hDC, &bmi->bmiHeader, CBM_INIT, NULL, bmi, DIB_RGB_COLORS);

	// Create a bitmap to display floor information
	CNewVisionView *floorView = ((CMainFrame*)(theApp.m_pMainWnd))->GetPane(FLOOR_VIEW);
	m_compatibleDCfloor.CreateCompatibleDC(floorView->GetDC());
	BYTE        tmpF[sizeof(BITMAPINFO)+255*4];
	BITMAPINFO *bmiF = (BITMAPINFO*)tmpF;
	memset(bmiF,0,sizeof(BITMAPINFO));

	// TEMP: use w,h from FloorModel instead
	fcx = 1068, fcy = 866;
	//fcx = 1024, fcy = 1024;
	bmiF->bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
	bmiF->bmiHeader.biWidth    = fcx;
	bmiF->bmiHeader.biHeight   = fcy;
	bmiF->bmiHeader.biPlanes   = 1;
	bmiF->bmiHeader.biBitCount = 24;
	bmiF->bmiHeader.biCompression = BI_RGB;
	bmiF->bmiHeader.biSizeImage  = fcx*fcy*1;
	bmiF->bmiHeader.biClrImportant  = 0;
	((DWORD*) bmiF->bmiColors)[0] = 0x00FF0000; /* red mask  */
	((DWORD*) bmiF->bmiColors)[1] = 0x0000FF00; /* green mask */
	((DWORD*) bmiF->bmiColors)[2] = 0x000000FF; /* blue mask  */
	m_floorBitmap = ::CreateDIBSection(m_compatibleDCfloor.m_hDC, bmiF, DIB_RGB_COLORS, NULL,0,0);

	// Initialize a bitmap to display overlay information
	pWC->QueryInterface(IID_IVMRMixerBitmap9, (LPVOID *)&pMixerImage);
	this->SetBitmapImage(m_bitmap, cx, cy);

	
	//VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) m_mediaType.pbFormat;
	//m_frameRate = UNITS/(double)pvi->AvgTimePerFrame;
	if (m_mode != PGRFILE) {
		CComPtr<IMediaDet> pDet;
		double duration = 0;
		pDet.CoCreateInstance(__uuidof(MediaDet));
		pDet->put_Filename(A2W(m_sourcename));
		pDet->get_FrameRate(&m_frameRate);
		pDet->get_StreamLength(&duration);
		m_frameTotal = round(duration*m_frameRate);
		pDet.Release();
	}
	else {
		pMSColor->GetDuration(&m_frameTotal);
		m_frameRate = 25;

	}
	// IMediaControl controls flow of data through the graph
	pGraph->QueryInterface(IID_IMediaControl, (void **)&pMC);

	//pMSColor->GetDuration(&m_frameTotal); 
	//pMSColor->GetStopPosition(&m_frameTotal); 
	//SaveGraphFile("F:\\My Documents\\My Downloads\\Filter Graph 04.grf");
	return true;
}

//---------------------------------------------------------------------------
bool SequenceProcessor::InitPGR() {
	USES_CONVERSION;
	// Create ISampleGrabber to grab frames
	if (FAILED(CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pGrabberF))) {
		::MessageBox( NULL, "Failed to create SampleGrabber filter!", "Error", MB_OK | MB_ICONINFORMATION );		
		return false;
	}
	ISampleGrabber *pGrabber;
	colorGrabber = new ColorGrabber(this);
	pGrabberF->QueryInterface(IID_ISampleGrabber, (void**)&pGrabber);
	ZeroMemory(&m_mediaType, sizeof(AM_MEDIA_TYPE));
	m_mediaType.majortype = MEDIATYPE_Video;
	//m_mediaType.subtype = MEDIASUBTYPE_RGB32;
	m_mediaType.subtype = MEDIASUBTYPE_RGB24;
	if (FAILED(pGrabber->SetMediaType(&m_mediaType))) {
		::MessageBox( NULL, "Media type is not True Color RGB!", "Error", MB_OK | MB_ICONINFORMATION );		
		return false;
	}
	pGrabber->SetBufferSamples( TRUE );
	pGrabber->SetOneShot( FALSE );
	pGrabber->SetCallback( colorGrabber, 1 );

	if (FAILED(pGraph->AddFilter(pGrabberF, L"Sample Grabber"))) {
		::MessageBox( NULL, "Failed to add SampleGrabber filter to the graph!", "Error", MB_OK | MB_ICONINFORMATION );		
		return false;
	}
	while (!File::Exists(gcnew String(m_sourcename)) || FAILED(pGraph->RenderFile(A2W(m_sourcename), NULL))) {
		AfxMessageBox("Video file: \""+m_sourcename+"\" appears corrupt or absent!\n Please select another file.", MB_ICONINFORMATION | MB_OK);
		char szFilters[] = "AVI Files (*.avi)|*.avi|MPEG Files (*.mpg)|*.mpg|PGR files (*.pgr)|*.pgr|All Files (*.*)|*.*||";
		CFileDialog fileDlg (TRUE, "avi", "*.avi", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, NULL);
		if ( fileDlg.DoModal () == IDOK )
		{
			m_sourcename = fileDlg.GetPathName();
		}
		else {
			return false;
		}
	}

	// Initialize stream seeking information
	IPin *pin1 = GetPin(pGrabberF, PINDIR_OUTPUT, 1);
	pin1->QueryInterface(IID_IMediaSeeking, (void **)&pMSColor);
	if (FAILED(pMSColor->SetTimeFormat(&TIME_FORMAT_FRAME))) {
		if (FAILED(pMSColor->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME))) {
			AfxMessageBox("Video filter doesn't support frame stepping mode!");
			return false;
		}
	}
	pGrabber->GetConnectedMediaType( &m_mediaType );
	return true;
}
//---------------------------------------------------------------------------
bool SequenceProcessor::InitColor() {
	USES_CONVERSION;
	// Create ISampleGrabber to grab frames
	if (FAILED(CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pGrabberF))) {
		::MessageBox( NULL, "Failed to create SampleGrabber filter!", "Error", MB_OK | MB_ICONINFORMATION );		
		return false;
	}
	ISampleGrabber *pGrabber;
	colorGrabber = new ColorGrabber(this);
	pGrabberF->QueryInterface(IID_ISampleGrabber, (void**)&pGrabber);
	ZeroMemory(&m_mediaType, sizeof(AM_MEDIA_TYPE));
	m_mediaType.majortype = MEDIATYPE_Video;
	m_mediaType.subtype = MEDIASUBTYPE_RGB24;
	if (FAILED(pGrabber->SetMediaType(&m_mediaType))) {
		::MessageBox( NULL, "Media type is not True Color RGB!", "Error", MB_OK | MB_ICONINFORMATION );		
		return false;
	}
	pGrabber->SetBufferSamples( TRUE );
	pGrabber->SetOneShot( FALSE );
	pGrabber->SetCallback(colorGrabber, 1 );
	if (FAILED(pGraph->AddFilter(pGrabberF, L"Sample Grabber"))) {
		::MessageBox( NULL, "Failed to add SampleGrabber filter to the graph!", "Error", MB_OK | MB_ICONINFORMATION );		
		return false;
	}

	// Render source file
	while (!File::Exists(gcnew String(m_sourcename)) || FAILED(pGraph->RenderFile(A2W(m_sourcename), NULL))) {
		AfxMessageBox("Video file: \""+m_sourcename+"\" appears corrupt or absent!\n Please select another file.", MB_ICONINFORMATION | MB_OK);
		char szFilters[] = "AVI Files (*.avi)|*.avi|MPEG Files (*.mpg)|*.mpg|All Files (*.*)|*.*||";
		CFileDialog fileDlg (TRUE, "avi", "*.avi", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, NULL);
		if ( fileDlg.DoModal () == IDOK )
		{
			m_sourcename = fileDlg.GetPathName();
		}
		else {
			return false;
		}

	}

	//SaveGraphFile("F:\\My Documents\\My Downloads\\Filter Graph 04.grf");

	// Initialize stream seeking information
	//pGraph->QueryInterface(IID_IMediaSeeking, (void **)&pMSColor);
	IPin *pin1 = GetPin(pGrabberF, PINDIR_OUTPUT, 1);
	pin1->QueryInterface(IID_IMediaSeeking, (void **)&pMSColor);
	if (FAILED(pMSColor->SetTimeFormat(&TIME_FORMAT_FRAME))) {
		if (FAILED(pMSColor->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME))) {
			AfxMessageBox("Video filter doesn't support frame stepping mode!");
			return false;
		}
	}
	pGrabber->GetConnectedMediaType( &m_mediaType );
	return true;
}
//---------------------------------------------------------------------------
bool SequenceProcessor::InitThermal() {
	USES_CONVERSION;

	// Render (thermal) file for color+thermal sequences

	//notify VMR9 of two inputs
	//pConfig->SetNumberOfStreams(2);

	// Create ISampleGrabber to grab frames
	if (FAILED(CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pGrabberFT))) {
		::MessageBox( NULL, "Failed to create SampleGrabber filter!", "Error", MB_OK | MB_ICONINFORMATION );		
		return false;
	}

	ISampleGrabber *pGrabberT;
	thermalGrabber = new ThermalGrabber(this);
	pGrabberFT->QueryInterface(IID_ISampleGrabber, (void**)&pGrabberT);
	AM_MEDIA_TYPE thermalMediaType;
	ZeroMemory(&thermalMediaType, sizeof(AM_MEDIA_TYPE));
	thermalMediaType.majortype = MEDIATYPE_Video;
	thermalMediaType.subtype = MEDIASUBTYPE_RGB24;
	if (FAILED(pGrabberT->SetMediaType(&thermalMediaType))) {
		::MessageBox( NULL, "Media type is not True Color RGB!", "Error", MB_OK | MB_ICONINFORMATION );		
		return false;
	}
	pGrabberT->SetBufferSamples( TRUE );
	pGrabberT->SetOneShot( FALSE );
	pGrabberT->SetCallback( thermalGrabber, 1 );

	if (FAILED(pGraph->AddFilter(pGrabberFT, L"Sample Grabber"))) {
		::MessageBox( NULL, "Failed to add SampleGrabber filter to the graph!", "Error", MB_OK | MB_ICONINFORMATION );		
		return false;
	}

	while (!File::Exists(gcnew String(m_sourcenameT)) || FAILED(pGraph->RenderFile(A2W(m_sourcenameT), NULL))) {
		AfxMessageBox("Thermal video file: \""+m_sourcenameT+"\" appears corrupt or absent!\n Please select another file.", MB_ICONINFORMATION | MB_OK);
		char szFilters[] = "AVI Files (*.avi)|*.avi|MPEG Files (*.mpg)|*.mpg|All Files (*.*)|*.*||";
		CFileDialog fileDlg (TRUE, "avi", "*.avi", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, NULL);
		if ( fileDlg.DoModal () == IDOK )
		{
			m_sourcenameT = fileDlg.GetPathName();
		}
		else {
			return false;
		}

	}

	IPin *pin1 = GetPin(pGrabberFT, PINDIR_OUTPUT, 1);
	pin1->QueryInterface(IID_IMediaSeeking, (void **)&pMSthermal);
	if (FAILED(pMSthermal->SetTimeFormat(&TIME_FORMAT_FRAME))) {
		if (FAILED(pMSthermal->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME))) {
			AfxMessageBox("Video filter for thermal stream doesn't support frame stepping or time stepping mode!");
			return false;
		}
	}
	return true;
}
//---------------------------------------------------------------------------
CDC* SequenceProcessor::GetOverlayDC() {
	return &m_compatibleDCfloor;
}
//---------------------------------------------------------------------------
void SequenceProcessor::IplImage2Overlay(IplImage* ipl, HBITMAP hBmp) {
	//if (FAILED(::SetBitmapBits(hBmp, ipl->imageSize, ipl->imageData)) || FAILED(pMixerImage->SetAlphaBitmap(&m_bmpInfo))) {
	if (FAILED(::SetDIBits(m_compatibleDCvideo.m_hDC, hBmp, 0, ipl->height, ipl->imageData, bmi, DIB_RGB_COLORS)) || FAILED(pMixerImage->SetAlphaBitmap(&m_bmpInfo))) {
		AfxMessageBox("Could not display overlay information", MB_ICONINFORMATION | MB_OK);
		Stop();
	}
}
//---------------------------------------------------------------------------
void SequenceProcessor::Overlay2IplImage(HBITMAP hBmp, IplImage* ipl) {
	if (FAILED(::GetBitmapBits(hBmp, ipl->imageSize, ipl->imageData))) {
		AfxMessageBox("Could not convert overlay to IplImage", MB_ICONINFORMATION | MB_OK);
		Stop();
	}
}
//---------------------------------------------------------------------------
/*
	Sets the VMR9AplhaBitmap from HBITMAP
*/
bool SequenceProcessor::SetBitmapImage(HBITMAP hBmp, LONG cx, LONG cy) {
	ZeroMemory(&m_bmpInfo, sizeof(m_bmpInfo) );    
	m_bmpInfo.dwFlags |= VMRBITMAP_HDC;// | VMRBITMAP_SRCRECT;
	m_compatibleDCfloor.SelectObject(hBmp);
	m_bmpInfo.hdc = m_compatibleDCfloor.m_hDC;
	m_bmpInfo.rSrc = CRect(0, 0, cx, cy);
	m_bmpInfo.rDest.left = 0; m_bmpInfo.rDest.right = 1.0;
	m_bmpInfo.rDest.top = 0; m_bmpInfo.rDest.bottom = 1.0;
    m_bmpInfo.fAlpha = 1;
	m_bmpInfo.dwFlags |= VMRBITMAP_SRCCOLORKEY; 
    m_bmpInfo.clrSrcKey = INVISIBLE_COLOR;
	if (FAILED(pMixerImage->SetAlphaBitmap(&m_bmpInfo))) {
		AfxMessageBox("Could not display overlay information", MB_ICONINFORMATION | MB_OK);
		return false;
	}
	else
		return true;
}
//---------------------------------------------------------------------------
bool SequenceProcessor::DisplaySeekingCapabilities() {
	DWORD dwCap = 0;
	HRESULT hr1 = pMSColor->GetCapabilities(&dwCap);
	(AM_SEEKING_CanSeekAbsolute & dwCap) ? AfxMessageBox("Can seek absolute"):1;
	(AM_SEEKING_CanSeekForwards & dwCap) ? AfxMessageBox("Can seek forwards"):1;
	(AM_SEEKING_CanSeekBackwards & dwCap) ? AfxMessageBox("Can seek backwards"):1;
	if (AM_SEEKING_CanSeekAbsolute & dwCap)
		return true;
	else 
		return false;
}
//----------------------------------------------------------------------------
void SequenceProcessor::SetVideoPosition(LPRECT dst, bool maintainAspectRatio) {
	if (FAILED(pWC->SetVideoPosition(NULL, dst)))
		AfxMessageBox("SetVideoPostition Failed!");
	if (maintainAspectRatio)	
		pWC->SetAspectRatioMode(VMR_ARMODE_LETTER_BOX);
	else
		pWC->SetAspectRatioMode(VMR_ARMODE_NONE);
	
}
//----------------------------------------------------------------------------
void SequenceProcessor::SetVideoPosition(LPRECT src, LPRECT dst, bool maintainAspectRatio) {
	pWC->SetVideoPosition(src, dst);
	if (maintainAspectRatio)	
		pWC->SetAspectRatioMode(VMR_ARMODE_LETTER_BOX);
	else
		pWC->SetAspectRatioMode(VMR_ARMODE_NONE);

}
//----------------------------------------------------------------------------
void SequenceProcessor::Stop() {
	pMC->Stop();
}
//----------------------------------------------------------------------------
void SequenceProcessor::StopWhenReady() {
	pMC->StopWhenReady();
}
//----------------------------------------------------------------------------
void SequenceProcessor::Run() {
	pMC->Run();
}
//----------------------------------------------------------------------------
void SequenceProcessor::Pause() {
	pMC->Pause();
}
//----------------------------------------------------------------------------
void SequenceProcessor::PushState() {
	GetState(DELAY, m_savedState);
}
//----------------------------------------------------------------------------
void SequenceProcessor::PopState() {
	SetState(DELAY, m_savedState);
}
//----------------------------------------------------------------------------
void SequenceProcessor::GetState(LONG msTimeout, OAFilterState& state) {
	pMC->GetState(msTimeout, &state);
}
//----------------------------------------------------------------------------
void SequenceProcessor::SetState(LONG msTimeout, OAFilterState& state) {
	if (state == State_Stopped)
        StopWhenReady();
    else if (state == State_Running)
        Run();
	else if (state == State_Paused)
		Pause();    
}
//----------------------------------------------------------------------------
void SequenceProcessor::SetRate(double rate) {
	//pMSColor->SetRate(rate);
}
//----------------------------------------------------------------------------
void SequenceProcessor::SetPosition(REFERENCE_TIME pos) {
	m_isSettingPositions = 1;
	if (pos == -1)
		pos = m_frameNumber;

	PushState();
	Pause();
	if (m_mode == TRGBFILE)
		HRESULT hr = pMSthermal->SetPositions(&pos, AM_SEEKING_AbsolutePositioning,
		NULL, AM_SEEKING_NoPositioning);

	HRESULT hr = pMSColor->SetPositions(&pos, AM_SEEKING_AbsolutePositioning,
			NULL, AM_SEEKING_NoPositioning);
	if (FAILED(hr))
		AfxMessageBox("SetPosition Failed in SequenceProcessor!");
	Pause();
	PopState();

}
//----------------------------------------------------------------------------
REFERENCE_TIME SequenceProcessor::GetCurrentPosition() {
	HRESULT hr;
	REFERENCE_TIME pNow;
	hr = pMSColor->GetCurrentPosition(&pNow);
    if (FAILED(hr))
        return -1;
	else 
		return pNow;
}
//----------------------------------------------------------------------------
bool SequenceProcessor::IsPaused() {
	OAFilterState state;
	if( pMC->GetState( DELAY, &state ) == S_OK &&  state == State_Paused )
		return true;
	return false;
}
//----------------------------------------------------------------------------
bool SequenceProcessor::IsRunning() {
	OAFilterState state;
	if( pMC->GetState( DELAY, &state) == S_OK &&  state == State_Running )
		return true;
	return false;
}
//----------------------------------------------------------------------------
bool SequenceProcessor::IsStopped() {
	OAFilterState state;
	if( pMC->GetState( DELAY, &state ) == S_OK &&  state == State_Stopped )
		return true;
	return false;
}
//----------------------------------------------------------------------------
void SequenceProcessor::Clear() {
	if (pGraph) {
		//// BUG: freezes here, see GRAPH NOTIFY in video manipulation sample project
		//Pause();
		//OAFilterState state;
		//pMC->GetState(DELAY, &state);
		//WaitForSingleObject(::Win16Mutex,INFINITE);
		pMC->Stop();
		//while (true) {
		//	if( pMC->GetState( 500, &state ) == S_OK &&  state == State_Stopped )
		//		break;
		//}
		removeAllFilters(pGraph);
	}
}
// --------------------------------------------------------------------------
HRESULT	SequenceProcessor::RepaintPosterFrame() {
	// notify the graph to repaint (this make it repaint even if it's not running)
	IMediaEventSink	*pMediaSink;
	pGraph->QueryInterface(IID_IMediaEventSink, (void**)&pMediaSink);
	pMediaSink->Notify(EC_REPAINT, NULL, NULL);
	return S_OK;
}
// --------------------------------------------------------------------------
HRESULT	SequenceProcessor::ShowPosterFrame(HWND  hwnd, HDC  hdc) {
	// display poster frame
	//echo(String::Format("repaint in {0}", m_frameNumber));
	return pWC->RepaintVideo(hwnd, hdc);
}
// --------------------------------------------------------------------------
void SequenceProcessor::SetTransparency(int mTransLevel) {
	// Set the transparency value (1.0 is opaque, 0.0 is transparent). mTransLevel is from 0 to 100
	m_bmpInfo.fAlpha = (float)mTransLevel/100;
	pMixerImage->UpdateAlphaBitmapParameters(&m_bmpInfo);
}
// --------------------------------------------------------------------------
void SequenceProcessor::SetVideoWindow(HWND  hwnd) {
	m_screenname = hwnd;
	pME->SetNotifyWindow ( ( OAHWND ) m_screenname, WM_GRAPHNOTIFY, 0 ) ;
	// specify the container window that the video should be clipped to
	pWC->SetVideoClippingWindow(m_screenname);
	//pWC->SetVideoClippingWindow(NULL);
}
// --------------------------------------------------------------------------
HRESULT SequenceProcessor::SaveGraphFile(CString filename) 
{
	USES_CONVERSION;
	WCHAR *wszPath = A2W(filename);
	const WCHAR wszStreamName[] = L"ActiveMovieGraph"; 
	HRESULT hr;

	IStorage *pStorage = NULL;
	hr = StgCreateDocfile(
		wszPath,
		STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
		0, &pStorage);
	if(FAILED(hr)) 
	{
		return hr;
	}

	IStream *pStream;
	hr = pStorage->CreateStream(
		wszStreamName,
		STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
		0, 0, &pStream);
	if (FAILED(hr)) 
	{
		pStorage->Release();    
		return hr;
	}

	IPersistStream *pPersist = NULL;
	pGraph->QueryInterface(IID_IPersistStream, (void**)&pPersist);
	hr = pPersist->Save(pStream, TRUE);
	pStream->Release();
	pPersist->Release();
	if (SUCCEEDED(hr)) 
	{
		hr = pStorage->Commit(STGC_DEFAULT);
	}
	pStorage->Release();
	return hr;
}