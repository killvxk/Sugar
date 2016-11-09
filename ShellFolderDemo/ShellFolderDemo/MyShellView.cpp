// MyShellView.cpp : Implementation of CMyShellView

#include "stdafx.h"
#include "MyShellView.h"

extern HINSTANCE   g_hInst;
// CMyShellView

STDMETHODIMP CMyShellView::TranslateAccelerator(LPMSG pmsg)
{
	return S_FALSE;
}

STDMETHODIMP CMyShellView::EnableModeless(BOOL fEnable)
{
	return E_NOTIMPL;
}

STDMETHODIMP CMyShellView::UIActivate(UINT uState)
{
	return S_OK;
}

STDMETHODIMP CMyShellView::Refresh(VOID)
{
	return S_OK;
}

STDMETHODIMP CMyShellView::CreateViewWindow(LPSHELLVIEW pPrevView, LPCFOLDERSETTINGS lpfs, LPSHELLBROWSER psb, LPRECT prcView, HWND *phWnd)
{
	WNDCLASS wc;
	*phWnd = NULL;

	HWND hwndParent;
	//get our parent window
	psb->GetWindow(&hwndParent);
	m_hWnd = *phWnd = CreateWindowEx(0,
		L"Static",
		NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
		prcView->left,
		prcView->top,
		prcView->right - prcView->left,
		prcView->bottom - prcView->top,
		hwndParent,
		NULL,
		g_hInst,
		(LPVOID)this);
	if (!*phWnd)
		return E_FAIL;

	return S_OK;
}

LRESULT CALLBACK CMyShellView::WndProc(HWND hWnd,
	UINT uMessage,
	WPARAM wParam,
	LPARAM lParam)
{
	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

STDMETHODIMP CMyShellView::DestroyViewWindow(VOID)
{
	return S_OK;
}

STDMETHODIMP CMyShellView::GetCurrentInfo(LPFOLDERSETTINGS lpfs)
{
	return S_OK;
}

STDMETHODIMP CMyShellView::AddPropertySheetPages(DWORD dwReserved,
	LPFNADDPROPSHEETPAGE lpfn,
	LPARAM lParam)
{
	return E_NOTIMPL;
}

STDMETHODIMP CMyShellView::SaveViewState(VOID)
{
	return S_OK;
}

STDMETHODIMP CMyShellView::SelectItem(LPCITEMIDLIST pidlItem, UINT uFlags)
{
	return E_NOTIMPL;
}

STDMETHODIMP CMyShellView::GetItemObject(UINT uItem, REFIID riid, LPVOID *ppvOut)
{
	*ppvOut = NULL;

	return E_NOTIMPL;
}

STDMETHODIMP CMyShellView::GetWindow(HWND *phWnd)
{
	*phWnd = m_hWnd;

	return S_OK;
}

STDMETHODIMP CMyShellView::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}