// ZoomView.cpp : implementation file
//

#include "stdafx.h"
#include "ZoomView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZoomView

IMPLEMENT_DYNCREATE(CZoomView, CScrollView)

CZoomView::CZoomView()
{
	m_bInitialSize = FALSE;
	m_zoomFactor = 1.0;
	m_minZoomFactor = 0.33f;
	m_maxZoomFactor = 3.0f;
}

CZoomView::~CZoomView()
{
}


BEGIN_MESSAGE_MAP(CZoomView, CScrollView)
	//{{AFX_MSG_MAP(CZoomView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZoomView drawing

void CZoomView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// CZoomView diagnostics


#ifdef _DEBUG
void CZoomView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CZoomView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
void CZoomView::SetDisplaySize(void)
{
	m_displaySize.cx = FloatToInt(m_layout.cx * m_zoomFactor);
	m_displaySize.cy = FloatToInt(m_layout.cy * m_zoomFactor);

	//if (m_displaySize.cx % 2 != 0)
	//	m_displaySize.cx+=1;
	//if (m_displaySize.cy % 2 != 0)
	//	m_displaySize.cy+=1;

	CRect rectMax(CPoint(0,0),m_displaySize);
	SetScrollSizes(MM_TEXT,m_displaySize,m_page,m_line);
	//SetScrollSizes(MM_TEXT,m_displaySize);
}

void CZoomView::SetZoomScale(float fScale)
{
	float oldZoomFactor = m_zoomFactor;
	CPoint centerScrollPosition = GetCenterScrollPosition();

	SetZoomFactor(fScale);
	SetDisplaySize();

	int newXScrollPosition = FloatToInt((centerScrollPosition.x / oldZoomFactor) * m_zoomFactor);
	int newYScrollPosition = FloatToInt((centerScrollPosition.y / oldZoomFactor) * m_zoomFactor);
	CRect clientRect(0,0,0,0);
	GetClientRect(&clientRect);
	newXScrollPosition -= clientRect.right/2;
	newYScrollPosition -= clientRect.bottom/2;
	ScrollToPosition(CPoint(newXScrollPosition,newYScrollPosition));
	Invalidate(FALSE);
}

void CZoomView::SetZoomArea(CRect zoomRect)
{
	CRect clientRect(0,0,0,0);
	GetClientRect(clientRect);

	CRect realRect(zoomRect);
//	LPtoDP(&realRect);

	float widthRatio = float(clientRect.Width()) / float(realRect.Width());
	float heightRatio = float(clientRect.Height()) / float(realRect.Height());
	float ratio = (widthRatio < heightRatio) ? widthRatio : heightRatio;
	ratio = ratio * m_zoomFactor;

	CPoint centerScrollPosition = CPoint(0,0);
	centerScrollPosition.x = zoomRect.left + zoomRect.Width() / 2;
	centerScrollPosition.y = zoomRect.top + zoomRect.Height() / 2;
	
	SetZoomFactor(ratio);
	SetDisplaySize();

	int newXScrollPosition = FloatToInt(centerScrollPosition.x  * m_zoomFactor);
	int newYScrollPosition = FloatToInt(centerScrollPosition.y  * m_zoomFactor);
	
	GetClientRect(clientRect);
	newXScrollPosition -= clientRect.Width()/2;
	newYScrollPosition -= clientRect.Height()/2;
	ScrollToPosition(CPoint(newXScrollPosition,newYScrollPosition));

	Invalidate(FALSE);
}

CPoint CZoomView::GetCenterScrollPosition(void)
{
	CPoint ScrollPosition = GetScrollPosition();
	CRect ClientRect(0,0,0,0);
	GetClientRect(&ClientRect);
	ScrollPosition.x += ClientRect.right/2;
	ScrollPosition.y += ClientRect.bottom/2;
	return ScrollPosition;
}

int CZoomView::FloatToInt(float fNumber)
{
	if (fNumber >= 0)
	{
		fNumber += 0.5f;
	}
	else
	{
		fNumber -= 0.5f;
	}
	return int(fNumber);
}

void CZoomView::SetZoomFactor(float fZoomFactor)
{
	if (fZoomFactor <= m_minZoomFactor) 
		m_zoomFactor = m_minZoomFactor;
	else if (fZoomFactor >= m_maxZoomFactor)
		m_zoomFactor = m_maxZoomFactor;
	else
		m_zoomFactor = fZoomFactor;
}

CPoint CZoomView::ClientToImage( CPoint p )
{
	CPoint img;
	
	img.x = FloatToInt(1.0/m_zoomFactor*(p.x + this->GetScrollPosition().x));
	img.y = FloatToInt(1.0/m_zoomFactor*(p.y + this->GetScrollPosition().y));		
	return img;
}

CPoint CZoomView::ImageToClient( CPoint p )
{
	CPoint client;
	client.x = FloatToInt(m_zoomFactor*p.x - this->GetScrollPosition().x);
	client.y = FloatToInt(m_zoomFactor*p.y - this->GetScrollPosition().y);	
	return client;
}