#include "stdafx.h"
#include "NewVisionDoc.h"
#include "SkinDialog.h"
#include "Globals.h"

BEGIN_MESSAGE_MAP(SkinDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTONCLEAR, OnBnClickedButtonClear)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CSkinControl, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// --------------------------------------------------------------------------
SkinDialog::SkinDialog(CWnd* pParent /*=NULL*/)
	: CPropertyPage(SkinDialog::IDD)
{
}
// --------------------------------------------------------------------------
SkinDialog::~SkinDialog()
{
}
// --------------------------------------------------------------------------
bool SkinDialog::Initialize() {
	if (!m_image.Initialize(doc))
		return false;
	return true;
}
// --------------------------------------------------------------------------
void SkinDialog::DeInitialize() {
	m_skinType = SKIN_2D_HISTOGRAM;
	color.Clear();
}
// --------------------------------------------------------------------------
BOOL SkinDialog::OnSetActive() {
	CPropertyPage::OnSetActive();
	return m_image.Initialize(doc);
}
// --------------------------------------------------------------------------
void SkinDialog::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_SKINIMAGE, m_image);
	DDX_Control(pDX, IDC_HISTOGRAMIMAGE, m_histogram);
	DDX_Control(pDX, IDC_COLORIMAGE, m_color);
	DDX_Control(pDX, IDC_SKIN_SCROLLBAR1, m_vbar);
	DDX_Control(pDX, IDC_SKIN_SCROLLBAR2, m_hbar);
	DDX_Radio(pDX, IDC_SKIN_2DHISTORGAM, m_skinType);
}
// --------------------------------------------------------------------------
void SkinDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int curpos = pScrollBar->GetScrollPos();
	int minpos;
	int maxpos;
	pScrollBar->GetScrollRange(&minpos, &maxpos);
	SCROLLINFO   info;
    pScrollBar->GetScrollInfo(&info, SIF_ALL);
	switch (nSBCode)
    {
		case SB_TOP:      
			curpos = minpos;
			break;
		case SB_BOTTOM:     
			curpos = maxpos;
			break;
		case SB_ENDSCROLL: 
			break;
		case SB_LINEUP:      
			if (curpos > minpos)
				curpos--;
			break;
		case SB_LINEDOWN:   
			if (curpos < maxpos)
				curpos++;
			break;
		case SB_PAGEUP:    
			curpos = max(minpos, curpos - (int) info.nPage);
			break;
		case SB_PAGEDOWN:
			curpos = min(maxpos, curpos + (int) info.nPage);
			break;
		case SB_THUMBPOSITION: 
			curpos = nPos;     
			break;
		case SB_THUMBTRACK:
			curpos = nPos;     
			break;
    }    
	offsety = curpos;
	pScrollBar->SetScrollPos(curpos);
	m_image.Invalidate();
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
// --------------------------------------------------------------------------
void SkinDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int curpos = pScrollBar->GetScrollPos();
	int minpos;
	int maxpos;
	pScrollBar->GetScrollRange(&minpos, &maxpos);
	SCROLLINFO   info;
    pScrollBar->GetScrollInfo(&info, SIF_ALL);
	switch (nSBCode)
    {
		case SB_LEFT:      
			curpos = minpos;
			break;
		case SB_RIGHT:     
			curpos = maxpos;
			break;
		case SB_ENDSCROLL: 
			break;
		case SB_LINELEFT:      
			if (curpos > minpos)
				curpos--;
			break;
		case SB_LINERIGHT:   
			if (curpos < maxpos)
				curpos++;
			break;
		case SB_PAGELEFT:    
			curpos = max(minpos, curpos - (int) info.nPage);
			break;
		case SB_PAGERIGHT:
			curpos = min(maxpos, curpos + (int) info.nPage);
			break;
		case SB_THUMBPOSITION: 
			curpos = nPos;     
			break;
		case SB_THUMBTRACK:
			curpos = nPos;     
			break;
    }    
	offsetx = curpos; 
	pScrollBar->SetScrollPos(curpos);
	m_image.Invalidate();
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
// --------------------------------------------------------------------------
void SkinDialog::OnBnClickedButtonClear()
{
	color.Clear();
	Invalidate();
	UpdateWindow();
}
// --------------------------------------------------------------------------
void SkinDialog::Serialize( CArchive& ar )
 {
	SkinModel::Serialize( ar );
 }

//==========================================================================
// ------------------ CSkinControl class -----------------------------------
 CSkinControl::CSkinControl() {
}
// --------------------------------------------------------------------------
bool CSkinControl::Initialize(CNewVisionDoc* doc) {
	// Get first frame from file or camera
	image.CopyOf(imgFrame);
	// Show scroll bars
	if (m_hWnd) {
		SkinDialog* tab = (SkinDialog*)GetParent();
		CRect r; this->GetWindowRect(&r);
		SCROLLINFO horz;
		horz.cbSize = sizeof(SCROLLINFO);
		horz.fMask = SIF_ALL;
		horz.nPage = 50;
		horz.nMin = 0;
		horz.nMax = image.Width()-r.Width()+horz.nPage;
		horz.nPos = 0;
		horz.nTrackPos = 0;
		tab->m_hbar.SetScrollInfo(&horz);
		SCROLLINFO vert;
		vert.cbSize = sizeof(SCROLLINFO);
		vert.fMask = SIF_ALL;
		vert.nPage = 50;
		vert.nMin = 0;
		vert.nMax = image.Height()-r.Height()+vert.nPage;
		vert.nPos = 0;
		vert.nTrackPos = 0;
		tab->m_vbar.SetScrollInfo(&vert);
		tab->m_vbar.ShowWindow(true);  //Hide Vertical Scroll Bar
		tab->m_hbar.ShowWindow(true);  //Hide Horizontal Scroll Bar
		tab->offsetx = 0; tab->offsety = 0;
		this->Invalidate();
	}
	return true;
}
// --------------------------------------------------------------------------
void CSkinControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);
	SkinDialog* tab = ((SkinDialog*)this->GetParent());
	CPoint off = CPoint(tab->m_hbar.GetScrollPos(), tab->m_vbar.GetScrollPos());
	Pixel p;
	Image::getPixel(image.GetImage(), &p, point.x+off.x, point.y+off.y);
	tab->AddValue(cvScalar(p.B, p.G, p.R));
	tab->Invalidate();
	tab->UpdateWindow();
}
// --------------------------------------------------------------------------
void CSkinControl::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);
	SkinDialog* tab = ((SkinDialog*)this->GetParent());
	CPoint off = CPoint(tab->m_hbar.GetScrollPos(), tab->m_vbar.GetScrollPos());
	Pixel p;
	Image::getPixel(image.GetImage(), &p, point.x+off.x, point.y+off.y);
	tab->m_color.color = RGB(p.R, p.G, p.B);
	tab->m_color.Invalidate();
	tab->m_color.UpdateWindow();
}
// --------------------------------------------------------------------------
void CSkinControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	SkinDialog* dlg=((SkinDialog*)this->GetParent());
	image.Show(pDC->m_hDC, 0, 0, image.Width(), image.Height(), dlg->offsetx, dlg->offsety);
}

//==========================================================================
// ------------------ CHistogramControl class -----------------------------------
CHistogramControl::CHistogramControl() {
}
 // --------------------------------------------------------------------------
void CHistogramControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	SkinDialog* dlg=((SkinDialog*)this->GetParent());
	dlg->Draw(pDC);
}
//==========================================================================
// ------------------ CColorControl class -----------------------------------
CColorControl::CColorControl() {
	color = RGB(0, 0, 0);
};
// --------------------------------------------------------------------------
void CColorControl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect box;	pDC->GetWindow()->GetWindowRect(&box);
	CBrush fill;   
	fill.CreateSolidBrush(color);   
	CRect rect(0, 0, box.Width(), box.Height());
	pDC->FillRect(&rect, &fill);
}
// --------------------------------------------------------------------------