// MyShellView.h : Declaration of the CMyShellView

#pragma once
#include "resource.h"       // main symbols


#include <shlobj.h>
#include "ShellFolderDemo_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CMyShellView

class ATL_NO_VTABLE CMyShellView :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMyShellView, &CLSID_MyShellView>,
	public IShellView,
	public IDispatchImpl<IMyShellView, &IID_IMyShellView, &LIBID_ShellFolderDemoLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CMyShellView()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_MYSHELLVIEW)


BEGIN_COM_MAP(CMyShellView)
	COM_INTERFACE_ENTRY(IMyShellView)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IShellView)
	COM_INTERFACE_ENTRY(IOleWindow)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	//IOleWindow methods
	STDMETHOD(GetWindow) (HWND*);
	STDMETHOD(ContextSensitiveHelp) (BOOL);

	//IShellView methods
	STDMETHOD(TranslateAccelerator) (LPMSG);
	STDMETHOD(EnableModeless) (BOOL);
	STDMETHOD(UIActivate) (UINT);
	STDMETHOD(Refresh) (VOID);
	STDMETHOD(CreateViewWindow) (LPSHELLVIEW, LPCFOLDERSETTINGS, LPSHELLBROWSER, LPRECT, HWND*);
	STDMETHOD(DestroyViewWindow) (VOID);
	STDMETHOD(GetCurrentInfo) (LPFOLDERSETTINGS);
	STDMETHOD(AddPropertySheetPages) (DWORD, LPFNADDPROPSHEETPAGE, LPARAM);
	STDMETHOD(SaveViewState) (VOID);
	STDMETHOD(SelectItem) (LPCITEMIDLIST, UINT);
	STDMETHOD(GetItemObject) (UINT, REFIID, LPVOID*);

public:


private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

	HWND m_hWnd;
};

OBJECT_ENTRY_AUTO(__uuidof(MyShellView), CMyShellView)
