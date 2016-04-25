#include "stdafx.h"
#include "NewVision.h"
#include "BlobDialog.h"
#include "TrackerModel.h"
#include "Globals.h"
#include "BlobDialog.h"

// BlobDialog dialog
BEGIN_MESSAGE_MAP(BlobDialog, CDialog)
END_MESSAGE_MAP()

// --------------------------------------------------------------------------
BlobDialog::BlobDialog(CWnd* pParent /*=NULL*/)
	: CPropertyPage(BlobDialog::IDD)
{
}
// --------------------------------------------------------------------------
BlobDialog::~BlobDialog()
{
}
// --------------------------------------------------------------------------
bool BlobDialog::Initialize() {
	if (!BlobModel::Initialize())
		return false;
	return true;
}
// --------------------------------------------------------------------------
void BlobDialog::DeInitialize() {
	BlobModel::DeInitialize();
	m_minArea = 0.001;
	m_maxArea = 0.3;
	m_maxBodies = 3;
	m_minRaySize = 0.60;
}
// --------------------------------------------------------------------------
void BlobDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MINIMUMAREA, m_minArea);
	DDX_Text(pDX, IDC_MAXIMUMAREA, m_maxArea);
	DDX_Text(pDX, IDC_MAXBODIES, m_maxBodies);
	DDX_Text(pDX, IDC_MINRAYSIZE, m_minRaySize);


	DDV_MinMaxDouble(pDX, m_minArea, 0.00001, 1);
	DDV_MinMaxDouble(pDX, m_maxArea, 0.00001, 1);
	DDV_MinMaxInt(pDX, m_maxBodies, 1, 10);
	DDV_MinMaxDouble(pDX, m_minRaySize, 0.1, 1.0);
}
// --------------------------------------------------------------------------
void BlobDialog::Serialize( CArchive& ar )
 {
	BlobModel::Serialize( ar );
	if( ar.IsStoring()) {
		ar << m_minArea << m_maxArea << m_maxBodies << m_minRaySize;
	}
	else {
		ar >> m_minArea >> m_maxArea >> m_maxBodies >> m_minRaySize;
	}
 }