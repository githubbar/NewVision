#if !defined( BODYACTIVITYDIALOG_H )
#define BODYACTIVITYDIALOG_H

#pragma once
#include "BodyActivityModel.h"
#include "Resource.h"

class BodyActivityDialog : public CPropertyPage, public BodyActivityModel
{
	enum { IDD = IDD_BODYACTIVITYDIALOG };
public:
    BodyActivityDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~BodyActivityDialog();
	bool Initialize();
	void DeInitialize();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void Serialize(CArchive& ar);
	DECLARE_MESSAGE_MAP()
};
#endif // !defined( BODYACTIVITYDIALOG_H )