#include "stdafx.h"
#include "EditableTooltip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CEditableTooltip, CEdit)

BEGIN_MESSAGE_MAP(CEditableTooltip, CEdit)
    //{{AFX_MSG_MAP(CEditableTooltip)
    ON_WM_KEYDOWN()
    ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_CREATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ---------------------------------------------------------------------
CEditableTooltip::CEditableTooltip() {
}
// ---------------------------------------------------------------------
CEditableTooltip::CEditableTooltip(CWnd* pParent, CRect r, CString text) {
	const FAKE_ID = 4000;
	this->pParent = pParent;
	this->text = text;
	this->r = r;
	Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE, r, pParent, FAKE_ID);
}
// ---------------------------------------------------------------------
int CEditableTooltip::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;	
	SetWindowText(text);
	SetFocus();
	return 0;
}
// ---------------------------------------------------------------------
CEditableTooltip::~CEditableTooltip() {
}
// ---------------------------------------------------------------------
int CEditableTooltip::DoModal() {
	MSG msg;
	BOOL bRet;
	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
	{	
		if (bRet == -1)
        {
            // handle the error and possibly exit
        }
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);		
			if (msg.message == WM_EDITABLETOOLTIPCLOSE)
				break; 
		}
	}	
	return retvalue; 
}
// ---------------------------------------------------------------------
void CEditableTooltip::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar) {
		case VK_RETURN: 
				retvalue = IDOK;
				PostMessage(WM_EDITABLETOOLTIPCLOSE);	
				return;
		case VK_ESCAPE: 
				retvalue = IDCANCEL;
				PostMessage(WM_EDITABLETOOLTIPCLOSE);
				return;
	}
    CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
// ---------------------------------------------------------------------
void CEditableTooltip::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if(nChar == VK_RETURN)
        return; 
    CEdit::OnChar(nChar, nRepCnt, nFlags);
}
// ---------------------------------------------------------------------
void CEditableTooltip::OnKillFocus(CWnd* pNewWnd) {
	CWnd::OnKillFocus(pNewWnd);
	retvalue = IDCANCEL;
	PostMessage(WM_EDITABLETOOLTIPCLOSE);
}
// ---------------------------------------------------------------------
