#if !defined( MAINFRM_H )
#define MAINFRM_H 
#pragma once
#include "TrueColorToolbar.h"
#include "Resource.h"
#include "VisualFx.h"

//struct AFX_SIZEPARENTPARAMS {};

#define MEM_INFO_STATUS_POS 1
#define FRAME_NUMBER_STATUS_POS 2
#define WM_UPDATESTATUSBAR	(WM_APP + 1)
#define WM_UPDATENAVBAR		(WM_APP + 2)
#define WM_UPDATEACTIVEVIEW (WM_APP + 3)
#define WM_GRAPHNOTIFY		(WM_APP + 4)
#define WM_LOADLASTFRAME	(WM_APP + 5)

class CTrackToolBar : public CDialogBar {
public:
	OAFilterState state;
	BOOL bStartOfScroll;
	CTrackToolBar();
	enum { IDD = IDD_TRACKTOOLBAR};
	DECLARE_MESSAGE_MAP()
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};

class CBlendToolBar : public CDialogBar {
public:
	enum { IDD = IDD_BLENDTOOLBAR};
	DECLARE_MESSAGE_MAP()
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};

class CZoomToolBar : public CDialogBar {
public:
	enum { IDD = IDD_ZOOMTOOLBAR};
	DECLARE_MESSAGE_MAP()
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};

extern CTrackToolBar      m_wndTrackToolBar;
extern CBlendToolBar      m_wndBlendToolBar;
extern CDialogBar         m_wndControlToolBar;
extern CZoomToolBar       m_wndZoomToolBar;
extern CReBar             m_wndPreferencesToolBar;
extern CTrueColorToolBar  m_wndToolBar;
extern CMenu		      m_displayMenu;
extern CMenu		      m_bodyMenu;
extern CMenu		      m_settingsMenu;
extern CStatusBar         m_wndStatusBar;

class CNewVisionView;

class CMainFrame : public CFrameWnd
{
	
protected: 
	DECLARE_DYNCREATE(CMainFrame)
	DECLARE_MESSAGE_MAP()
	CMainFrame();

public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
	CSplitterWnd		m_splitter; // CSplitterWnd or CTabCtrl
	TVisualFramework	imgFramework;
	TVisualObject		*view[4];

	CNewVisionView* GetActivePane();
	CNewVisionView* GetPane(int index);
	void			DockControlBarLeftOf(CControlBar* Bar, CControlBar* LeftOf);
	afx_msg LRESULT OnUpdateStatusBar(UINT wParam, LONG lParam);
	afx_msg LRESULT OnUpdateNavigationBar(UINT wParam, LONG lParam);
	afx_msg LRESULT OnUpdateActiveView(UINT wParam, LONG lParam);
	afx_msg LRESULT OnGraphNotify ( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnLoadLastFrame ( WPARAM wParam, LPARAM lParam );
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL	OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg void	OnDestroy();
	afx_msg void	OnHelpAbout();

	};


#endif // !defined( MAINFRM_H )