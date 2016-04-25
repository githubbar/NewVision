// RecordDialog.cpp : implementation file
//
#include "stdafx.h"
#include "NewVision.h"
#include "RecordDialog.h"


// RecordDialog dialog

IMPLEMENT_DYNAMIC(RecordDialog, CDialog)
RecordDialog::RecordDialog(CWnd* pParent /*=NULL*/)
	: CDialog(RecordDialog::IDD, pParent)
{
}

RecordDialog::~RecordDialog()
{
}

void RecordDialog::DoDataExchange(CDataExchange* pDX)
{
	DDX_Check(pDX, IDC_RECORD_INPUT_VIEW, m_recordVideoView);
	DDX_Check(pDX, IDC_RECORD_OUTPUT_VIEW, m_recordSceneView);
	DDX_Check(pDX, IDC_RECORD_ACTIVITY_VIEW, m_recordActivityView);
	DDX_Check(pDX, IDC_RECORD_FLOOR_VIEW, m_recordFloorView);

	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(RecordDialog, CDialog)
END_MESSAGE_MAP()


// RecordDialog message handlers
