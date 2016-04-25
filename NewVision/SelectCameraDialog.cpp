// SelectCameraDialog.cpp : implementation file
//
#include "stdafx.h"
#include "afxtempl.h"
#include "NewVision.h"
#include "filters.h"
#include "SelectCameraDialog.h"


// SelectCameraDialog dialog

IMPLEMENT_DYNAMIC(SelectCameraDialog, CDialog)
SelectCameraDialog::SelectCameraDialog(CWnd* pParent /*=NULL*/)
	: CDialog(SelectCameraDialog::IDD, pParent)
{
}

SelectCameraDialog::~SelectCameraDialog()
{
}

BOOL SelectCameraDialog::OnInitDialog() {
	CArray<CLSID> ids;
//	m_combo.Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST, CRect(10,10,200,100), this, IDC_COMBOCAMERANAME);
	enumFilters(CLSID_VideoInputDeviceCategory, m_names, ids);
	CComboBox* combo = (CComboBox*)GetDlgItem(IDC_COMBOCAMERANAME);
	for (int i=0; i<m_names.GetCount();i++)
		combo->AddString(m_names[i]);
	return true;
}

void SelectCameraDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if (pDX->m_bSaveAndValidate)
		m_choice = ((CComboBox*)GetDlgItem(IDC_COMBOCAMERANAME))->GetCurSel();
}


BEGIN_MESSAGE_MAP(SelectCameraDialog, CDialog)
END_MESSAGE_MAP()


// SelectCameraDialog message handlers
