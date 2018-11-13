// MFCHostWPFDlg.h : header file
//

#pragma once

#include <vcclr.h>
#include <string.h>

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Documents;
using namespace System::Threading;
using namespace System::Windows::Controls;
using namespace System::Windows::Media;
using namespace System::Windows::Media::Animation;

//RECT rect;
HWND GetHwnd(HWND parent, int x, int y, int width, int height);
void RefreshWPFControl();




// CMFCHostWPFDlg dialog
class CMFCHostWPFDlg : public CDialog
{
// Construction
public:
	CMFCHostWPFDlg(CWnd* pParent = NULL);	// standard constructor
	void DateAdd(CTime *pDateTime, long lMinute);
	void DateMinus(CTime *pDateTime, long lMinute);
	long GetGMTOffSet();

	CStatic m_grpDate;
	CStatic m_grpTime;
	CString m_editHour;
	CString m_editMinute;
	CString m_editSecond;
	CTime m_calDate;

// Dialog Data
	enum { IDD = IDD_MFCHOSTWPF_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBtnApply();
};
