// MFCHostWPFDlg.cpp : implementation file
//

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Controls;
using namespace System::Windows::Media;
using namespace System::Runtime;
using namespace System::Runtime::InteropServices;
using namespace System::Windows::Interop;

#include "stdafx.h"
#include "MFCHostWPF.h"
#include "MFCHostWPFDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////
//////////////////////WPF Implementation//////////////////////

ref class Globals
{ 
public:
	static System::Windows::Interop::HwndSource^ gHwndSource;
	static WPFControls::AnimClock^ gwcClock;
};

HWND hwndWPF; //The hwnd associated with the hosted WPF page

HWND GetHwnd(HWND parent, int x, int y, int width, int height)
{
    System::Windows::Interop::HwndSourceParameters^ sourceParams = gcnew System::Windows::Interop::HwndSourceParameters ("MFCWPFApp");
	sourceParams->PositionX = x;
	sourceParams->PositionY = y;
	sourceParams->Height = height;
	sourceParams->Width = width;
	sourceParams->ParentWindow = IntPtr(parent);
	sourceParams->WindowStyle = WS_VISIBLE | WS_CHILD;
	Globals::gHwndSource = gcnew System::Windows::Interop::HwndSource(*sourceParams);

	DateTime tm = DateTime::Now;
	Globals::gwcClock = gcnew WPFControls::AnimClock();
	Globals::gwcClock->ChangeDateTime(tm.Year,tm.Month,tm.Day,tm.Hour,tm.Minute,tm.Second);
	FrameworkElement^ myPage = Globals::gwcClock;

	Globals::gHwndSource->RootVisual = myPage;
	return (HWND) Globals::gHwndSource->Handle.ToPointer();
}

void RefreshWPFControl()
{  
	FrameworkElement^ page;
	DateTime tm = DateTime::Now;

	Globals::gwcClock->ChangeDateTime(tm.Year,tm.Month,tm.Day,tm.Hour,tm.Minute,tm.Second);
	page = Globals::gwcClock;

	Globals::gHwndSource->RootVisual = page;

	return;
}



///////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMFCHostWPFDlg dialog




CMFCHostWPFDlg::CMFCHostWPFDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMFCHostWPFDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCHostWPFDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRP_DATE, m_grpDate);
	DDX_Control(pDX, IDC_GRP_TIME, m_grpTime);
	DDX_Text(pDX, IDC_EDIT_HOUR, m_editHour);
	DDX_Text(pDX, IDC_EDIT_MINUTE, m_editMinute);
	DDX_Text(pDX, IDC_EDIT_SECOND, m_editSecond);
	DDX_MonthCalCtrl(pDX, IDC_CALENDAR, m_calDate);
}

BEGIN_MESSAGE_MAP(CMFCHostWPFDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_APPLY, &CMFCHostWPFDlg::OnBtnApply)
END_MESSAGE_MAP()


// CMFCHostWPFDlg message handlers

int CMFCHostWPFDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	hwndWPF = GetHwnd(this->GetSafeHwnd(), 20, 28, 205, 130);
	
	return 0;
}

void CMFCHostWPFDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent==0) {
		SYSTEMTIME tm;
		GetSystemTime(&tm);
		CTime timeNow(tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);
		DateAdd(&timeNow, GetGMTOffSet());
		timeNow.GetAsSystemTime(tm);

		CString sTmp;
		sTmp.Format(L"%02d", tm.wSecond);
		GetDlgItem(IDC_EDIT_SECOND)->SetWindowTextW(sTmp);

		//update minute
		if (tm.wSecond==0) {
			sTmp.Format(L"%02d", tm.wMinute);
			GetDlgItem(IDC_EDIT_MINUTE)->SetWindowTextW(sTmp);
			//update hour
			if (tm.wMinute==0) {
				sTmp.Format(L"%02d", tm.wHour);
				GetDlgItem(IDC_EDIT_HOUR)->SetWindowTextW(sTmp);
			}
		}
	}
}

BOOL CMFCHostWPFDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	SYSTEMTIME tm;
	GetSystemTime(&tm);
	CTime timeNow(tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);
	DateAdd(&timeNow, GetGMTOffSet());
	timeNow.GetAsSystemTime(tm);

	m_calDate = CTime(tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond);
	m_editHour.Format(L"%02d", tm.wHour); 
	m_editMinute.Format(L"%02d", tm.wMinute);
	m_editSecond.Format(L"%02d", tm.wSecond);

	UpdateData(false);

	SetTimer(0, 1000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCHostWPFDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CMFCHostWPFDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CMFCHostWPFDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCHostWPFDlg::OnBtnApply()
{
	UpdateData(true);

	SYSTEMTIME tm;
	GetSystemTime(&tm);

	tm.wYear = (WORD)m_calDate.GetYear();
	tm.wMonth = (WORD)m_calDate.GetMonth();
	tm.wDay = (WORD)m_calDate.GetDay();
	tm.wHour = (WORD)_wtoi(m_editHour) - (GetGMTOffSet()/60);
	tm.wMinute = (WORD)_wtoi(m_editMinute) - (GetGMTOffSet()%60);
	tm.wSecond = (WORD)_wtoi(m_editSecond);

	SetSystemTime(&tm);

	RefreshWPFControl();
}



////////////////////DateTime Functions//////////////////////////

void CMFCHostWPFDlg::DateAdd(CTime *pDateTime, long lMinute)
{
	DWORD dwDay=0,dwHour=0,dwMin=0,dwSec=0;

	dwMin = lMinute ;
	CTimeSpan time_dif(dwDay,dwHour,dwMin,dwSec);// minute differences
	*pDateTime +=time_dif;
}

void CMFCHostWPFDlg::DateMinus(CTime *pDateTime, long lMinute)
{
	DWORD dwDay=0,dwHour=0,dwMin=0,dwSec=0;

	dwMin = lMinute ;
	CTimeSpan time_dif(dwDay,dwHour,dwMin,dwSec);// minute differences
	*pDateTime -= time_dif;
}

//All translations between UTC and local time are as: UTC = local time + bias 
long CMFCHostWPFDlg::GetGMTOffSet()
{
	TIME_ZONE_INFORMATION TimeZoneInfo;
	GetTimeZoneInformation(&TimeZoneInfo);
	CString sTimeZoneDesc = TimeZoneInfo.StandardName;
	GetDlgItem(IDC_ST_TIMEZONE)->SetWindowTextW(L"Time Zone: " + sTimeZoneDesc);
	return -(TimeZoneInfo.Bias);
}
