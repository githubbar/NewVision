#if !defined( TRACKERDIALOG_H )
#define TRACKERDIALOG_H

#pragma once
#include "TrackerModel.h"
#include "highgui.h" 
#include "Resource.h"
#include "atltime.h"

// TrackerDialog dialog

class TrackerDialog : public CPropertyPage, public TrackerModel
{
	DECLARE_MESSAGE_MAP()
	enum { IDD = IDD_TRACKER};

public:
	TrackerDialog();
	//TrackerDialog(CString, int, int, double);
	virtual ~TrackerDialog();
	bool Initialize();
	void DeInitialize();

	virtual void Serialize(CArchive& ar);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedSourceSelectButton();
	afx_msg void OnBnClickedSourceSelectButtonT();
};
#endif // !defined( TRACKERDIALOG_H )