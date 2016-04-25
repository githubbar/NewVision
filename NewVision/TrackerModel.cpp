/*!
 * Main Visual Tracking Model file.
 * 
 * Copyright (c) 2005 by Alex Leykin
 */

#include "StdAfx.h"
#include "TrackerModel.h"
#include "vfw.h"
#include "NewVision.h"
#include "NewVisionDoc.h"
#include "NewVisionView.h"
#include "BodyModel.h"
#include "Body.h"
#include "BodyPath.h"
#include "MainFrm.h"
#include "Globals.h"
#include "Cylinder.h"
#using <mscorlib.dll>
#using <System.dll>
#include <dvdmedia.h>

using namespace System;
using namespace System::Diagnostics;
using namespace System::IO;
using namespace System::Threading;

IMPLEMENT_SERIAL(TrackerModel, Model, 1)
UINT TrackerCallback(LPVOID pParam ) {
	TrackerModel* pObject = (TrackerModel*)pParam;
	if (pObject == NULL ||
		!pObject->IsKindOf(RUNTIME_CLASS(TrackerModel)))
		return 1;   // if pObject is not valid
	else
		pObject->RunTracker();
	return 0;
}
// --------------------------------------------------------------------------
bool TrackerModel::Initialize() {
	if (m_initialized)
		return true;

	if (!SequenceProcessor::Initialize()) 
		return false;

	// Initialize Images 
	CvSize sz		= cvSize(cx, cy);	
	imgVideo		= cvCreateImageHeader(sz, IPL_DEPTH_8U, 3);	
	imgVideoThermal = cvCreateImageHeader(sz, IPL_DEPTH_8U, 3);	
	
	//// Half the size if in thermal+color mode
	//if (m_mode == TRGBFILE)
	//	sz.width /= 2;

	imgFrame		= cvCreateImage(sz, IPL_DEPTH_8U, 3);	
	// Set OpenCV image origin (TopLeft or BottomLeft)
	//((VIDEOINFOHEADER *) (m_mediaType.pbFormat))->bmiHeader.biHeight > 0 ? imgFrame->origin = 1 : imgFrame->origin = 0;
	imgThermal		= cvCreateImage(sz, IPL_DEPTH_8U, 1);	
	FGMap			= cvCreateImage(sz, IPL_DEPTH_8U, 1);
	dilated			= cvCreateImage(sz, IPL_DEPTH_8U, 1);	
	mask			= cvCreateImage(sz, IPL_DEPTH_8U, 1);
	weight			= cvCreateImage(sz, IPL_DEPTH_8U, 1);
	l_plane			= cvCreateImage(sz, IPL_DEPTH_8U, 1);
	a_plane			= cvCreateImage(sz, IPL_DEPTH_8U, 1);
	b_plane			= cvCreateImage(sz, IPL_DEPTH_8U, 1);
	fg				= cvCreateImage(sz, IPL_DEPTH_8U, 1);
	bg				= cvCreateImage(sz, IPL_DEPTH_8U, 1);
	zbuffer			= cvCreateImage(sz, IPL_DEPTH_8U, 1);
	body_zbuffer	= cvCreateImage(sz, IPL_DEPTH_8U, 1);
	temp_bg			= cvCreateImage(sz, IPL_DEPTH_8U, 1);
	temp			= cvCreateImage(sz, IPL_DEPTH_8U, 1);
	body_mask		= cvCreateImage(sz, IPL_DEPTH_8U, 1);
	blob_mask		= cvCreateImage(sz, IPL_DEPTH_8U, 1);	
	obstacle_disp	= cvCreateImage(sz, IPL_DEPTH_8U, 1);	
	door_mask		= cvCreateImage(sz, IPL_DEPTH_8U, 1);	
	distance_mask	= cvCreateImage(sz, IPL_DEPTH_32F, 1);	
	velx 			= cvCreateImage(sz, IPL_DEPTH_32F, 1);	
	vely 			= cvCreateImage(sz, IPL_DEPTH_32F, 1);	
	temp32			= cvCreateImage(sz, IPL_DEPTH_32F, 1);
	FGMapDouble		= cvCreateImage(sz, IPL_DEPTH_32F, 1);	
	ibg				= cvCreateImage(sz, IPL_DEPTH_32F, 1);
	ifg				= cvCreateImage(sz, IPL_DEPTH_32F, 1);

//	imgVideo		= cvCreateImage(sz, IPL_DEPTH_8U, 3);	
	imgOverlayInfo	= cvCreateImage(sz, IPL_DEPTH_8U, 3);	
	imgPreviousFrame= cvCreateImage(sz, IPL_DEPTH_8U, 3);	
	sceneImg		= cvCreateImage(sz, IPL_DEPTH_8U, 3);	
	activityImg 	= cvCreateImage(sz, IPL_DEPTH_8U, 3);	
	blobImg			= cvCreateImage(sz, IPL_DEPTH_8U, 3);	
	lab				= cvCreateImage(sz, IPL_DEPTH_8U, 3);
	biggy			= cvCreateImage(sz, IPL_DEPTH_32F, 3);

	// Initialize Supporting Structures
	__time64_t tmp_time;
	if (DEBUG_LEVEL > 0)
		rnd_seed = cvRNG(1);
	else
		rnd_seed = cvRNG(_time64(&tmp_time));
	OpenCVmemory = cvCreateMemStorage(0);
	aviWriterInputView		= NULL; 
	aviWriterOutputView		= NULL; 
	aviWriterActivityView	= NULL; 
	aviWriterFloorView		= NULL; 
	m_record				= false;
	m_initialized			= true;

	m_frameLastProcessed	= m_frameNumber = m_frameStart;
	//SetPosition();
	thermalTrackerReady.SetEvent();
	colorTrackerReady.SetEvent();
	m_pThread = AfxBeginThread(TrackerCallback, this);
	return true;
}
// --------------------------------------------------------------------------
void TrackerModel::DeInitialize() {
	if (!m_initialized)
		return;
	//CAutoLock dataLock( &dataCriticalSection); 
	SequenceProcessor::DeInitialize(); 
	// Release recording structures
	if (m_record) {
		CAutoLock singleLock(&recordCriticalSection);
		m_record = false;
		//cvReleaseImage(&rec);
	}

	// Release images and buffers
	cvReleaseMemStorage(&OpenCVmemory); 
	cvReleaseImageHeader(&imgVideo);
	cvReleaseImageHeader(&imgVideoThermal);
	cvReleaseImage(&imgFrame);
	cvReleaseImage(&blobImg);
	cvReleaseImage(&dilated);
	cvReleaseImage(&FGMap);
	cvReleaseImage(&FGMapDouble);
	cvReleaseImage(&imgOverlayInfo);
	cvReleaseImage(&imgThermal);
	cvReleaseImage(&sceneImg);
	cvReleaseImage(&activityImg);
	cvReleaseImage(&imgPreviousFrame);
	cvReleaseImage(&l_plane); cvReleaseImage(&a_plane); cvReleaseImage(&b_plane);
	cvReleaseImage(&mask); cvReleaseImage(&lab); cvReleaseImage(&weight); 
	cvReleaseImage(&fg);cvReleaseImage(&bg);cvReleaseImage(&zbuffer);cvReleaseImage(&body_zbuffer);
	cvReleaseImage(&temp);
	cvReleaseImage(&body_mask);	cvReleaseImage(&blob_mask);
	cvReleaseImage(&door_mask);
	cvReleaseImage(&distance_mask);
	cvReleaseImage(&obstacle_disp);
	cvReleaseImage(&velx);
	cvReleaseImage(&vely);
	cvReleaseImage(&temp32);
	m_initialized = false;
}
// --------------------------------------------------------------------------
void TrackerModel::ReleaseVideoWriters() {
	if (aviWriterInputView)
		cvReleaseVideoWriter(&aviWriterInputView);
	if (aviWriterOutputView)
		cvReleaseVideoWriter(&aviWriterOutputView);
	if (aviWriterActivityView)
		cvReleaseVideoWriter(&aviWriterActivityView);
	if (aviWriterFloorView)
		cvReleaseVideoWriter(&aviWriterFloorView);
	aviWriterInputView = NULL; aviWriterOutputView = NULL; aviWriterActivityView  = NULL; aviWriterFloorView  = NULL; 
}
// --------------------------------------------------------------------------
TrackerModel::TrackerModel() {
	m_initialized = false;
}
// --------------------------------------------------------------------------
TrackerModel::~TrackerModel(void)	{
}
// --------------------------------------------------------------------------
void TrackerModel::RunTracker() {
	while (m_initialized) {
		{ // +++ begin data processing critical section	
			if (m_mode == TRGBFILE) {
				CSyncObject* syncObjects[] = { &colorFrameReady, &thermalFrameReady};
				CMultiLock mlock( syncObjects, 2 );
				if (mlock.Lock() == -1)
					continue;
			}
			else {
				CSingleLock lock(&colorFrameReady);
				if (lock.Lock() == -1)
					continue;
			}

			CAutoLock dataLock( &dataCriticalSection); 
			cvClearMemStorage(OpenCVmemory);
			//echo(String::Format("track. frame = {0}", m_frameNumber));
			if (doc->m_initialized) switch (doc->m_ProcessingMode) {
				case TRACKING: {
					if (m_frameNumber < m_frameLastProcessed)
						theApp.m_pMainWnd->PostMessage(WM_LOADLASTFRAME);
					if (m_frameNumber > m_frameLastProcessed) {
						doc->bgmodel.Update((int)m_frameNumber);
						doc->bgmodel.SubtractBackground(FGMap, (int)m_frameNumber);
						doc->blobmodel.FindBlobs(imgFrame, FGMap);
						doc->bodymodel.Predict(); 
						doc->m_TrackingData.Put((int)m_frameNumber);
						doc->AutoSave();
					}
					break;
				}
				case HEATMAP: {
					if (m_frameNumber < m_frameLastProcessed)
						theApp.m_pMainWnd->PostMessage(WM_LOADLASTFRAME);
					else if (m_frameNumber > m_frameLastProcessed) {
						doc->bgmodel.Update((int)m_frameNumber);
						doc->bgmodel.SubtractBackground(FGMap, (int)m_frameNumber);
						//// BEGIN TEMP
						//cvThreshold(FGMap, fg, 254, 1, CV_THRESH_TOZERO_INV);
						//double T_LOOSE = 0.2;
						//cvThreshold(fg, fg, T_LOOSE*255, 1, CV_THRESH_BINARY);
						//doc->blobmodel.FindBlobs(imgFrame, fg, T_LOOSE);
						//cvThreshold(FGMap, fg, 254, 1, CV_THRESH_BINARY);
						//doc->blobmodel.FindBlobs(imgFrame, fg, 1);
						//// END TEMP
						doc->blobmodel.FindBlobs(imgFrame, FGMap);
						doc->blobmodel.UpdateHeatMap(motionmap);
						doc->m_HeatmapData.Put((int)m_frameNumber);
					}
					break;
				}
				case ACTIVITY: {
					doc->m_TrackingData.Get((int)m_frameNumber);
					doc->bodyactivitymodel.DetectEvents((int)m_frameNumber);
					doc->swarmactivitymodel.DetectEvents((int)m_frameNumber);
					doc->m_ActivityData.Put((int)m_frameNumber);
					break;
				}
				case PLAYBACK: {
					doc->m_TrackingData.Get((int)m_frameNumber);
					doc->bgmodel.Update((int)m_frameNumber);
					doc->bgmodel.SubtractBackground(FGMap, (int)m_frameNumber);
					doc->blobmodel.FindBlobs(imgFrame, FGMap);
					//doc->bodymodel.PredictOrientation(); 
					//doc->bodymodel.RefreshOpticalFlow(doc->bodymodel.body);
					break;
				}
			};
		} // +++ end data processing critical section		
		// --------------- Display information -----------------------------
		doc->UpdateOverlayInfo();
		if (theApp.m_pMainWnd) {
			theApp.m_pMainWnd->PostMessage(WM_UPDATENAVBAR);
			theApp.m_pMainWnd->PostMessage(WM_UPDATESTATUSBAR);
		}
		// --------------- Record results ----------------------------------
		RecordFrame();
		// --------------- Archive and Cleanup -----------------------------
		if (doc->m_initialized && doc->m_ProcessingMode != PLAYBACK)
			cvCopy(imgFrame, imgPreviousFrame);
		m_frameLastProcessed = max(m_frameLastProcessed, m_frameNumber);
		//echo(String::Format("tracker = {0}", m_frameNumber));
		//echo("=======================");
		thermalTrackerReady.SetEvent();
		colorTrackerReady.SetEvent();
	}
}
// --------------------------------------------------------------------------
void TrackerModel::RecordFrame() {
	// need a mutex here to block access to m_record from outside
	CAutoLock singleLock(&recordCriticalSection);
	if (m_record) {
		//int old_panel_type = ((CMainFrame*)(theApp.m_pMainWnd))->GetActivePane()->m_type;
		if (m_recordVideoView) {
			//((CMainFrame*)(theApp.m_pMainWnd))->GetActivePane()->m_type = VIDEO_VIEW;
			////m_compatibleDCvideo.getGetCurrentBitmap()->m_hObject
			//doc->UpdateOverlayInfo();
			//Overlay2IplImage(m_bitmap, imgOverlayInfo);
			cvWriteFrame(aviWriterInputView, imgOverlayInfo/*imgFrame*/);
		}
		if (m_recordSceneView)
			cvWriteFrame(aviWriterOutputView, sceneImg);
		if (m_recordActivityView)
			cvWriteFrame(aviWriterActivityView, activityImg);
		if (m_recordFloorView)
			cvWriteFrame(aviWriterFloorView, floorImg);
		//((CMainFrame*)(theApp.m_pMainWnd))->GetActivePane()->m_type = old_panel_type;
	}
}
// --------------------------------------------------------------------------
void TrackerModel::ShowTimeStamp(IplImage* frame) {
	CTimeSpan elapsed((int)(m_frameNumber / m_frameRate));
	CTime time = m_startTime + elapsed;
	drawTextInRectangle(
		time.Format( "%a, %m/%d/%y, %I:%M:%S%p"), 
		cvPoint(doc->cameramodel.w-260, doc->cameramodel.h-20), 
		frame, CV_RGB(255,255,255), CV_RGB(0,0,0), (float)1.1);
}
// --------------------------------------------------------------------------
DateTime TrackerModel::GetDateTime(int nFrame) {
	DateTime time(m_startTime.GetYear(), m_startTime.GetMonth(), m_startTime.GetDay(), m_startTime.GetHour(), m_startTime.GetMinute(), m_startTime.GetSecond());
	time = time.AddSeconds(1.0*nFrame/m_frameRate);
	return time;
}
// --------------------------------------------------------------------------
TimeSpan TrackerModel::GetTimeSpan(int nFrames) {
	TimeSpan span(0, 0, (int)(nFrames/m_frameRate));
	return span;
}
// --------------------------------------------------------------------------
int  TrackerModel::Seconds2Frames(double sec) {
	return round(sec*m_frameRate);
};
// --------------------------------------------------------------------------
double TrackerModel::Frames2Seconds(int f) {
	return 1.0*f/m_frameRate;
};