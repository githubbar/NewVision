#if !defined( BODYDIALOG_H )
#define BODYDIALOG_H

#pragma once
#include "BodyModel.h"
#include "TabCtrlSSL.h"

class BodyDialog : public CPropertyPage, public BodyModel
{
	enum { IDD = IDD_BODYDIALOG, IDD0 = IDDS_BODYPRIORSTAB, IDD1 = IDDS_BODYTRACKINGTAB,  };
public:
	CTabCtrlSSL m_bodyParameters;

    BodyDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~BodyDialog();
	bool Initialize();
	void DeInitialize();
	BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void Serialize(CArchive& ar);

	class BodyPriorsTab : public CTabPageSSL {
		public:
		CString name;
		BodyDialog* mama;
		virtual void DoDataExchange(CDataExchange* pDX); 
		BodyPriorsTab() {CTabPageSSL (IDD0, NULL); this->name = "Priors";};
	};

	class BodyTrackingTab : public CTabPageSSL {
		public:
		CString name;
		BodyDialog* mama;
		virtual void DoDataExchange(CDataExchange* pDX); 
		BodyTrackingTab() {CTabPageSSL (IDD1, NULL); this->name = "Tracking";};
	};

	BodyPriorsTab m_bodyPriorsTab;
	BodyTrackingTab m_bodyTrackingTab;
};


#endif // !defined( BODYDIALOG_H )