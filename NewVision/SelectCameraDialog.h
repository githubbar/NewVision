#pragma once
#include "afxwin.h"


// SelectCameraDialog dialog

class SelectCameraDialog : public CDialog
{
	DECLARE_DYNAMIC(SelectCameraDialog)

public:
	SelectCameraDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~SelectCameraDialog();

// Dialog Data
	enum { IDD = IDD_SELECTCAMERA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL SelectCameraDialog::OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CArray<CString> m_names;
	int m_choice;
};
