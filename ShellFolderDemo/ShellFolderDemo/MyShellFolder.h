// MyShellFolder.h : Declaration of the CMyShellFolder

#pragma once
#include "resource.h"       // main symbols


#include <shlobj.h>
#include "ShellFolderDemo_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CMyShellFolder

class ATL_NO_VTABLE CMyShellFolder :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMyShellFolder, &CLSID_MyShellFolder>,
	public IPersistFolder,
	public IShellFolder,
	public IDispatchImpl<IMyShellFolder, &IID_IMyShellFolder, &LIBID_ShellFolderDemoLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CMyShellFolder()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_MYSHELLFOLDER)


BEGIN_COM_MAP(CMyShellFolder)
	COM_INTERFACE_ENTRY(IMyShellFolder)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IPersistFolder)
	COM_INTERFACE_ENTRY(IShellFolder)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	//IShellFolder methods
	STDMETHOD(ParseDisplayName) (HWND, LPBC, LPOLESTR, LPDWORD, LPITEMIDLIST*, LPDWORD);
	STDMETHOD(EnumObjects) (HWND, DWORD, LPENUMIDLIST*);
	STDMETHOD(BindToObject) (LPCITEMIDLIST, LPBC, REFIID, LPVOID*);
	STDMETHOD(BindToStorage) (LPCITEMIDLIST, LPBC, REFIID, LPVOID*);
	STDMETHOD(CompareIDs) (LPARAM, LPCITEMIDLIST, LPCITEMIDLIST);
	STDMETHOD(CreateViewObject) (HWND, REFIID, LPVOID*);
	STDMETHOD(GetAttributesOf) (UINT, LPCITEMIDLIST*, LPDWORD);
	STDMETHOD(GetUIObjectOf) (HWND, UINT, LPCITEMIDLIST*, REFIID, LPUINT, LPVOID*);
	STDMETHOD(GetDisplayNameOf) (LPCITEMIDLIST, DWORD, LPSTRRET);
	STDMETHOD(SetNameOf) (HWND, LPCITEMIDLIST, LPCOLESTR, DWORD, LPITEMIDLIST*);

	//IPersist methods
	STDMETHOD(GetClassID)(LPCLSID);

	//IPersistFolder methods
	STDMETHOD(Initialize)(LPCITEMIDLIST);


};

OBJECT_ENTRY_AUTO(__uuidof(MyShellFolder), CMyShellFolder)
