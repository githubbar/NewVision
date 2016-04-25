#if !defined( MESSAGE_DIALOG_H )
#define MESSAGE_DIALOG_H

#pragma once
#include "MessageModel.h"
#include "Resource.h"
#include "afxcmn.h"


// ------------------- MessageDialog class ------------------
class MessageDialog : public CPropertyPage, public MessageModel
{
	DECLARE_MESSAGE_MAP()
	enum { IDD = IDD_MESSAGEDIALOG};

public:
	CListCtrl         m_list;

	MessageDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~MessageDialog();
	bool Initialize();
	void DeInitialize();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog( );
	void	Clear();
	afx_msg void OnBnClickedButtonMessageRemove();
	afx_msg void OnBnClickedSaveFilterGraph();
};
#endif // !defined( MESSAGE_DIALOG_H )