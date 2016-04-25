#include "stdafx.h"
#include "Resource.h"
#include "AboutDialog.h"
#include "res/version.h"

#using <System.dll>
using namespace System::Reflection;

// AboutDialog dialog

IMPLEMENT_DYNAMIC(AboutDialog, CDialog)
AboutDialog::AboutDialog(CWnd* pParent /*=NULL*/)
	: CDialog(AboutDialog::IDD, pParent)
{
}
// --------------------------------------------------------------------------
AboutDialog::~AboutDialog()
{
}
// --------------------------------------------------------------------------
void AboutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HYPERLINK, m_HyperLink);
}
// --------------------------------------------------------------------------
BOOL AboutDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	//CString name = "NewVision Version " + (CString)(Assembly::GetExecutingAssembly()->GetName()->Version->ToString());
	CString tmp;
	tmp = "Version " + STR(VERSION_MAJOR) + "." + STR(VERSION_MINOR) + "." + STR(VERSION_BUILD) + "." + STR(VERSION_QFE);
	this->GetDlgItem(IDC_NEWVISIONVERSION)->SetWindowTextA(tmp);
	tmp = "Built on " + VERSION_BUILD_DATE_TIME;
	this->GetDlgItem(IDC_NEWVISIONBUILDDATETIME)->SetWindowTextA(tmp);
			
	m_HyperLink.SetURL(_T("http://www.kelley.iu.edu/Marketing/Research/page10554.html"));
	m_HyperLink.SetUnderline(FALSE);
//	m_HyperLink.SetLinkCursor(AfxGetApp()->LoadCursor(IDC_HAND));
	return TRUE;  
}
// --------------------------------------------------------------------------

// AboutDialog message handlers
