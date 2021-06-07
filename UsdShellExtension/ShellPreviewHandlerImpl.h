// Copyright 2021 Activision Publishing, Inc. 
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "ShellExt_h.h"

#include "UsdLoadScreenDlg.h"
#include "resource.h"


class ATL_NO_VTABLE CShellPreviewHandlerImpl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CShellPreviewHandlerImpl, &CLSID_ShellPreviewHandler>,
	public IInitializeWithFile,
	public IObjectWithSite,
	public IOleWindow,
	public IPreviewHandler,
	public IPreviewHandlerVisuals
{
public:
	DECLARE_NOT_AGGREGATABLE(CShellPreviewHandlerImpl)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CShellPreviewHandlerImpl)
		COM_INTERFACE_ENTRY(IInitializeWithFile)
		COM_INTERFACE_ENTRY(IObjectWithSite)
		COM_INTERFACE_ENTRY(IOleWindow)
		COM_INTERFACE_ENTRY(IPreviewHandler)
		COM_INTERFACE_ENTRY(IPreviewHandlerVisuals)
	END_COM_MAP()

	HRESULT FinalConstruct();
	void FinalRelease();

	// IInitializeWithFile
	STDMETHODIMP Initialize( __RPC__in_string LPCWSTR pszFilePath, DWORD grfMode ) override;

	// IObjectWithSite
	STDMETHODIMP SetSite(__RPC__in_opt IUnknown* pUnkSite) override;
	STDMETHODIMP GetSite(__RPC__in REFIID riid, __RPC__deref_out_opt void** ppvSite) override;

	// IOleWindow
	STDMETHODIMP GetWindow(__RPC__deref_out_opt HWND* phwnd) override;
	STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode) override;

	// IPreviewHandler
	STDMETHODIMP SetWindow( __RPC__in HWND hwnd, __RPC__in const RECT* prc) override;
	STDMETHODIMP SetRect( __RPC__in const RECT* prc ) override;
	STDMETHODIMP DoPreview() override;
	STDMETHODIMP Unload() override;
	STDMETHODIMP SetFocus() override;
	STDMETHODIMP QueryFocus( __RPC__deref_out_opt HWND* phwnd) override;
	STDMETHODIMP TranslateAccelerator(__RPC__in MSG* pmsg) override;

	// IPreviewHandlerVisuals
    STDMETHODIMP SetBackgroundColor(COLORREF color) override;
    STDMETHODIMP SetFont(__RPC__in const LOGFONTW *plf) override;
    STDMETHODIMP SetTextColor(COLORREF color) override;

	static HRESULT WINAPI UpdateRegistry(_In_ BOOL bRegister) throw();

private:
	HRESULT CreateLocalServer();
	void DestroyLocalServer();

	CComPtr<IInitializeWithFile> m_pInitializeWithFile;
	CComPtr<IObjectWithSite> m_pObjectWithSite;
	CComPtr<IOleWindow> m_pOleWindow;
	CComPtr<IPreviewHandler> m_pPreviewHandler;
	CComPtr<IPreviewHandlerVisuals> m_pPreviewHandlerVisuals;

	HWND m_hWnd = nullptr;
	// raw dpi-unaware rect
	RECT m_rcWndRaw = {};
	// window rect adjusted for dpi
	RECT m_rcWndDpiAware = {};

	COLORREF m_clrBackground;
	COLORREF m_clrText;

	CUsdLoadScreenDlg m_LoadScreenDlg;
	bool m_bLoadScreenCreated = false;
};

OBJECT_ENTRY_AUTO(__uuidof(ShellPreviewHandler), CShellPreviewHandlerImpl)

