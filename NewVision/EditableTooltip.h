#pragma once

#define WM_EDITABLETOOLTIPCLOSE WM_USER+1

class CEditableTooltip : public CEdit
{
	DECLARE_DYNCREATE(CEditableTooltip)

	CEditableTooltip();
	CEditableTooltip(CWnd* pParent, CRect r, CString text);
    virtual ~CEditableTooltip();
	int DoModal();

protected:
    //{{AFX_MSG(CEditableTooltip)
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:		
	CWnd* pParent;
	CString text;
	CRect r;
	int retvalue;
};



