#include "stdafx.h"
#include "NewVision.h"
#include "BodyDialog.h"
#include "BodyActivityModel.h"
#include "TrackerModel.h"
#include "BodyDialog.h"
#using <mscorlib.dll>
#using <System.dll>
using namespace System;
using namespace System::Diagnostics;


// BodyDialog dialog
// --------------------------------------------------------------------------
BodyDialog::BodyDialog(CWnd* pParent /*=NULL*/)
	: CPropertyPage(BodyDialog::IDD)
{
}
// --------------------------------------------------------------------------
BodyDialog::~BodyDialog()
{
}
// --------------------------------------------------------------------------
bool BodyDialog::Initialize() {
	if (!BodyModel::Initialize())
		return false;
	m_bodyPriorsTab.mama  = this;
	m_bodyTrackingTab.mama = this;
	m_initialized = true;
	return true;
}
// --------------------------------------------------------------------------
BOOL BodyDialog::OnInitDialog() {
	if (!CPropertyPage::OnInitDialog()){ 
		return false;
	}	
	m_bodyPriorsTab.mama = this;
	m_bodyPriorsTab.Create(IDD0, this);
	m_bodyParameters.AddSSLPage(m_bodyPriorsTab.name, 0, &m_bodyPriorsTab);
	m_bodyTrackingTab.mama = this;
	m_bodyTrackingTab.Create(IDD1, this);
	m_bodyParameters.AddSSLPage(m_bodyTrackingTab.name, 0, &m_bodyTrackingTab);
	return true;
}
// --------------------------------------------------------------------------
void BodyDialog::DeInitialize() {
	BodyModel::DeInitialize();
	
	// body size priors
	m_heightM = 170;
	m_heightS = 20;
	m_widthM = 70;
	m_widthS = 20;
	m_minHeight = 120;
	m_maxHeight = 200;
	m_minWidth = 40;
	m_maxWidth = 100;

	// tracking parameters
	m_iterations = 20;
	m_iterationsPerBody = 6;
	m_randomness = 0.25;
	m_maxDoorDistance = 200;
	m_invisibleFor = 30;
	m_dormantFor = 100;
	m_outsiderFor = 30; 
	m_distanceConfidence = 60;

	// prior probability penlties
	p_BodyOverlap = 0.3;
	m_BodyMinDist = 0.5;
	p_ObstacleOverlap = 0.2;
	p_number = 0;
	p_new = 1;
	p_delete = 1;
	p_distance = 0.1;
	p_height = 0.1;
	p_width = 0.1;
	p_color = 0.1;
	m_UseColor = 1;
	p_background = 1; 
	p_foreground = 1; 
	p_abnormalW = 0.1;
	p_abnormalH = 0.1;


	// jump-diffusion probabilities
	p_add = 0.1;
	p_remove = 0.1;
	p_exchange = 0.1;
	p_update = 0.2;
	p_diffuse = 0.3;

	// Kalman parameters
	m_posModelNoise = 1;
	m_posMeasurementNoise = 50;
	m_orientModelNoise = 1e-3;
	m_orientMeasurementNoise = 1;

	// Tracking control parameters
	m_saturationTime = 15;
	m_maxHistChange = 0.2;
	m_maxPosChange = 30;
	m_maxDeletedAge = 20;
	m_maxDeletedDist = 200;
}
// --------------------------------------------------------------------------
void BodyDialog::BodyPriorsTab::DoDataExchange(CDataExchange* pDX) {
	// BUG: parameters are all zeroes ; if not usin safe wnd crashes!
	//BodyDialog* mama = (BodyDialog*)GetParent()->GetSafeHwnd();
	DDX_Text(pDX, IDC_HEIGHTM, mama->m_heightM);
	DDX_Text(pDX, IDC_HEIGHTS, mama->m_heightS);
	DDX_Text(pDX, IDC_WIDTHM, mama->m_widthM);
	DDX_Text(pDX, IDC_WIDTHS, mama->m_widthS);
	DDX_Text(pDX, IDC_HEIGHTMIN, mama->m_minHeight);
	DDX_Text(pDX, IDC_HEIGHTMAX, mama->m_maxHeight);
	DDX_Text(pDX, IDC_WIDTHMIN, mama->m_minWidth);
	DDX_Text(pDX, IDC_WIDTHMAX, mama->m_maxWidth);

	DDX_Text(pDX, IDC_PNONE, mama->p_number);
	DDX_Text(pDX, IDC_PNEW, mama->p_new);
	DDX_Text(pDX, IDC_PDELETE, mama->p_delete);
	DDX_Text(pDX, IDC_PDISTANCE, mama->p_distance);
	DDX_Text(pDX, IDC_PHEIGHT, mama->p_height);
	DDX_Text(pDX, IDC_PWIDTH, mama->p_width);
	DDX_Text(pDX, IDC_POBJECT, mama->p_color);
	DDX_Check(pDX, IDC_USECOLOR, mama->m_UseColor);
	DDX_Text(pDX, IDC_PBG, mama->p_background); 
	DDX_Text(pDX, IDC_PFG, mama->p_foreground); 
	DDX_Text(pDX, IDC_PABNORMALW, mama->p_abnormalW);
	DDX_Text(pDX, IDC_PABNORMALH, mama->p_abnormalH);
	DDX_Text(pDX, IDC_BODYOVERLAP, mama->p_BodyOverlap);
	DDX_Text(pDX, IDC_BODYMINDISTANCE, mama->m_BodyMinDist);
	DDX_Text(pDX, IDC_OBSTACLEOVERLAP, mama->p_ObstacleOverlap);
}
// --------------------------------------------------------------------------
void BodyDialog::BodyTrackingTab::DoDataExchange(CDataExchange* pDX) {
	//BodyDialog* mama = (BodyDialog*)GetParent()->GetSafeHwnd();
	DDX_Text(pDX, IDC_ITERATIONS, mama->m_iterations);
	DDX_Text(pDX, IDC_ITERATIONS_PER_BODY, mama->m_iterationsPerBody);
	DDX_Text(pDX, IDC_RANDOMNESS, mama->m_randomness);
	DDX_Text(pDX, IDC_DOORDISTANCE, mama->m_maxDoorDistance);
	DDX_Text(pDX, IDC_INVISIBLEFOR, mama->m_invisibleFor);
	DDX_Text(pDX, IDC_DORMANTFOR, mama->m_dormantFor);
	DDX_Text(pDX, IDC_OUTSIDERFOR, mama->m_outsiderFor);
	DDX_Text(pDX, IDC_DISTANCECONFIDENCE, mama->m_distanceConfidence);
	
	DDX_Text(pDX, IDC_SATURATIONTIME, mama->m_saturationTime);
	DDX_Text(pDX, IDC_MAXHISTCHANGE, mama->m_maxHistChange);
	DDX_Text(pDX, IDC_MAXPOSCHANGE, mama->m_maxPosChange);
	DDX_Text(pDX, IDC_MAXDELETEDAGE, mama->m_maxDeletedAge);
	DDX_Text(pDX, IDC_MAXDELETEDDIST, mama->m_maxDeletedDist);

	
	DDX_Text(pDX, IDC_POSMODELNOISE, mama->m_posModelNoise);
	DDX_Text(pDX, IDC_POSMEASUREMENTNOISE, mama->m_posMeasurementNoise);
	DDX_Text(pDX, IDC_ORIENTMODELNOISE, mama->m_orientModelNoise);
	DDX_Text(pDX, IDC_ORIENTMEASUREMENTNOISE, mama->m_orientMeasurementNoise);

	DDX_Text(pDX, IDC_ADD, mama->p_add);
	DDX_Text(pDX, IDC_REMOVE, mama->p_remove);
	DDX_Text(pDX, IDC_EXCHANGE, mama->p_exchange);
	DDX_Text(pDX, IDC_UPDATE, mama->p_update);
	DDX_Text(pDX, IDC_DIFFUSE, mama->p_diffuse);

}
// --------------------------------------------------------------------------
void BodyDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BODYTAB, m_bodyParameters);
	if (m_bodyParameters.GetSSLActivePage() >=0 ) {
		m_bodyPriorsTab.UpdateData(pDX->m_bSaveAndValidate);
		m_bodyTrackingTab.UpdateData(pDX->m_bSaveAndValidate);
	}
}
// --------------------------------------------------------------------------
void BodyDialog::Serialize( CArchive& ar ) {
	CObject::Serialize(ar);
	if( ar.IsStoring()) {
		ar << m_heightM << m_heightS << m_widthM << m_widthS;
		ar << m_minHeight << m_maxHeight << m_minWidth << m_maxWidth << m_iterations << m_iterationsPerBody << m_randomness;
		ar << m_posModelNoise << m_posMeasurementNoise << m_orientModelNoise << m_orientMeasurementNoise;
		ar << m_maxDoorDistance << m_invisibleFor << m_outsiderFor << m_distanceConfidence;
		ar << p_BodyOverlap << m_BodyMinDist << p_ObstacleOverlap;
		ar << p_number << p_new << p_delete << p_distance << p_height << p_width;
		ar << p_color << p_background << p_foreground << p_abnormalW << p_abnormalH;
		ar << p_add << p_remove << p_exchange << p_update << p_diffuse;
		ar << m_saturationTime << m_maxHistChange << m_maxPosChange << m_maxDeletedAge << m_maxDeletedDist;

	}
	else {
		ar >> m_heightM >> m_heightS >> m_widthM >> m_widthS;
		ar >> m_minHeight >> m_maxHeight >> m_minWidth >> m_maxWidth >> m_iterations >> m_iterationsPerBody >> m_randomness;
		ar >> m_posModelNoise >> m_posMeasurementNoise >> m_orientModelNoise >> m_orientMeasurementNoise;
		ar >> m_maxDoorDistance >> m_invisibleFor >> m_outsiderFor >> m_distanceConfidence;
		ar >> p_BodyOverlap >> m_BodyMinDist >> p_ObstacleOverlap;
		ar >> p_number >> p_new >> p_delete >> p_distance >> p_height >> p_width;
		ar >> p_color >> p_background >> p_foreground >> p_abnormalW >> p_abnormalH;
		ar >> p_add >> p_remove >> p_exchange >> p_update >> p_diffuse;
		ar >> m_saturationTime >> m_maxHistChange >> m_maxPosChange >> m_maxDeletedAge >> m_maxDeletedDist;
	}
}
// --------------------------------------------------------------------------

