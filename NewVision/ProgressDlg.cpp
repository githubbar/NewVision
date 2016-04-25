// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

// ------------------------------------------------------------------------
CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ------------------------------------------------------------------------
void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDlg)
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_progress);
	//}}AFX_DATA_MAP
}

// ------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CProgressDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg message handlers
// ------------------------------------------------------------------------
BOOL CProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CenterWindow();
	
	m_progress.SetRange( 0, 100 );
	m_progress.SetPos( 0 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
// ------------------------------------------------------------------------
void CProgressDlg::SetPos(int nPos)
{
	m_progress.SetPos(nPos);
}
// ------------------------------------------------------------------------
void CProgressDlg::SetMessage(LPCTSTR msg) {
	this->GetDlgItem(IDC_MESSAGE)->SetWindowText(msg);
}