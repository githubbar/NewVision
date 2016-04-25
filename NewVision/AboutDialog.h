#if !defined( ABOUTDIALOG_H )
#define ABOUTDIALOG_H
#pragma once
#include "HyperLink.h"

// AboutDialog dialog
class AboutDialog : public CDialog
{
	DECLARE_DYNAMIC(AboutDialog)

public:
	CHyperLink	m_HyperLink;
	AboutDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~AboutDialog();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
};
#endif // !defined( ABOUTDIALOG_H )