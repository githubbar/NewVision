#if !defined(TEXTENTRYDLG_H)
#define TEXTENTRYDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextEntryDlg1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTextEntryDlg window

class CTextEntryDlg : protected CWnd
{
// Construction
public:
	CTextEntryDlg();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextEntryDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	LPCTSTR GetText();
	int Show(CWnd *pParent, LPCTSTR pszTitle, LPCTSTR pszPrompt, LPCTSTR pszDefault = _T(""), bool bPassword = false);
	virtual ~CTextEntryDlg();

protected:
	int DoModal(CWnd *pParent);
	
	CString m_strText;

	// controls
	CEdit m_ctlEdit;
	CFont m_Font;
	CStatic m_ctlLabel;
	CButton m_ctlCancel;
	CButton m_ctlOK;
	
	// Generated message map functions
	//{{AFX_MSG(CTextEntryDlg)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg void OnCancel();
	afx_msg void OnOK();
	afx_msg void OnEditboxChanged();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined( TEXTENTRYDLG_H )
