#if !defined(ZOOMVIEW_H)
#define ZOOMVIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ZoomView.h : header file
//


class CZoomView : public CScrollView
{
protected:
	CZoomView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CZoomView)

// Attributes
public:
	// Flicker free attributes
	BOOL		m_bInitialSize;

public:
	// Zoom attributes
	float		m_zoomFactor;
	float		m_minZoomFactor;
	float		m_maxZoomFactor;
	CSize		m_line;
	CSize		m_page;
	CSize		m_layout;
	CSize		m_displaySize;

// Operations
public:
	// Display Operation /////////////////////////////////////////////////////
	void SetDisplaySize(void);
	void SetZoomScale(float fScale);
	void SetZoomArea(CRect zoomRect);
	CPoint GetCenterScrollPosition(void);
	CSize GetDisplaySize()	{return m_displaySize;};	
	//////////////////////////////////////////////////////////////////////////
	CPoint ClientToImage(CPoint p);
	CPoint ImageToClient(CPoint p);
	int FloatToInt(float fNumber);

protected:
	void SetZoomFactor(float fZoomFactor);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZoomView)
	protected:
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnDraw(CDC* pDC) {}  // overridden to draw this view
	virtual ~CZoomView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CZoomView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(ZOOMVIEW_H)
