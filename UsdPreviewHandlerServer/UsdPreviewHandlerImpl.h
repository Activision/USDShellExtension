#pragma once

#include "USDPreviewLocalServer_h.h"
#include "USDPreviewThread.h"
#include "UsdPreviewHandlerPython\UsdPreviewHandlerEvent.h"

#include "resource.h"

class CPreviewHandlerFrameInfo : public PREVIEWHANDLERFRAMEINFO
{
public:
	CPreviewHandlerFrameInfo()
	{
		haccel = nullptr;
		cAccelEntries = 0;
	}

	~CPreviewHandlerFrameInfo()
	{
		Term();
	}

	void Term()
	{
		if ( haccel )
		{
			DestroyAcceleratorTable( haccel );
			haccel = nullptr;
		}
	}
};

class ATL_NO_VTABLE CUsdPreviewHandlerImpl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUsdPreviewHandlerImpl, &CLSID_USDPreviewHandler>,
	public IInitializeWithFile,
	public IObjectWithSite,
	public IOleWindow,
	public IPreviewHandler,
	public IPreviewHandlerVisuals
{
public:
	DECLARE_NOT_AGGREGATABLE(CUsdPreviewHandlerImpl)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CUsdPreviewHandlerImpl)
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
	bool PushEvent(eUsdPreviewEvent event, intptr_t data1 = 0, intptr_t data2 = 0);

	HMODULE m_hUsdPreviewLib = nullptr;
	FNUSDPREVIEWPUSHEVENT m_fnUsdPreviewPushEvent = nullptr;
	FNUSDPREVIEWGETPREVIEWWINDOW m_fnUsdGetPreviewWindow = nullptr;
	CUSDPreviewThread m_PreviewThread;

	CStringW m_usdStagePath;

	CComPtr<IUnknown> m_pSite;
	CComPtr<IPreviewHandlerFrame> m_pPreviewHandlerFrame;

	HWND m_hWndParent = nullptr;
	RECT m_rcWindow = {};
	CPreviewHandlerFrameInfo m_PreviewHandlerFrameInfo;

	COLORREF m_clrBackground;
	COLORREF m_clrText;
};

OBJECT_ENTRY_AUTO(__uuidof(USDPreviewHandler), CUsdPreviewHandlerImpl)

