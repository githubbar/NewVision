#include "stdafx.h"
#include "NewVision.h"
#include "TrackerDialog.h"
#include "NewVisionDoc.h"
#include "NewVisionView.h"
#include "SelectCameraDialog.h"
#include "TrackerDialog.h"
#include "MainFrm.h"


BEGIN_MESSAGE_MAP(TrackerDialog, CPropertyPage)
	ON_BN_CLICKED(IDC_SOURCESELECTBUTTON, OnBnClickedSourceSelectButton)
	ON_BN_CLICKED(IDC_SOURCESELECTBUTTONT, OnBnClickedSourceSelectButtonT)
END_MESSAGE_MAP()
// --------------------------------------------------------------------------
TrackerDialog::TrackerDialog()
	: CPropertyPage(TrackerDialog::IDD)
{	
	m_frameStart = 0;
	m_frameStop = 0;
	m_startTime = CTime::GetCurrentTime();
}
// --------------------------------------------------------------------------
bool TrackerDialog::Initialize() {
	return TrackerModel::Initialize();
}
// --------------------------------------------------------------------------
void TrackerDialog::DeInitialize() {
	TrackerModel::DeInitialize();
	m_sourcename = _T("");
	m_sourcenameT = _T("");
	m_mode = 0;
	m_frameStart = 0;
	m_frameStop = 0;
	m_recordVideoView = 0; 
	m_recordSceneView = 0;
	m_recordActivityView = 0;
	m_recordFloorView = 0;

	m_startTime = CTime::GetCurrentTime();
}
// --------------------------------------------------------------------------
TrackerDialog::~TrackerDialog()
{
}
// --------------------------------------------------------------------------
void TrackerDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SOURCENAME, m_sourcename);
	DDX_Text(pDX, IDC_SOURCENAMET, m_sourcenameT);
	DDX_Radio(pDX, IDC_RADIOFILE, m_mode);
	DDX_Text(pDX, IDC_FRAMESTART, m_frameStart);
	DDX_Text(pDX, IDC_FRAMESTOP, m_frameStop);
	CTime time1 = m_startTime, date1 = m_startTime;
	DDX_DateTimeCtrl(pDX, IDC_STARTTIME, time1);
	DDX_MonthCalCtrl(pDX, IDC_STARTDATE, date1);
	if (pDX->m_bSaveAndValidate) {
		m_startTime = CTime(
			date1.GetYear(),
			date1.GetMonth(),
			date1.GetDay(),
			time1.GetHour(),
			time1.GetMinute(),
			time1.GetSecond());
	}
}
// --------------------------------------------------------------------------
void TrackerDialog::OnBnClickedSourceSelectButton()
{
	UpdateData();
	if (m_mode == RGBFILE || m_mode == TRGBFILE || m_mode == PGRFILE) {
		CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, "AVI files (*.avi)|*.avi|MPEG files (*.mpg; *.mpeg)|*.mpeg; *.mpg|PGR files (*.pgr)|*.pgr|All Files (*.*)|*.*||", this);
		char title[]= {"Open Video File"};
		dlg.m_ofn.lpstrTitle = title;
		if (dlg.DoModal() == IDOK) {
			m_sourcename = dlg.GetPathName();
		}
	}
	else {
		//int* out;
		//int nselected = cvcamSelectCamera(&out);
		SelectCameraDialog camDlg;
		if (camDlg.DoModal()) {
			m_sourcename = camDlg.m_names[camDlg.m_choice];
		}
	}
	if (!Initialize())
		m_sourcename = "";
	UpdateData(false);
	theApp.m_pMainWnd->PostMessage(WM_UPDATEACTIVEVIEW);
}
// --------------------------------------------------------------------------
void TrackerDialog::OnBnClickedSourceSelectButtonT()
{
	UpdateData();
	if (m_mode == TRGBFILE) {
		CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, "AVI files (*.avi)|*.avi|MPEG files (*.mpg; *.mpeg)|*.mpeg; *.mpg|PGR files (*.pgr)|*.pgr|All Files (*.*)|*.*||", this);
		char title[]= {"Open Thermal Video File"};
		dlg.m_ofn.lpstrTitle = title;
		if (dlg.DoModal() == IDOK) {
			m_sourcenameT = dlg.GetPathName();
		}
	}
	UpdateData(false);
}
// --------------------------------------------------------------------------
void TrackerDialog::Serialize( CArchive& ar )
 {
	CObject::Serialize( ar );
	if( ar.IsStoring()) {
		ar << m_sourcename << m_mode << m_frameStart << m_frameStop << m_startTime;	
		if (m_mode == TRGBFILE)
			ar << m_sourcenameT;
	}
	else {
		ar >> m_sourcename >> m_mode >> m_frameStart >> m_frameStop >> m_startTime;
		if (m_mode == TRGBFILE)
			ar >> m_sourcenameT;
	}
 }
// --------------------------------------------------------------------------