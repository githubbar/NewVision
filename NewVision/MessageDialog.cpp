#include "stdafx.h"
#include "afxtempl.h"
#include "NewVisionDoc.h"
#include "MessageDialog.h"
#include "Globals.h"

BEGIN_MESSAGE_MAP(MessageDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTONMESSAGEREMOVE, OnBnClickedButtonMessageRemove)
	ON_BN_CLICKED(IDC_SAVE_FILTER_GRAPH, OnBnClickedSaveFilterGraph)
END_MESSAGE_MAP()

// --------------------------------------------------------------------------
MessageDialog::MessageDialog(CWnd* pParent /*=NULL*/)
	: CPropertyPage(MessageDialog::IDD), m_list()
{
}
// --------------------------------------------------------------------------
MessageDialog::~MessageDialog()
{
}
// --------------------------------------------------------------------------
bool MessageDialog::Initialize() {
	if (m_list) {
		m_list.DeleteAllItems();
	}
	return true;
}
// --------------------------------------------------------------------------
void MessageDialog::Clear() {
	if (m_list) {
		m_list.DeleteAllItems();
	}
}
// --------------------------------------------------------------------------
void MessageDialog::DeInitialize() {
	if (m_list) {
		m_list.DeleteAllItems();
	}
}
// --------------------------------------------------------------------------
BOOL MessageDialog::OnInitDialog( ) {
	CPropertyPage::OnInitDialog();
	m_list.SetBkColor(RGB(255,255,200));
	m_list.SetTextBkColor(RGB(255,255,200));
	m_list.InsertColumn(0, "Message text", LVCFMT_LEFT, 400);
	return true;
}
// --------------------------------------------------------------------------
BOOL MessageDialog::OnSetActive() {
	CPropertyPage::OnSetActive();
	return true;
}
// --------------------------------------------------------------------------
BOOL MessageDialog::OnKillActive() {
	CPropertyPage::OnSetActive();
	return true;
}
// --------------------------------------------------------------------------
void MessageDialog::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_MESSAGES, m_list);
}
// --------------------------------------------------------------------------
void MessageDialog::OnBnClickedButtonMessageRemove()
{
	m_list.DeleteAllItems();
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void MessageDialog::OnBnClickedSaveFilterGraph() {
	char szFilters[] = "DirectShow Filter Graph (*.grf)|*.grf|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, ".grf", "Filter Graph 01", OFN_OVERWRITEPROMPT |OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, szFilters, NULL);
	char title[]= {"Save DirectShow Filter Graph ..."};
	dlg.m_ofn.lpstrTitle = title;
	if (dlg.DoModal() == IDOK) {
		CString filename = dlg.GetPathName();
		doc->trackermodel.SaveGraphFile(filename);
	}	
}