// MyShellFolder.cpp : Implementation of CMyShellFolder

#include "stdafx.h"
#include "MyShellFolder.h"
#include "MyShellView.h"

// CMyShellFolder
STDMETHODIMP CMyShellFolder::ParseDisplayName(HWND hwndOwner,
	LPBC pbcReserved,
	LPOLESTR lpDisplayName,
	LPDWORD pdwEaten,
	LPITEMIDLIST *pPidlNew,
	LPDWORD pdwAttributes)
{
	return E_NOTIMPL;
}

STDMETHODIMP CMyShellFolder::EnumObjects(HWND hwndOwner,
	DWORD dwFlags,
	LPENUMIDLIST *ppEnumIDList)
{
	return S_OK;
}

STDMETHODIMP CMyShellFolder::BindToObject(LPCITEMIDLIST pidl,
	LPBC pbcReserved,
	REFIID riid,
	LPVOID *ppvOut)
{
	*ppvOut = NULL;
	//Make sure the item is a folder.
	ULONG ulAttribs = SFGAO_FOLDER;
	this->GetAttributesOf(1, &pidl, &ulAttribs);
	if (!(ulAttribs & SFGAO_FOLDER))
		return E_INVALIDARG;
	CComObject<CMyShellFolder> *pShellFolder;
	CComObject<CMyShellFolder>::CreateInstance(&pShellFolder);
	pShellFolder->AddRef();
	HRESULT  hr = pShellFolder->QueryInterface(riid, ppvOut);
	pShellFolder->Release();
	return hr;
}

STDMETHODIMP CMyShellFolder::BindToStorage(LPCITEMIDLIST pidl,
	LPBC pbcReserved,
	REFIID riid,
	LPVOID *ppvOut)
{
	*ppvOut = NULL;

	return E_NOTIMPL;
}

STDMETHODIMP CMyShellFolder::CompareIDs(LPARAM lParam,
	LPCITEMIDLIST pidl1,
	LPCITEMIDLIST pidl2)
{
	HRESULT        hr = E_FAIL;
	LPITEMIDLIST   pidlTemp1;
	LPITEMIDLIST   pidlTemp2;

	//the items are the same
	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}

STDMETHODIMP CMyShellFolder::CreateViewObject(HWND hwndOwner, REFIID riid, LPVOID *ppvOut)
{
	HRESULT  hr = E_NOINTERFACE;
	if (IsEqualIID(riid, IID_IShellView))
	{
		IMyShellView  *pShellView;
		*ppvOut = NULL;
		CMyShellView::CreateInstance(&pShellView);
		if (!pShellView)
			return E_OUTOFMEMORY;
		pShellView->AddRef();
		hr = pShellView->QueryInterface(riid, ppvOut);
		pShellView->Release();
	}

	return hr;
}

STDMETHODIMP CMyShellFolder::GetAttributesOf(UINT uCount, LPCITEMIDLIST aPidls[], LPDWORD pdwAttribs)
{
	return S_OK;
}

STDMETHODIMP CMyShellFolder::GetUIObjectOf(HWND hwndOwner, UINT uCount, LPCITEMIDLIST *pPidls, REFIID riid, LPUINT puReserved, LPVOID *ppvOut)
{
	*ppvOut = NULL;
	return S_OK;
}

STDMETHODIMP CMyShellFolder::GetDisplayNameOf(LPCITEMIDLIST pidl,
	DWORD dwFlags,
	LPSTRRET lpName)
{
	return S_OK;
}

STDMETHODIMP CMyShellFolder::SetNameOf(HWND hwndOwner,
	LPCITEMIDLIST pidl,
	LPCOLESTR lpName,
	DWORD dwFlags,
	LPITEMIDLIST *ppidlOut)
{
	if (!pidl)
		return E_INVALIDARG;

	return S_OK;
}

STDMETHODIMP CMyShellFolder::GetClassID(LPCLSID lpClassID)
{
	*lpClassID = CLSID_MyShellFolder;
	return S_OK;
}

STDMETHODIMP CMyShellFolder::Initialize(LPCITEMIDLIST pidlFQ)
{
	return S_OK;
}