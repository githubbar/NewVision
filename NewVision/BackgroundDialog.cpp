#include "stdafx.h"
#include "NewVision.h"
#include "NewVisionDoc.h"
#include "BackgroundDialog.h"
#include "TrackerModel.h"

// --------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(BackgroundDialog, CDialog)
	ON_BN_CLICKED(IDC_RESET, OnBnClickedReset)
	ON_BN_CLICKED(IDC_RADIOLEARNING, OnBnClickedRadioLearning)
	ON_BN_CLICKED(IDC_RADIODYNAMIC, OnBnClickedRadioDynamic)
END_MESSAGE_MAP()
// --------------------------------------------------------------------------
BackgroundDialog::BackgroundDialog(CWnd* pParent /*=NULL*/)
	: CPropertyPage(BackgroundDialog::IDD), BackgroundModel()
{
}
// --------------------------------------------------------------------------
BackgroundDialog::~BackgroundDialog()
{
}
// --------------------------------------------------------------------------
bool BackgroundDialog::Initialize() {
	return BackgroundModel::Initialize();
}
// --------------------------------------------------------------------------
void BackgroundDialog::DeInitialize() {
	BackgroundModel::DeInitialize();
	m_mode = 1;
	m_CDmax = 12;
	m_BDmin = 0.8;
	m_BDmax = 1.2;
	m_ThermalMin = 20;
	m_ThermalMax = 20;
	m_initStage = 200;
	m_staleCutoff = 600;
	m_removeStaleFreq = 100;
	m_minAge = 5;
	m_trackAfterAge = 20;
	m_trackAfterAgePercentage = 30;
	m_updateFreq = 10;
	m_sensitivity = 0.5;

	m_Erode = 1;
	m_ErodeSize = 2;
	m_Dilate = 1;
	m_DilateSize = 3;
}
// --------------------------------------------------------------------------
void BackgroundDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	// convert frames to seconds
	double sec_m_initStage, sec_m_staleCutoff, sec_m_removeStaleFreq;
	if (!pDX->m_bSaveAndValidate) {
		sec_m_initStage = doc->trackermodel.Frames2Seconds(m_initStage);
		sec_m_staleCutoff = doc->trackermodel.Frames2Seconds(m_staleCutoff);
		sec_m_removeStaleFreq = doc->trackermodel.Frames2Seconds(m_removeStaleFreq);
	}
	
	DDX_Text(pDX, IDC_CDMAX, m_CDmax);
	DDX_Text(pDX, IDC_BDMIN, m_BDmin);
	DDX_Text(pDX, IDC_BDMAX, m_BDmax);

	DDX_Text(pDX, IDC_THERMALMIN, m_ThermalMin);
	DDX_Text(pDX, IDC_THERMALMAX, m_ThermalMax);
	

	DDX_Text(pDX, IDC_MINAGE, m_minAge );
	DDX_Text(pDX, IDC_TRACKAFTERAGE, m_trackAfterAge );
	DDX_Text(pDX, IDC_TRACKAFTERPERCENTAGE, m_trackAfterAgePercentage );
	DDX_Text(pDX, IDC_UPDATEFREQ, m_updateFreq);
	DDX_Text(pDX, IDC_BGSENSITIVITY, m_sensitivity);
	
	
	DDX_Radio(pDX, IDC_RADIOLEARNING, m_mode);
	DDX_Check(pDX, IDC_ERODE, m_Erode);
	DDX_Text(pDX, IDC_ERODESIZE, m_ErodeSize);
	DDX_Check(pDX, IDC_DILATE, m_Dilate);
	DDX_Text(pDX, IDC_DILATESIZE, m_DilateSize);

	DDX_Text(pDX, IDC_INITFORNFRAMES, sec_m_initStage);
	DDX_Text(pDX, IDC_STALECUTOFF, sec_m_staleCutoff);
	DDX_Text(pDX, IDC_CLEANSTALEEVERY, sec_m_removeStaleFreq);

	// convert seconds to frames 
	if (pDX->m_bSaveAndValidate) {
		m_initStage = doc->trackermodel.Seconds2Frames(sec_m_initStage);
		m_staleCutoff = doc->trackermodel.Seconds2Frames(sec_m_staleCutoff);
		m_removeStaleFreq = doc->trackermodel.Seconds2Frames(sec_m_removeStaleFreq);
	}

	DDV_MinMaxInt(pDX, m_CDmax, 0, 255);
	DDV_MinMaxDouble(pDX, m_BDmin, 0, 0.99);
	DDV_MinMaxDouble(pDX, m_BDmax, 1, 9.99);
	DDV_MinMaxDouble(pDX, m_initStage, 0, 10000000);
	DDV_MinMaxDouble(pDX, m_staleCutoff, 1, 10000000);
	DDV_MinMaxDouble(pDX, m_removeStaleFreq, 1, 10000000);
	DDV_MinMaxInt(pDX, m_minAge, 0, 100);
	DDV_MinMaxInt(pDX, m_trackAfterAge, 0, 10000000 );
	DDV_MinMaxInt(pDX, m_trackAfterAgePercentage, 0, 100);
	DDV_MinMaxInt(pDX, m_ErodeSize, 1, 10);
	DDV_MinMaxInt(pDX, m_DilateSize, 1, 10);

	UpdateControls();
}
// --------------------------------------------------------------------------
void BackgroundDialog::UpdateControls() {
	switch (m_mode) {
		case 0:
			GetDlgItem(IDC_INITFORNFRAMES)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_TEXTLEARNING1)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_TEXTDYN1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_TEXTDYN2)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_TEXTDYN3)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_TRACKAFTERAGE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_TRACKAFTERPERCENTAGE)->ShowWindow(SW_HIDE);
			break;
		case 1:
			GetDlgItem(IDC_TEXTDYN1)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_TEXTDYN2)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_TEXTDYN3)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_TRACKAFTERAGE)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_TRACKAFTERPERCENTAGE)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_INITFORNFRAMES)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_TEXTLEARNING1)->ShowWindow(SW_HIDE);
	};

}
// --------------------------------------------------------------------------
void BackgroundDialog::OnBnClickedRadioLearning()
{
	UpdateData();
}
// --------------------------------------------------------------------------
void BackgroundDialog::OnBnClickedRadioDynamic()
{
	UpdateData();
}
// --------------------------------------------------------------------------
void BackgroundDialog::OnBnClickedReset()
{
	{ // +++ begin data processing critical section	
		CAutoLock dataLock( &dataCriticalSection); 
		RemoveAll();
	} // +++ end data processing critical section	
}
