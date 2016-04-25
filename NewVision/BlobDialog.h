#if !defined( BLOBDIALOG_H )
#define BLOBDIALOG_H
#pragma once

#include "BlobModel.h"
#include "Resource.h"

// BlobDialog dialog

class BlobDialog : public CPropertyPage, public BlobModel
{
	DECLARE_MESSAGE_MAP()
	enum { IDD = IDD_BLOBDIALOG };
	
public:

	BlobDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~BlobDialog();
	bool Initialize();
	void DeInitialize();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void Serialize(CArchive& ar);
};

#endif // !defined( BLOBDIALOG_H )