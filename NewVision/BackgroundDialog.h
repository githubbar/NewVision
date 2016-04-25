#if !defined( BACKGROUNDDIALOG_H )
#define BACKGROUNDDIALOG_H
#pragma once
#include "BackgroundModel.h"
#include "Resource.h"

class BackgroundDialog : public CPropertyPage, public BackgroundModel {
	enum { IDD = IDD_BACKGROUNDDIALOG };
	DECLARE_MESSAGE_MAP()

public:
	BackgroundDialog(CWnd* pParent = NULL);   
	virtual ~BackgroundDialog();
	bool Initialize();
	void DeInitialize();
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedRadioLearning();
	afx_msg void OnBnClickedRadioDynamic();
	void UpdateControls();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void Serialize( CArchive& ar ) {BackgroundModel::Serialize(ar);};
};
#endif // !defined( BACKGROUNDDIALOG_H )