#ifndef __PROGRESSDLG__
#define __PROGRESSDLG__

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

class CProgressDlg : public CDialog
{
// Construction
public:
	void SetPos(int nPos);
	void SetMessage(LPCTSTR msg);
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProgressDlg)
	enum { IDD = IDD_PROGRESS };
	CProgressCtrl	m_progress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgressDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //__PROGRESSDLG__

