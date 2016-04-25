#if !defined( SWARMACTIVITYDIALOG_H )
#define SWARMACTIVITYDIALOG_H

#pragma once
#include "SwarmActivityModel.h"
#include "Resource.h"
#include "afxcmn.h"
#include "TabCtrlSSL.h"

class SwarmActivityDialog : public CPropertyPage, public SwarmActivityModel
{
	enum { IDD = IDD_SWARMACTIVITYDIALOG };

		DECLARE_MESSAGE_MAP()
public:
	CTabCtrlSSL m_eventParameters;

    SwarmActivityDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~SwarmActivityDialog();
	bool Initialize();
	void DeInitialize();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnInitDialog();
	void		 DetectActivities();
	afx_msg void OnBnClickedProximity();
	afx_msg void OnBnClickedGrouping();
	afx_msg void OnBnClickedLeaderfollowing();
	afx_msg void OnBnClickedLiningup();
	afx_msg void OnBnClickedCodwelling();
};


#endif // !defined( SWARMACTIVITYDIALOG_H )