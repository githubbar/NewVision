#include "stdafx.h"
#include "SwarmActivityDialog.h"
#include "NewVisionDoc.h"
#include "swarmactivitydialog.h"
#using <mscorlib.dll>
#using <System.dll>
using namespace System::Diagnostics;
// --------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(SwarmActivityDialog, CPropertyPage)
END_MESSAGE_MAP()

// SwarmActivityDialog dialog
// --------------------------------------------------------------------------
SwarmActivityDialog::SwarmActivityDialog(CWnd* pParent /*=NULL*/)
	: CPropertyPage(SwarmActivityDialog::IDD)
{
}
// --------------------------------------------------------------------------
SwarmActivityDialog::~SwarmActivityDialog()
{
}
// --------------------------------------------------------------------------
BOOL SwarmActivityDialog::OnInitDialog() {
	if (!CPropertyPage::OnInitDialog())
		return false;
	for (int i=0; i< N_EVENT_TYPES;i++) {
		// add dialog to a tab
		 m_eventDetector[i]->Create(m_eventDetector[i]->IDD, this);
		 m_eventParameters.AddSSLPage(_T(m_eventDetector[i]->name), i, m_eventDetector[i]);
	}
	return true;
}
// --------------------------------------------------------------------------
bool SwarmActivityDialog::Initialize() {
	if (!SwarmActivityModel::Initialize())
		return false;
	m_eventCheckIDC[0] = IDC_PROXIMITY;	
	m_eventCheckIDC[1] = IDC_GROUPING;
	m_eventCheckIDC[2] = IDC_LEADERFOLLOWING;
	m_eventCheckIDC[3] = IDC_LININGUP;
	m_eventCheckIDC[4] = IDC_CODWELLING;
	m_initialized = true;
	// TEMP:
	m_eventType[1] = 1;
	return true;
}
// --------------------------------------------------------------------------
void SwarmActivityDialog::DeInitialize() {
	SwarmActivityModel::DeInitialize();
	for (int i=0; i< N_EVENT_TYPES;i++) {
		m_eventType[i] = 0;
	}
}
// --------------------------------------------------------------------------
void SwarmActivityDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	for (int i=0; i< N_EVENT_TYPES;i++) {
		DDX_Check(pDX, m_eventCheckIDC[i], m_eventType[i]);
	}
	
	DDX_Control(pDX, IDC_SWARMACTIVITYTAB, m_eventParameters);

	int n = m_eventParameters.GetSSLActivePage();
	if (n >= 0)
		m_eventDetector[n]->UpdateData(pDX->m_bSaveAndValidate);
}
// --------------------------------------------------------------------------
void SwarmActivityDialog::Serialize( CArchive& ar ) {
	CObject::Serialize(ar);
	for (int i=0; i< N_EVENT_TYPES;i++) {
		m_eventDetector[i]->Serialize(ar);
		ar.IsStoring() ? ar << m_eventType[i] : ar >> m_eventType[i];
	}
}
// --------------------------------------------------------------------------
void SwarmActivityDialog::DetectActivities()
{
	int acount = 0;
	for (int i=0;i<N_EVENT_TYPES;i++)
		if (m_eventType[i]) {
			acount += m_eventDetector[i]->PostProcessActivities(doc);
		}
}

