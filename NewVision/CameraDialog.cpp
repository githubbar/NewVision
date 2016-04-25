// CameraDialog.cpp : implementation file
#include "stdafx.h"
#include "NewVisionDoc.h"
#include "CameraDialog.h"
#include "Globals.h"
#include "CameraDialog.h"
#include <iostream>
#include <sstream>
#include <fstream>

#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::Diagnostics;

// CameraDialog dialog

BEGIN_MESSAGE_MAP(CameraDialog, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTONRESET, OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTONADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTONDELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTONDELETEALL, OnBnClickedButtonDeleteAll)
	ON_WM_MOUSEMOVE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_CAMERA_MONO, OnBnClickedCameraMono)
	ON_BN_CLICKED(IDC_CAMERA_PANO, OnBnClickedCameraPano)
	ON_BN_CLICKED(IDC_CAMERA_XML_FILE, OnBnClickedXMLFile)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_XML_FILE, &CameraDialog::OnBnClickedButtonBrowseXmlFile)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CCalibrateControl, CButton)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()
// --------------------------------------------------------------------------
CameraDialog::CameraDialog(CWnd* pParent /* = NULL */)
	: CPropertyPage(CameraDialog::IDD), m_image()
{
}
// --------------------------------------------------------------------------
CameraDialog::~CameraDialog()
{
}
// --------------------------------------------------------------------------
void CameraDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE, m_image);
	DDX_Control(pDX, IDC_HEIGHTS, m_heights);
	DDX_Control(pDX, IDC_CURRENTHEIGHT, m_currentHeight);
	DDX_Control(pDX, IDC_CAMERA_SCROLLBAR1, m_vbar);
	DDX_Control(pDX, IDC_CAMERA_SCROLLBAR2, m_hbar);
	DDX_Text(pDX, IDC_ELEVATION, m_elevation);
	DDX_Radio(pDX, IDC_CAMERA_MONO, m_cameraType);
	DDX_Text(pDX, IDC_EDIT_XML_FILE, m_sourcename);
	UpdateControls();
	Initialize();
}
// --------------------------------------------------------------------------
void CameraDialog::UpdateControls() {
	switch (m_cameraType) {
		case PROJECTION:
			GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC4)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_ELEVATION)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC1)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTONADD)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTONDELETE)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_IMAGE)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_HEIGHTS)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_CURRENTHEIGHT)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_CAMERA_SCROLLBAR1)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_CAMERA_SCROLLBAR2)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTONRESET)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTONDELETEALL)->ShowWindow(SW_SHOW);				
			GetDlgItem(IDC_EDIT_XML_FILE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_BROWSE_XML_FILE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_XML_FILE)->ShowWindow(SW_HIDE);
			break;
		case SPHERICAL:
			GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTONADD)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTONDELETE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_IMAGE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_HEIGHTS)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_CURRENTHEIGHT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_CAMERA_SCROLLBAR1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_CAMERA_SCROLLBAR2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC3)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATIC4)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_ELEVATION)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTONRESET)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTONDELETEALL)->ShowWindow(SW_HIDE);	
			GetDlgItem(IDC_EDIT_XML_FILE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_BROWSE_XML_FILE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_XML_FILE)->ShowWindow(SW_HIDE);
			break;
		case XMLFILE:
			GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTONADD)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTONDELETE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_IMAGE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_HEIGHTS)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_CURRENTHEIGHT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_CAMERA_SCROLLBAR1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_CAMERA_SCROLLBAR2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC4)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_ELEVATION)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTONRESET)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTONDELETEALL)->ShowWindow(SW_HIDE);			
			GetDlgItem(IDC_EDIT_XML_FILE)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_BROWSE_XML_FILE)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATIC_XML_FILE)->ShowWindow(SW_SHOW);
	};

}
// --------------------------------------------------------------------------
void CameraDialog::OnBnClickedCameraMono()
{
	UpdateData();
}
// --------------------------------------------------------------------------
void CameraDialog::OnBnClickedCameraPano()
{
	UpdateData();
}
// --------------------------------------------------------------------------
void CameraDialog::OnBnClickedXMLFile()
{
	UpdateData();
}
// --------------------------------------------------------------------------
void CameraDialog::Serialize( CArchive& ar )
 {
	CameraModel::Serialize(ar);
}
// --------------------------------------------------------------------------
BOOL CameraDialog::OnSetActive() {
	CPropertyPage::OnSetActive();
	m_heights.UpdateData(false);
	switch (m_cameraType) {
		case PROJECTION: return m_image.Initialize(doc); 
		case SPHERICAL: return true;
		case XMLFILE: return true;
	}
	return true;
}
// --------------------------------------------------------------------------
bool CameraDialog::Initialize() {
	w = imgFrame->width; h = imgFrame->height;
	CvMat *A, *B, *X;
	switch (m_cameraType) {
		case PROJECTION: {
			// see Cipolla, R and Drummond, Tom and Robertson, D
			// "Camera Calibration from Vanishing Points in Images of Architectural Scenes"
			// check if there are enough reference heights
			if (num.GetCount() != r.GetCount())
				return false;	

			// check if there are enough lines for calibration
			if (x.GetCount() < 2 || y.GetCount() < 2 
				|| z.GetCount() < 2 || r.GetCount() < 1)
				return false;

			//// =========================================================================
			// Compute vanishing points 
			// Important!!! Calibration lines for VX, VY, VZ must be aiming toward the vanishing points, not away from them!!!
			FindVanishingPoint(x, VX, w, h);
			FindVanishingPoint(y, VY, w, h);
			FindVanishingPoint(z, VZ, w, h);
			//double u1 = cvmGet(VX,0,0)/cvmGet(VX, 2, 0), v1 = cvmGet(VX,1,0)/cvmGet(VX, 2, 0);
			//double u2 = cvmGet(VY,0,0)/cvmGet(VY, 2, 0), v2 = cvmGet(VY,1,0)/cvmGet(VY, 2, 0);
			//double u3 = cvmGet(VZ,0,0)/cvmGet(VZ, 2, 0), v3 = cvmGet(VZ,1,0)/cvmGet(VZ, 2, 0);

			cvmSet(P, 0, 0, cvmGet(VX, 0, 0)); 
			cvmSet(P, 1, 0, cvmGet(VX, 1, 0)); 
			cvmSet(P, 2, 0, cvmGet(VX, 2, 0)); 
			cvmSet(P, 0, 1, cvmGet(VY, 0, 0)); 
			cvmSet(P, 1, 1, cvmGet(VY, 1, 0)); 
			cvmSet(P, 2, 1, cvmGet(VY, 2, 0)); 
			cvmSet(P, 0, 2, cvmGet(VZ, 0, 0)); 
			cvmSet(P, 1, 2, cvmGet(VZ, 1, 0)); 
			cvmSet(P, 2, 2, cvmGet(VZ, 2, 0)); 


			//// =========================================================================
			// Find camera elevation
			// Find h using height samples: assume p14 = -h*p13; p24 = -h*p23; p34 = -h*p33
			// sbot*(x1, y1, 1)=P*(X, Y, Zbot, 1) 
			// stop*(x2, y2, 1)=P*(X, Y, Ztop, 1) 
			// UNKNOWNS: X, Y, Zbot, Ztop, sbot, stop, h (aka. m_elevation)
			CvMat *top, *bot;
			m_elevation = 0;
			A = cvCreateMat(7, 7, CV_64FC1); 
			B = cvCreateMat(7, 1, CV_64FC1); 
			X = cvCreateMat(7, 1, CV_64FC1); 
			bot = cvCreateMat(3, 1, CV_64FC1); 
			top = cvCreateMat(3, 1, CV_64FC1); 
			cvZero(B);
			for (int n=0;n<r.GetCount();n++) {
				CvPoint2D32f vzImage = coordsHomo2Image(VZ);
				CvPoint2D32f botImage = cvPointTo32f(cvPoint(r[n].p1)), topImage = cvPointTo32f(cvPoint(r[n].p2));
				if (d(botImage, vzImage) > d(topImage, vzImage)) {
					CvPoint2D32f tempImage = botImage;
					botImage = topImage; topImage = tempImage;
				}
				coordsImage2Homo(botImage, bot);
				coordsImage2Homo(topImage, top);
				double A_[] = {
					cvmGet(P,0,0), cvmGet(P,0,1), cvmGet(P,0,2),             0, -cvmGet(bot, 0, 0),                  0, -cvmGet(P,0,2),
					cvmGet(P,0,0), cvmGet(P,0,1),             0, cvmGet(P,0,2),                  0, -cvmGet(top, 0, 0), -cvmGet(P,0,2),
					cvmGet(P,1,0), cvmGet(P,1,1), cvmGet(P,1,2),             0, -cvmGet(bot, 1, 0),                  0, -cvmGet(P,1,2),
					cvmGet(P,1,0), cvmGet(P,1,1),             0, cvmGet(P,1,2),                  0, -cvmGet(top, 1, 0), -cvmGet(P,1,2),
					cvmGet(P,2,0), cvmGet(P,2,1), cvmGet(P,2,2),             0,                 -1,                  0, -cvmGet(P,2,2),
					cvmGet(P,2,0), cvmGet(P,2,1),             0, cvmGet(P,2,2),                  0,                 -1, -cvmGet(P,2,2),
					0,             0,            -1,             1,                  0,                  0, 0
				};
				memcpy(A->data.db, A_, sizeof(A_));
				cvSetReal2D(B, 6, 0, num[n]);
				cvSolve(A, B, X, CV_SVD);
				m_elevation += (double)(cvGetReal2D(X,6,0));
			}
			m_elevation /= r.GetCount();
			cvReleaseMat(&A); cvReleaseMat(&B); cvReleaseMat(&X); cvReleaseMat(&bot); cvReleaseMat(&top);

			// Set translation from world to camera coordinates; column 4 is the image of the world origin
			cvmSet(P, 0, 3, -m_elevation*cvmGet(VZ, 0, 0)); 
			cvmSet(P, 1, 3, -m_elevation*cvmGet(VZ, 1, 0)); 
			cvmSet(P, 2, 3, -m_elevation*cvmGet(VZ, 2, 0)); 

			// intrinsic
			mFocal = 100;
			cvmSet(K, 0, 0, mFocal); cvmSet(K, 0, 1, 0); cvmSet(K, 0, 2, w/2); 
			cvmSet(K, 1, 0, 0); cvmSet(K, 1, 1, mFocal); cvmSet(K, 1, 2, h/2); 
			cvmSet(K, 2, 0, 0); cvmSet(K, 2, 1, 0); cvmSet(K, 2, 2, 1); 

			// identity
			CvMat* I = cvCreateMat(3, 4, CV_64FC1);
			cvSetIdentity(I);

			// extrinsic
			CvMat* P4x4 = cvCreateMat(4, 4, CV_64FC1);
			cvZero(P4x4);
			cvmSet(P4x4, 0, 0, cvmGet(VX, 0, 0)); cvmSet(P4x4, 0, 1, cvmGet(VY, 0, 0)); cvmSet(P4x4, 0, 2, cvmGet(VZ, 0, 0)); cvmSet(P4x4, 0, 3, cvmGet(P, 0, 3));
			cvmSet(P4x4, 1, 0, cvmGet(VX, 1, 0)); cvmSet(P4x4, 1, 1, cvmGet(VY, 1, 0)); cvmSet(P4x4, 1, 2, cvmGet(VZ, 1, 0)); cvmSet(P4x4, 1, 3, cvmGet(P, 1, 3));
			cvmSet(P4x4, 2, 0, cvmGet(VX, 2, 0)); cvmSet(P4x4, 2, 1, cvmGet(VY, 2, 0)); cvmSet(P4x4, 2, 2, cvmGet(VZ, 2, 0)); cvmSet(P4x4, 2, 3, cvmGet(P, 2, 3));
			cvmSet(P4x4, 3, 0, 0); cvmSet(P4x4, 3, 1, 0); cvmSet(P4x4, 3, 2, 0); cvmSet(P4x4, 3, 3, 1); 

			// final P
			cvMatMul(K, I, P);
			cvMatMul(P, P4x4, P);
			cvReleaseMat(&I);
			cvReleaseMat(&P4x4);

			cvmSet(VX, 0, 0, cvmGet(P, 0, 0)); 
			cvmSet(VX, 1, 0, cvmGet(P, 1, 0)); 
			cvmSet(VX, 2, 0, cvmGet(P, 2, 0)); 
			cvmSet(VY, 0, 0, cvmGet(P, 0, 1)); 
			cvmSet(VY, 1, 0, cvmGet(P, 1, 1)); 
			cvmSet(VY, 2, 0, cvmGet(P, 2, 1)); 
			cvmSet(VZ, 0, 0, cvmGet(P, 0, 2)); 
			cvmSet(VZ, 1, 0, cvmGet(P, 1, 2)); 
			cvmSet(VZ, 2, 0, cvmGet(P, 2, 2)); 
			coordsImage2Homo(cvPoint2D32f(w/2, h/2), V0);

			break;
		}
		case SPHERICAL: {
			break;
		}
		case XMLFILE: {
			UtilXml::Init();
			std::ifstream is;
			is.open(m_sourcename);
			fromXml(is);
			UtilXml::Cleanup();

			// intrinsic
			cvmSet(K, 0, 0, mFocal*mSx/mDpx); cvmSet(K, 0, 1, mKappa1); cvmSet(K, 0, 2, mCx); 
			cvmSet(K, 1, 0, 0); cvmSet(K, 1, 1, mFocal/mDpy); cvmSet(K, 1, 2, mCy); 
			cvmSet(K, 2, 0, 0); cvmSet(K, 2, 1, 0); cvmSet(K, 2, 2, 1); 

			// identity
			CvMat* I = cvCreateMat(3, 4, CV_64FC1);
			cvSetIdentity(I);
			
			// extrinsic
			CvMat* P4x4 = cvCreateMat(4, 4, CV_64FC1);
			cvmSet(P4x4, 0, 0, mR11); cvmSet(P4x4, 0, 1, mR12); cvmSet(P4x4, 0, 2, mR13); cvmSet(P4x4, 0, 3, mTx); 
			cvmSet(P4x4, 1, 0, mR21); cvmSet(P4x4, 1, 1, mR22); cvmSet(P4x4, 1, 2, mR23); cvmSet(P4x4, 1, 3, mTy); 
			cvmSet(P4x4, 2, 0, mR31); cvmSet(P4x4, 2, 1, mR32); cvmSet(P4x4, 2, 2, mR33); cvmSet(P4x4, 2, 3, mTz); 
			cvmSet(P4x4, 3, 0, 0); cvmSet(P4x4, 3, 1, 0); cvmSet(P4x4, 3, 2, 0); cvmSet(P4x4, 3, 3, 1); 
			
			

			// final P
			cvMatMul(K, I, P);
			cvMatMul(P, P4x4, P);
			cvReleaseMat(&I);
			cvReleaseMat(&P4x4);

			cvmSet(VX, 0, 0, cvmGet(P, 0, 0)); 
			cvmSet(VX, 1, 0, cvmGet(P, 1, 0)); 
			cvmSet(VX, 2, 0, cvmGet(P, 2, 0)); 
			cvmSet(VY, 0, 0, cvmGet(P, 0, 1)); 
			cvmSet(VY, 1, 0, cvmGet(P, 1, 1)); 
			cvmSet(VY, 2, 0, cvmGet(P, 2, 1)); 
			cvmSet(VZ, 0, 0, cvmGet(P, 0, 2)); 
			cvmSet(VZ, 1, 0, cvmGet(P, 1, 2)); 
			cvmSet(VZ, 2, 0, cvmGet(P, 2, 2)); 
			coordsImage2Homo(cvPoint2D32f(mCx, mCy), V0);

			m_elevation = mCposz;
		}
	}

	// initialize distance_mask and m_minBodyDistance, m_maxBodyDistance
	// TEMP: the largest distance
	//double LARGE_DIST = 2000;
	double LARGE_DIST = 50000;
	m_minBodyDistance = INT_MAX;
	m_maxBodyDistance = 0;
	for (int x=0;x<w;x++) {
		for (int y=0;y<h;y++) {
			double dist = coordsDistanceFromCamera(coordsImage2Real(cvPoint2D32f(x, y), doc->bodymodel.m_heightM/2));
			dist > LARGE_DIST ? dist = 0.2*LARGE_DIST : 1; // artificially lower the weight of the far-away pixels (usually they are too noisy to consider)
			((float*)(distance_mask->imageData + distance_mask->widthStep*y))[x] = (float)dist;
			m_maxBodyDistance = max(m_maxBodyDistance, dist);
			m_minBodyDistance = min(m_minBodyDistance, dist);
		}
	}
	return true;
}
// --------------------------------------------------------------------------
void CameraDialog::DeInitialize() {
	if (m_hWnd && m_heights) 
		m_heights.Clear();
	x.RemoveAll();
	y.RemoveAll();
	z.RemoveAll();
	r.RemoveAll();
	w = 320; h = 240;
	m_cameraType = PROJECTION;
	m_elevation = 350;
}
// ------------------------------------------------------------------------
void CameraDialog::FindVanishingPoint(CArray<CLine>& x, CvMat* v, int w, int h) {
	CvMat* M = cvCreateMat(3, 3, CV_64FC1);
	cvZero(M);
	CvMat* EVALS = cvCreateMat(3, 1, CV_64FC1); 
	CvMat* EVECS = cvCreateMat(3, 3, CV_64FC1); 
	CvMat* e1 = cvCreateMat(3, 1, CV_64FC1); 
	CvMat* e2 = cvCreateMat(3, 1, CV_64FC1);
	CvMat* line = cvCreateMat(3, 1, CV_64FC1);

	for (int i=0;i<x.GetCount();i++) {
		coordsImage2Homo(cvPointTo32f(cvPoint(x[i].p1)), e1);
		coordsImage2Homo(cvPointTo32f(cvPoint(x[i].p2)), e2);
		cvCrossProduct(e1, e2, line);
		// fill bottom left triangle of M
		double a = cvGetReal1D(line, 0), b = cvGetReal1D(line, 1), c = cvGetReal1D(line, 2);
		cvSetReal2D(M, 0, 0, a*a+cvGetReal2D(M,0,0));
		cvSetReal2D(M, 0, 1, a*b+cvGetReal2D(M,0,1));
		cvSetReal2D(M, 0, 2, a*c+cvGetReal2D(M,0,2));
		cvSetReal2D(M, 1, 1, b*b+cvGetReal2D(M,1,1));
		cvSetReal2D(M, 1, 2, b*c+cvGetReal2D(M,1,2));
		cvSetReal2D(M, 2, 2, c*c+cvGetReal2D(M,2,2));
	}

	// fill the top right triangle of M symmetrically
	cvSetReal2D(M, 1, 0, cvGetReal2D(M,0,1));
	cvSetReal2D(M, 2, 0, cvGetReal2D(M,0,2));
	cvSetReal2D(M, 2, 1, cvGetReal2D(M,1,2));

	// find eigen vector that corresponds to the smallest eigen value
	cvSVD(M, EVALS, EVECS, 0, CV_SVD_MODIFY_A);
	double a[3]= {cvGetReal2D(EVECS,0,2), cvGetReal2D(EVECS,1,2), cvGetReal2D(EVECS,2,2)};
	cvReleaseMat(&M);
	cvReleaseMat(&EVALS);
	cvReleaseMat(&EVECS);
	cvReleaseMat(&e1);
	cvReleaseMat(&e2);
	cvReleaseMat(&line);
	cvSetReal1D( v, 0, a[0]);cvSetReal1D( v, 1, a[1]);cvSetReal1D( v, 2, a[2]);
}
// --------------------------------------------------------------------------
void CameraDialog::OnBnClickedButtonAdd()
{
	char s[10];
	m_currentHeight.GetLine(0, s, 4);
	if (atoi(s)) {
		m_heights.Add(atoi(s));
	}
	else 
		AfxMessageBox("Please enter a numeric value!", MB_ICONINFORMATION | MB_OK);

	m_currentHeight.SetSel(0,-1); m_currentHeight.Clear();
}
// --------------------------------------------------------------------------
void CameraDialog::OnBnClickedButtonDeleteAll()
{
	m_heights.Clear();
}
// --------------------------------------------------------------------------
void CameraDialog::OnBnClickedButtonDelete()
{
	if (m_heights.GetCurSel() != LB_ERR)
		m_heights.RemoveCurrent();
}
// --------------------------------------------------------------------------
void CameraDialog::OnBnClickedButtonReset()
{
	x.RemoveAll();
	y.RemoveAll();
	z.RemoveAll();
	r.RemoveAll();
	m_heights.Clear();
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void CameraDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int curpos = pScrollBar->GetScrollPos();
	int minpos;
	int maxpos;
	pScrollBar->GetScrollRange(&minpos, &maxpos);
	SCROLLINFO   info;
    pScrollBar->GetScrollInfo(&info, SIF_ALL);
	switch (nSBCode)
    {
		case SB_TOP:      
			curpos = minpos;
			break;
		case SB_BOTTOM:     
			curpos = maxpos;
			break;
		case SB_ENDSCROLL: 
			break;
		case SB_LINEUP:      
			if (curpos > minpos)
				curpos--;
			break;
		case SB_LINEDOWN:   
			if (curpos < maxpos)
				curpos++;
			break;
		case SB_PAGEUP:    
			curpos = max(minpos, curpos - (int) info.nPage);
			break;
		case SB_PAGEDOWN:
			curpos = min(maxpos, curpos + (int) info.nPage);
			break;
		case SB_THUMBPOSITION: 
			curpos = nPos;     
			break;
		case SB_THUMBTRACK:
			curpos = nPos;     
			break;
    }    
	offsety = curpos;
	pScrollBar->SetScrollPos(curpos);
	m_image.Invalidate();
    CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
// --------------------------------------------------------------------------
void CameraDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int curpos = pScrollBar->GetScrollPos();
	int minpos;
	int maxpos;
	pScrollBar->GetScrollRange(&minpos, &maxpos);
	SCROLLINFO   info;
    pScrollBar->GetScrollInfo(&info, SIF_ALL);
	switch (nSBCode)
    {
		case SB_LEFT:      
			curpos = minpos;
			break;
		case SB_RIGHT:     
			curpos = maxpos;
			break;
		case SB_ENDSCROLL: 
			break;
		case SB_LINELEFT:      
			if (curpos > minpos)
				curpos--;
			break;
		case SB_LINERIGHT:   
			if (curpos < maxpos)
				curpos++;
			break;
		case SB_PAGELEFT:    
			curpos = max(minpos, curpos - (int) info.nPage);
			break;
		case SB_PAGERIGHT:
			curpos = min(maxpos, curpos + (int) info.nPage);
			break;
		case SB_THUMBPOSITION: 
			curpos = nPos;     
			break;
		case SB_THUMBTRACK:
			curpos = nPos;     
			break;
    }    
	offsetx = curpos; 
	pScrollBar->SetScrollPos(curpos);
	m_image.Invalidate();
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
// --------------------------------------------------------------------------
// ==========================================================================
// -------------------------- CNumberListBox class --------------------------
void CNumberListBox::Add(int number) {
	CameraDialog* dlg=((CameraDialog*)this->GetParent());
	dlg->num.Add(number);
	UpdateData(false);
}
// --------------------------------------------------------------------------
void CNumberListBox::Clear() {
	CameraDialog* dlg=((CameraDialog*)this->GetParent());
	dlg->num.RemoveAll();
	UpdateData(false);
}
// --------------------------------------------------------------------------
void CNumberListBox::RemoveCurrent() {
	CameraDialog* dlg=((CameraDialog*)this->GetParent());
	dlg->num.RemoveAt(GetCurSel());
	UpdateData(false);
}
// --------------------------------------------------------------------------
void CNumberListBox::UpdateData(BOOL bSaveAndValidate) {
	CListBox::UpdateData(bSaveAndValidate);
	CameraDialog* dlg=((CameraDialog*)this->GetParent());
	if (bSaveAndValidate) {
		dlg->num.RemoveAll();
		for (int i=0;i<GetCount();i++)
			dlg->num[i] = (DWORD) GetItemData(i);
	}
	else {
		while(GetCount() > 0)
			DeleteString(0);
		for (int i=0;i<dlg->num.GetCount();i++) {
			char name[20];
			StringCchPrintf(name, 20, "line %d is %d cm", GetCount()+1, dlg->num[i]);
			AddString(name);
		}
	}
}
// =========================================================================
// --------------------- CCalibrateControl class ---------------------------
CCalibrateControl::CCalibrateControl() {
	m_dragging = false;
	m_creating = false;
}
// --------------------- CCalibrateControl class ---------------------------
bool CCalibrateControl::Initialize(CNewVisionDoc* doc) {
	// Get first frame from file or camera
	image.CopyOf(imgFrame);
	// Show scroll bars
	if (m_hWnd) {
		CameraDialog* tab = (CameraDialog*)GetParent();
		CRect r; this->GetWindowRect(&r);
		SCROLLINFO horz;
		horz.cbSize = sizeof(SCROLLINFO);
		horz.fMask = SIF_ALL;
		horz.nPage = 50;
		horz.nMin = 0;
		horz.nMax = image.Width()-r.Width()+horz.nPage;
		horz.nPos = 0;
		horz.nTrackPos = 0;
		tab->m_hbar.SetScrollInfo(&horz);
		SCROLLINFO vert;
		vert.cbSize = sizeof(SCROLLINFO);
		vert.fMask = SIF_ALL;
		vert.nPage = 50;
		vert.nMin = 0;
		vert.nMax = image.Height()-r.Height()+vert.nPage;
		vert.nPos = 0;
		vert.nTrackPos = 0;
		tab->m_vbar.SetScrollInfo(&vert);
		tab->m_vbar.ShowWindow(true);  //Hide Vertical Scroll Bar
		tab->m_hbar.ShowWindow(true);  //Hide Horizontal Scroll Bar
		tab->offsetx = 0; tab->offsety = 0;
		this->Invalidate();
	}
	return true;
}
// --------------------------------------------------------------------------
void CCalibrateControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CameraDialog* dlg=(CameraDialog*)this->GetParent();
	image.Show(pDC->m_hDC, 0, 0, image.Width(), image.Height(), dlg->offsetx, dlg->offsety);
	
	CPen red(PS_SOLID, 1, RGB(255, 0, 0));
	CPen green(PS_SOLID, 1, RGB(0, 255, 0));
	CPen blue(PS_SOLID, 1, RGB(0, 0, 255));
	CPen aqua(PS_SOLID, 1, RGB(0, 255, 255));
	DrawLines(pDC, dlg->x, &red);
	DrawLines(pDC, dlg->y, &green);
	DrawLines(pDC, dlg->z, &blue);
	DrawLines(pDC, dlg->r, &aqua);
	
	
	// draw vanishing points !!!!!!!!!
	CvPoint VX = cvPointFrom32f(dlg->coordsHomo2Image(dlg->VX));
	CvPoint VY = cvPointFrom32f(dlg->coordsHomo2Image(dlg->VY));
	CvPoint VZ = cvPointFrom32f(dlg->coordsHomo2Image(dlg->VZ));
	CBrush bred(RGB(255, 0, 0));
	CBrush bgreen(RGB(0, 255, 0));
	CBrush bblue(RGB(0, 0, 255));
 	pDC->SelectObject(bred);
	pDC->Ellipse(VX.x-2*DOT-dlg->offsetx, VX.y-2*DOT-dlg->offsety, VX.x+2*DOT-dlg->offsetx, VX.y+2*DOT-dlg->offsety);
	pDC->SelectObject(bgreen);
	pDC->Ellipse(VY.x-2*DOT-dlg->offsetx, VY.y-2*DOT-dlg->offsety, VY.x+2*DOT-dlg->offsetx, VY.y+2*DOT-dlg->offsety);
	pDC->SelectObject(bblue);
	pDC->Ellipse(VZ.x-2*DOT-dlg->offsetx, VZ.y-2*DOT-dlg->offsety, VZ.x+2*DOT-dlg->offsetx, VZ.y+2*DOT-dlg->offsety);
	
}
// --------------------------------------------------------------------------
void CCalibrateControl::DrawLines(CDC* pDC, CArray<CLine>& x, CPen* pen) {
	CBrush light(RGB(255, 255, 0));
	CBrush dark(RGB(50, 50, 0));
	CBrush* pOldBrush = pDC->SelectObject(&dark);
	CPen* pOldPen = pDC->SelectObject(pen);
	CameraDialog* dlg=(CameraDialog*)this->GetParent();
	for (int i=0;i<x.GetCount();i++) {
		CPoint p1 = CPoint(x[i].p1.x-dlg->offsetx, x[i].p1.y-dlg->offsety); 
		CPoint p2 = CPoint(x[i].p2.x-dlg->offsetx, x[i].p2.y-dlg->offsety);

		// draw a line
		pDC->SelectObject(pen);
		pDC->MoveTo(p1);
		pDC->LineTo(p2);

		// draw numbered boxes
		pDC->SetBkColor(RGB(0,100,100));
		pDC->SetTextColor(RGB(255,255,255));
		char c[3];
		pDC->DrawText(itoa(i+1, c, 10), CRect(p1, CSize(12*((i%10)+1), 16)), DT_SINGLELINE);
		pDC->DrawText(itoa(i+1, c, 10), CRect(p2, CSize(12*((i%10)+1), 16)), DT_SINGLELINE);
		
		// draw endpoints
		pDC->SelectObject(&dark);
		pDC->Ellipse(p1.x-DOT, p1.y-DOT, p1.x+DOT, p1.y+DOT);
		pDC->Ellipse(p2.x-DOT, p2.y-DOT, p2.x+DOT, p2.y+DOT);
		if (m_dragging) {
			pDC->SelectObject(&light);
			pDC->Ellipse(m_dragPoint->x-DOT-dlg->offsetx, m_dragPoint->y-DOT-dlg->offsety, m_dragPoint->x+DOT-dlg->offsetx, m_dragPoint->y+DOT-dlg->offsety);
		}	
	}
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}
// --------------------------------------------------------------------------
BOOL CCalibrateControl::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	if( (message == WM_LBUTTONDOWN) ||   
		(message == WM_MBUTTONDOWN) ||
		(message == WM_RBUTTONDOWN) )
		OnButtonDown(LOWORD(wParam), CPoint(lParam));
	if( (message == WM_LBUTTONUP) ||   
		(message == WM_MBUTTONUP) ||
		(message == WM_RBUTTONUP) )
		OnButtonUp(LOWORD(wParam), CPoint(lParam));
	if( message == WM_KEYDOWN && m_dragging && wParam == VK_DELETE ) {
		m_dragArray->RemoveAt(m_dragLine);
		Invalidate();
		UpdateWindow();
	}
	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}
// --------------------------------------------------------------------------
void CCalibrateControl::OnButtonDown(UINT nFlags, CPoint point) {
	CWnd::OnLButtonDown(nFlags, point);
	CameraDialog* dlg=((CameraDialog*)this->GetParent());
	point.x += dlg->offsetx;
	point.y += dlg->offsety;
	if (!ChangeLine(dlg->x, point) 
		&& !ChangeLine(dlg->y, point) 
		&& !ChangeLine(dlg->z, point) 
		&& !ChangeLine(dlg->r, point)) {
		// create new point
		CLine l = CLine(point, point);
		if (nFlags & MK_CONTROL) {
			dlg->r.Add(l); m_createPoint = &(dlg->r[dlg->r.GetCount()-1].p2);
		}
		else if (nFlags & MK_LBUTTON) {
			dlg->x.Add(l); m_createPoint = &(dlg->x[dlg->x.GetCount()-1].p2);
		}
		else if (nFlags & MK_MBUTTON) {
			dlg->y.Add(l); m_createPoint = &(dlg->y[dlg->y.GetCount()-1].p2);
		}
		else if (nFlags & MK_RBUTTON) {
			dlg->z.Add(l); m_createPoint = &(dlg->z[dlg->z.GetCount()-1].p2);
		}
		m_creating = true;
	}
}
// --------------------------------------------------------------------------
bool CCalibrateControl::ChangeLine(CArray<CLine>& l, CPoint point) {
	for (int i=0;i<l.GetCount();i++) {
		CRect rect1 = CRect(l[i].p1, CSize(12*((i%10)+1), 16)); // i>0 first time?
		CRect rect2 = CRect(l[i].p2, CSize(12*((i%10)+1), 16));
		if(rect1.PtInRect(point)) {
			m_dragging = true;
			m_dragPoint = &(l[i].p1);
			m_dragLine = i;
			m_dragArray = &l;
			Invalidate();
			UpdateWindow();
			return true;

		}
		if(rect2.PtInRect(point)) {
			m_dragging = true;
			m_dragPoint = &(l[i].p2);
			m_dragLine = i;
			m_dragArray = &l;
			Invalidate();
			UpdateWindow();
			return true;
		}
	}
	return false; // line unchanged
}
// --------------------------------------------------------------------------
afx_msg void CCalibrateControl::OnMouseMove(UINT nFlags, CPoint point) {
	CameraDialog* dlg=((CameraDialog*)this->GetParent());
	if (m_dragging) {
		*m_dragPoint = CPoint(point.x + dlg->offsetx, point.y + dlg->offsety);
		Invalidate();
		UpdateWindow();
	}
	if (m_creating) {
		*m_createPoint = CPoint(point.x + dlg->offsetx, point.y + dlg->offsety);
		Invalidate();
		UpdateWindow();
	}
}
// --------------------------------------------------------------------------
void CCalibrateControl::OnButtonUp(UINT nFlags, CPoint point) {
	m_dragging = false;
	m_creating = false;
	Invalidate();
	UpdateWindow();
}


void CameraDialog::OnBnClickedButtonBrowseXmlFile()
{
	UpdateData();
	if (m_cameraType == XMLFILE) {
		CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, "XML files (*.xml)|*.xml||", this);
		char title[]= {"Open Camera Calibration File"};
		dlg.m_ofn.lpstrTitle = title;
		if (dlg.DoModal() == IDOK) {
			m_sourcename = dlg.GetPathName();
		}
	}
	else {
		return;
	}

	if (!Initialize())
		m_sourcename = "";
	UpdateData(false);	
}
