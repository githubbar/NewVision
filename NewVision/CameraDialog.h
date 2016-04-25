#if !defined( CAMERADIALOG_H )
#define CAMERADIALOG_H

#pragma once

#include "cv.h" 
#include "highgui.h" 
#include "cameramodel.h"

// ---------- CNumberListBox class -------------------
class CNumberListBox : public CListBox {
	public:
	void Add(int number);
	void RemoveCurrent();
	void Clear();
	void UpdateData(BOOL bSaveAndValidate = TRUE);
};

// ---------- CCalibrateControl class -------------------
class CCalibrateControl : public CButton {
	DECLARE_MESSAGE_MAP()
public:
	static const int DOT = 2;
	CImage image;
	CPoint* m_dragPoint; 
	int m_dragLine;
	CArray<CLine>* m_dragArray;
	CPoint* m_createPoint; 
	bool m_dragging;
	bool m_creating;
	CCalibrateControl();
	bool Initialize(CNewVisionDoc* doc);
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void DrawLines(CDC* pDC, CArray<CLine>& x, CPen* pen);
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void OnButtonDown(UINT nFlags, CPoint point);
	void OnButtonUp(UINT nFlags, CPoint point);
	void afx_msg OnMouseMove(UINT nFlags, CPoint point);
	bool ChangeLine(CArray<CLine>& x, CPoint point);

};

// ---------------------------------------------------
// CameraDialog dialog
class CameraDialog : public CPropertyPage, public CameraModel
{
	enum { IDD = IDD_CALIBRATEDIALOG };
	DECLARE_MESSAGE_MAP()
public:
	CCalibrateControl m_image;
	CNumberListBox m_heights;
	CEdit m_currentHeight;
	CScrollBar m_vbar;
	CScrollBar m_hbar;
	int offsetx, offsety;

	CameraDialog(CWnd* pParent = NULL);
	virtual ~CameraDialog();
	bool Initialize();
	void DeInitialize();
	void UpdateControls();
	void FindVanishingPoint(CArray<CLine>& x, CvMat* v, int w, int h);
	virtual BOOL OnSetActive();
	virtual void Serialize(CArchive& ar);
	virtual void DoDataExchange(CDataExchange* pDX);   
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonDeleteAll();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCameraMono();
	afx_msg void OnBnClickedCameraPano();
	afx_msg void OnBnClickedXMLFile();
	afx_msg void OnBnClickedButtonBrowseXmlFile();
};
#endif // !defined( CAMERADIALOG_H )