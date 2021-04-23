#include "stdafx.h"
#include "USDPreviewHandlerImpl.h"
#include "Module.h"
#include "shared\PythonUtil.h"
#include "shared\emb.h"
#include "shared\environment.h"
#include "shared\EventViewerLog.h"

HRESULT CUsdPreviewHandlerImpl::FinalConstruct()
{
	SetupPythonEnvironment();

	m_hUsdPreviewLib = LoadLibraryEx(_T("UsdPreviewHandler.pyd"), nullptr, LOAD_LIBRARY_SEARCH_APPLICATION_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (m_hUsdPreviewLib == nullptr)
		return E_FAIL;

	m_fnUsdPreviewPushEvent = reinterpret_cast<FNUSDPREVIEWPUSHEVENT>(GetProcAddress(m_hUsdPreviewLib, "UsdPreviewPushEvent"));
	if (m_fnUsdPreviewPushEvent == nullptr)
		return E_FAIL;

	m_fnUsdGetPreviewWindow = reinterpret_cast<FNUSDPREVIEWGETPREVIEWWINDOW>(GetProcAddress(m_hUsdPreviewLib, "UsdPreviewGetPreviewWindow"));
	if (m_fnUsdGetPreviewWindow == nullptr)
		return E_FAIL;

	if ( !m_PreviewThread.Init() )
		return E_FAIL;

	m_clrBackground = ::GetSysColor( COLOR_WINDOW );
	m_clrText = ::GetSysColor( COLOR_WINDOWTEXT );

	return __super::FinalConstruct();
}

void CUsdPreviewHandlerImpl::FinalRelease()
{
	m_PreviewThread.Term();

	if (m_hUsdPreviewLib)
	{
		FreeLibrary(m_hUsdPreviewLib);
		m_hUsdPreviewLib = nullptr;
		m_fnUsdPreviewPushEvent = nullptr;
		m_fnUsdGetPreviewWindow = nullptr;
	}

	__super::FinalRelease();
}

// IInitializeWithFile
STDMETHODIMP CUsdPreviewHandlerImpl::Initialize(__RPC__in_string LPCWSTR pszFilePath, DWORD grfMode)
{
	UNREFERENCED_PARAMETER(grfMode);

	m_usdStagePath = pszFilePath;

	return S_OK;
}

// IObjectWithSite
STDMETHODIMP CUsdPreviewHandlerImpl::SetSite(__RPC__in_opt IUnknown* pUnkSite)
{
	m_pSite = nullptr;
	m_pPreviewHandlerFrame = nullptr;
	m_PreviewHandlerFrameInfo.Term();

	if (pUnkSite != nullptr)
	{
		m_pSite = pUnkSite;
		CComQIPtr<IPreviewHandlerFrame> pIPreviewHandlerFrame(m_pSite);
		m_pPreviewHandlerFrame = pIPreviewHandlerFrame;
		if ( m_pPreviewHandlerFrame )
			m_pPreviewHandlerFrame->GetWindowContext( &m_PreviewHandlerFrameInfo );
	}

	return S_OK;
}

STDMETHODIMP CUsdPreviewHandlerImpl::GetSite(__RPC__in REFIID riid, __RPC__deref_out_opt void** ppvSite)
{
	if (ppvSite == nullptr)
		return E_POINTER;

	if (m_pSite == nullptr)
	{
		*ppvSite = nullptr;
		return E_FAIL;
	}

	HRESULT hr;
	hr = m_pSite->QueryInterface(riid, ppvSite);

	return hr;
}

// IOleWindow
STDMETHODIMP CUsdPreviewHandlerImpl::GetWindow(__RPC__deref_out_opt HWND* phwnd)
{
	if (phwnd == nullptr)
		return E_POINTER;

	*phwnd = m_fnUsdGetPreviewWindow();
	if ( *phwnd == nullptr )
		return E_FAIL;

	return S_OK;
}

STDMETHODIMP CUsdPreviewHandlerImpl::ContextSensitiveHelp(BOOL fEnterMode)
{
	UNREFERENCED_PARAMETER(fEnterMode);
	return E_NOTIMPL;
}

// IPreviewHandler
STDMETHODIMP CUsdPreviewHandlerImpl::SetWindow(__RPC__in HWND hwnd, __RPC__in const RECT* prc)
{
	if (prc == nullptr)
		return E_POINTER;

	m_hWndParent = hwnd;
	m_rcWindow = *prc;

	PushEvent( USDPREVIEWEVENT_SETWINDOW, reinterpret_cast<intptr_t>(m_hWndParent), reinterpret_cast<intptr_t>(&m_rcWindow) );

	return S_OK;
}

STDMETHODIMP CUsdPreviewHandlerImpl::SetRect(__RPC__in const RECT* prc)
{
	if (prc == nullptr)
		return E_POINTER;

	m_rcWindow = *prc;

	PushEvent( USDPREVIEWEVENT_RESIZERECT, reinterpret_cast<intptr_t>(&m_rcWindow) );

	return S_OK;
}

STDMETHODIMP CUsdPreviewHandlerImpl::DoPreview()
{
	HRESULT hr;
	hr = m_PreviewThread.LaunchPreviewWindow( reinterpret_cast<HWND>(m_hWndParent), m_usdStagePath );
	if (FAILED(hr))
		return hr;

	return S_OK;
}

STDMETHODIMP CUsdPreviewHandlerImpl::Unload()
{
	// push the shutdown event to Python
	PushEvent( USDPREVIEWEVENT_QUIT );

	m_usdStagePath.Empty();
	m_pSite = nullptr;
	m_pPreviewHandlerFrame = nullptr;
	m_PreviewHandlerFrameInfo.Term();
	m_hWndParent = nullptr;
	m_rcWindow = {};

	return S_OK;
}

STDMETHODIMP CUsdPreviewHandlerImpl::SetFocus()
{
	return S_OK;
}

STDMETHODIMP CUsdPreviewHandlerImpl::QueryFocus(__RPC__deref_out_opt HWND* phwnd)
{
	if ( phwnd == nullptr )
		return E_POINTER;

	*phwnd = nullptr;
	return S_OK;
}

STDMETHODIMP CUsdPreviewHandlerImpl::TranslateAccelerator(__RPC__in MSG* pmsg)
{
	if (pmsg == nullptr)
		return E_POINTER;

	if ( m_pPreviewHandlerFrame && IsAccelerator( m_PreviewHandlerFrameInfo.haccel, m_PreviewHandlerFrameInfo.cAccelEntries, pmsg, nullptr ) )
	{
		return m_pPreviewHandlerFrame->TranslateAccelerator(pmsg);
	}

	return S_FALSE;
}

STDMETHODIMP CUsdPreviewHandlerImpl::SetBackgroundColor( COLORREF color )
{
	m_clrBackground = color;
	return S_OK;
}

STDMETHODIMP CUsdPreviewHandlerImpl::SetFont( __RPC__in const LOGFONTW *plf )
{
	UNREFERENCED_PARAMETER( plf );
	return S_OK;
}

STDMETHODIMP CUsdPreviewHandlerImpl::SetTextColor( COLORREF color )
{
	m_clrText = color;
	return S_OK;
}


bool CUsdPreviewHandlerImpl::PushEvent(eUsdPreviewEvent event, intptr_t data1, intptr_t data2)
{
	// push the event to Python
	if (m_fnUsdPreviewPushEvent == nullptr)
		return false;

	m_fnUsdPreviewPushEvent(event, data1, data2);

	return true;
}

HRESULT WINAPI CUsdPreviewHandlerImpl::UpdateRegistry(_In_ BOOL bRegister) throw()
{
	ATL::_ATL_REGMAP_ENTRY regMapEntries[] =
	{
		{ L"APPID", L"{067AAB34-285D-40D1-92EF-A5ED449EFEEE}" },
		{ L"TYPELIB", L"{68DF0FAC-996B-4933-B092-9733E77061BB}" },
		{ L"CLSID_USDPREVIEWHANDLER", L"{07D4D38F-9C98-48F8-A057-26241405D5C8}" },
		{ nullptr, nullptr }
	};

	return g_AtlModule.UpdateRegistryFromResource(IDR_REGISTRY_USDPREVIEWHANDLERIMPL, bRegister, regMapEntries);
}