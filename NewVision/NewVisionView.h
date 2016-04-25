#if !defined( NEWVISIONVIEW_H )
#define NEWVISIONVIEW_H 
#pragma once
#include "cv.h"      
#include "highgui.h" 
#include "ZoomView.h"

enum ViewType {VIDEO_VIEW=0, SCENE_VIEW=1, ACTIVITY_VIEW=2, FLOOR_VIEW=3};

//class COleDataObject {};
class CNewVisionDoc;

class CNewVisionView : public CZoomView
{
protected: // create from serialization only
	DECLARE_DYNCREATE(CNewVisionView)

// Attributes
public:
	CNewVisionDoc* GetDocument() const;


	IplImage*	image;
	CRect		r;
	bool		m_drag, m_rotate, m_rect, m_click;
	CvPoint		m_click_point;
	CPoint		m_cClickPoint;
	bool		m_wasRunning;
// Display parameters
public:
	int			m_type;        // view type 
	int			m_match;     // ID of the body being dragged
	int			m_match2;    // ID of the second body
	bool		m_fullScreen; // full screen mode indicator
	CWnd		*m_savedParent; // saved parent window

// Overrides
	public:
	//virtual BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	void		 ShowImage(CDC* pDC, IplImage* pIplImage);
			void UpdateVideoPosition();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	afx_msg void OnViewFullscreen();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnContextMenu(CWnd* pWnd, CPoint pos);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
    virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
// Implementation
public:
				 CNewVisionView();
				 CNewVisionView(ViewType);
	virtual		 ~CNewVisionView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in NewVisionView.cpp
inline CNewVisionDoc* CNewVisionView::GetDocument() const
   { return reinterpret_cast<CNewVisionDoc*>(m_pDocument); }
#endif

#endif // !defined( NEWVISIONVIEW_H )