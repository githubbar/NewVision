#include "stdafx.h"
#include "BodyActivityDialog.h"
#include "NewVisionDoc.h"
#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::Diagnostics;

// BodyActivityDialog dialog
// --------------------------------------------------------------------------
BodyActivityDialog::BodyActivityDialog(CWnd* pParent /*=NULL*/)
	: CPropertyPage(BodyActivityDialog::IDD)
{
}
// --------------------------------------------------------------------------
BodyActivityDialog::~BodyActivityDialog()
{
}
// --------------------------------------------------------------------------
bool BodyActivityDialog::Initialize() {
	if (!BodyActivityModel::Initialize())
		return false;
	m_initialized = true;
	return true;
}
// --------------------------------------------------------------------------
void BodyActivityDialog::DeInitialize() {
	BodyActivityModel::DeInitialize();

	// Sales representatives histogram threshold
	m_SalesRepHistThreshold = 0.90;

	// Smoothing and dwell time detection
	m_pathSmoothSigma = 15;
	m_dwellTime       = 60;
	m_dwellAreaRadius = 100;
}
// --------------------------------------------------------------------------
void BodyActivityDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	// convert frames to seconds
	double sec_m_pathSmoothSigma, sec_m_dwellTime;
	if (!pDX->m_bSaveAndValidate) {
		sec_m_pathSmoothSigma = doc->trackermodel.Frames2Seconds(m_pathSmoothSigma);
		sec_m_dwellTime = doc->trackermodel.Frames2Seconds(m_dwellTime);
	}

	DDX_Text(pDX, IDC_DWELLAREARADIUS, m_dwellAreaRadius);
	DDX_Text(pDX, IDC_SALESREPTHRESHOLD, m_SalesRepHistThreshold);
	DDX_Text(pDX, IDC_PATHSMOOTHSIGMA, sec_m_pathSmoothSigma);
	DDX_Text(pDX, IDC_DWELLTIME, sec_m_dwellTime);
	
	// convert seconds to frames
	if (pDX->m_bSaveAndValidate) {
		m_pathSmoothSigma = doc->trackermodel.Seconds2Frames(sec_m_pathSmoothSigma);
		m_dwellTime = doc->trackermodel.Seconds2Frames(sec_m_dwellTime);
	}

}
// --------------------------------------------------------------------------
void BodyActivityDialog::Serialize( CArchive& ar ) {
	CObject::Serialize(ar);
	if( ar.IsStoring()) {
		ar << m_SalesRepHistThreshold;
		ar << m_pathSmoothSigma << m_dwellTime << m_dwellAreaRadius;
	}
	else {
		ar >> m_SalesRepHistThreshold;
		ar >> m_pathSmoothSigma >> m_dwellTime >> m_dwellAreaRadius;
	}
}
// --------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(BodyActivityDialog, CPropertyPage)
END_MESSAGE_MAP()
