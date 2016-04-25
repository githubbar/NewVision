#if !defined( RECORDDIALOG_H )
#define RECORDDIALOG_H  
#pragma once


// RecordDialog dialog

class RecordDialog : public CDialog
{
	DECLARE_DYNAMIC(RecordDialog)

public:
	RecordDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~RecordDialog();

// Dialog Data
	enum { IDD = IDD_RECORD_DIALOG };
	int m_recordVideoView, m_recordSceneView, m_recordActivityView, m_recordFloorView;     

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
#endif // !defined( RECORDDIALOG_H )