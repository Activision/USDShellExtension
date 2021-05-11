#include "stdafx.h"
#include "ShellPreviewHandlerImpl.h"
#include "Module.h"
#include "resource.h"

#pragma warning(push)
#pragma warning(disable:4192 4278)
#import "UsdPreviewLocalServer.tlb" raw_interfaces_only 
#pragma warning(pop)

static RECT UpdateRectForDPI(HWND hwnd, const RECT& rcWnd)
{
#if 0
	int iDpiProcess = static_cast<int>(::GetSystemDpiForProcess( ::GetCurrentProcess() ));
	int iDpiWindow = static_cast<int>(::GetDpiForWindow( hwnd ));

	RECT rcDpi;
	rcDpi.left = MulDiv(rcWnd.left, iDpiProcess, iDpiWindow); 
	rcDpi.top = MulDiv(rcWnd.top, iDpiProcess, iDpiWindow); 
	rcDpi.right = MulDiv(rcWnd.right, iDpiProcess, iDpiWindow); 
	rcDpi.bottom = MulDiv(rcWnd.bottom, iDpiProcess, iDpiWindow); 

	CStringA sDebug;
	sDebug.Format( "DPI: p:%d w:%d, {%d, %d, %d, %d} -> {%d, %d, %d, %d}\n", iDpiProcess, iDpiWindow, 
		rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top,
		rcDpi.left, rcDpi.top, rcDpi.right - rcDpi.left, rcDpi.bottom - rcDpi.top
		);
	OutputDebugStringA( sDebug );

	return rcDpi;
#else
	UNREFERENCED_PARAMETER( hwnd );
	return rcWnd;
#endif
}


HRESULT CShellPreviewHandlerImpl::CreateLocalServer()
{
	HRESULT hr;
	hr = m_pPreviewHandler.CoCreateInstance(__uuidof(USDPreview::USDPreviewHandler));
	if ( FAILED( hr ) )
		return hr;

	CComQIPtr<IInitializeWithFile> pInitializeWithFile = m_pPreviewHandler.p;
	m_pInitializeWithFile = pInitializeWithFile;
	if ( m_pInitializeWithFile == nullptr )
		return E_NOINTERFACE;

	CComQIPtr<IObjectWithSite> pObjectWithSite = m_pPreviewHandler.p;
	m_pObjectWithSite = pObjectWithSite;
	if ( m_pObjectWithSite == nullptr )
		return E_NOINTERFACE;

	CComQIPtr<IOleWindow> pOleWindow = m_pPreviewHandler.p;
	m_pOleWindow = pOleWindow;
	if ( m_pOleWindow == nullptr )
		return E_NOINTERFACE;

	CComQIPtr<IPreviewHandlerVisuals> pPreviewHandlerVisuals = m_pPreviewHandler.p;
	m_pPreviewHandlerVisuals = pPreviewHandlerVisuals;
	if ( m_pPreviewHandlerVisuals == nullptr )
		return E_NOINTERFACE;

	return S_OK;
}

void CShellPreviewHandlerImpl::DestroyLocalServer()
{
	m_pInitializeWithFile = nullptr;
	m_pObjectWithSite = nullptr;
	m_pOleWindow = nullptr;
	m_pPreviewHandler = nullptr;
	m_pPreviewHandlerVisuals = nullptr;
}

HRESULT CShellPreviewHandlerImpl::FinalConstruct()
{
	return __super::FinalConstruct();
}

void CShellPreviewHandlerImpl::FinalRelease()
{
	DestroyLocalServer();

	__super::FinalRelease();
}

// IInitializeWithFile
STDMETHODIMP CShellPreviewHandlerImpl::Initialize(__RPC__in_string LPCWSTR pszFilePath, DWORD grfMode)
{
	HRESULT hr;

	hr = CreateLocalServer();
	if ( FAILED( hr ) )
		return hr;

	if ( m_pInitializeWithFile == nullptr )
		return E_FAIL;

	hr = m_pInitializeWithFile->Initialize( pszFilePath, grfMode );
	if ( FAILED( hr ) )
		return hr;

	m_clrBackground = ::GetSysColor( COLOR_WINDOW );
	m_clrText = ::GetSysColor( COLOR_WINDOWTEXT );

	return hr;
}

// IObjectWithSite
STDMETHODIMP CShellPreviewHandlerImpl::SetSite(__RPC__in_opt IUnknown* pUnkSite)
{
	if ( m_pObjectWithSite == nullptr )
		return E_FAIL;

	HRESULT hr;
	hr = m_pObjectWithSite->SetSite( pUnkSite );
	if ( FAILED( hr ) )
		return hr;

	return hr;
}

STDMETHODIMP CShellPreviewHandlerImpl::GetSite(__RPC__in REFIID riid, __RPC__deref_out_opt void** ppvSite)
{
	if ( m_pObjectWithSite == nullptr )
		return E_FAIL;

	HRESULT hr;
	hr = m_pObjectWithSite->GetSite( riid, ppvSite );
	if ( FAILED( hr ) )
		return hr;

	return hr;
}

// IOleWindow
STDMETHODIMP CShellPreviewHandlerImpl::GetWindow(__RPC__deref_out_opt HWND* phwnd)
{
	if ( m_pOleWindow == nullptr )
		return E_FAIL;

	HRESULT hr;
	hr = m_pOleWindow->GetWindow( phwnd );
	if ( FAILED( hr ) )
		return hr;

	return hr;
}

STDMETHODIMP CShellPreviewHandlerImpl::ContextSensitiveHelp(BOOL fEnterMode)
{
	if ( m_pOleWindow == nullptr )
		return E_FAIL;

	HRESULT hr;
	hr = m_pOleWindow->ContextSensitiveHelp( fEnterMode );
	if ( FAILED( hr ) )
		return hr;

	return hr;
}

// IPreviewHandler
STDMETHODIMP CShellPreviewHandlerImpl::SetWindow(__RPC__in HWND hwnd, __RPC__in const RECT* prc)
{
	if ( prc == nullptr )
		return E_POINTER;

	CAutoDpiAware dpi( DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE );

	m_hWnd = hwnd;
	m_rcWndRaw = *prc;

	m_rcWndDpiAware = UpdateRectForDPI( m_hWnd, m_rcWndRaw );

	if ( m_pPreviewHandler == nullptr )
		return E_FAIL;

	if ( m_bLoadScreenCreated )
	{
		m_LoadScreenDlg.SetParent( m_hWnd );
		m_LoadScreenDlg.SetWindowPos( nullptr, &m_rcWndDpiAware, 0 );
	}

	HRESULT hr;
	hr = m_pPreviewHandler->SetWindow( m_LoadScreenDlg, &m_rcWndRaw );
	if ( FAILED( hr ) )
		return hr;

	return hr;
}

STDMETHODIMP CShellPreviewHandlerImpl::SetRect(__RPC__in const RECT* prc)
{
	if ( prc == nullptr )
		return E_POINTER;

	CAutoDpiAware dpi( DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE );

	m_rcWndRaw = *prc;
	m_rcWndDpiAware = UpdateRectForDPI( m_hWnd, m_rcWndRaw );

	if ( m_pPreviewHandler == nullptr )
		return E_FAIL;

	if (m_bLoadScreenCreated)
		m_LoadScreenDlg.SetWindowPos( nullptr, &m_rcWndDpiAware, 0 );

	HRESULT hr;
	hr = m_pPreviewHandler->SetRect( &m_rcWndRaw );
	if ( FAILED( hr ) )
		return hr;

	return hr;
}

STDMETHODIMP CShellPreviewHandlerImpl::DoPreview()
{
	if ( m_pPreviewHandler == nullptr )
		return E_FAIL;

	CAutoDpiAware dpi( DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE );

	m_LoadScreenDlg.SetBackgroundColor( m_clrBackground );
	m_LoadScreenDlg.Create( m_hWnd );
	m_LoadScreenDlg.SetWindowPos( nullptr, &m_rcWndDpiAware, 0 );
	m_bLoadScreenCreated = true;

	HRESULT hr;
	hr = m_pPreviewHandler->SetWindow( m_LoadScreenDlg, &m_rcWndRaw );
	if ( FAILED( hr ) )
		return hr;

	hr = m_pPreviewHandler->DoPreview();
	if ( FAILED( hr ) )
		return hr;

	return hr;
}

STDMETHODIMP CShellPreviewHandlerImpl::Unload()
{
	if ( m_pPreviewHandler == nullptr )
		return E_FAIL;

	m_LoadScreenDlg.DestroyWindow();
	m_bLoadScreenCreated = false;

	HRESULT hr;
	hr = m_pPreviewHandler->Unload();

	DestroyLocalServer();

	return hr;
}

STDMETHODIMP CShellPreviewHandlerImpl::SetFocus()
{
	if ( m_pPreviewHandler == nullptr )
		return E_FAIL;

	HRESULT hr;
	hr = m_pPreviewHandler->SetFocus();
	if ( FAILED( hr ) )
		return hr;

	return hr;
}

STDMETHODIMP CShellPreviewHandlerImpl::QueryFocus(__RPC__deref_out_opt HWND* phwnd)
{
	if ( m_pPreviewHandler == nullptr )
		return E_FAIL;

	HRESULT hr;
	hr = m_pPreviewHandler->QueryFocus(phwnd);
	if ( FAILED( hr ) )
		return hr;

	return hr;
}

STDMETHODIMP CShellPreviewHandlerImpl::TranslateAccelerator(__RPC__in MSG* pmsg)
{
	if ( m_pPreviewHandler == nullptr )
		return E_FAIL;

	HRESULT hr;
	hr = m_pPreviewHandler->TranslateAccelerator(pmsg);
	if ( FAILED( hr ) )
		return hr;

	return hr;
}

STDMETHODIMP CShellPreviewHandlerImpl::SetBackgroundColor( COLORREF color )
{
	if ( m_pPreviewHandlerVisuals == nullptr )
		return E_FAIL;

	HRESULT hr;
	hr = m_pPreviewHandlerVisuals->SetBackgroundColor(color);
	if ( FAILED( hr ) )
		return hr;

	m_clrBackground = color;

	m_LoadScreenDlg.SetBackgroundColor( m_clrBackground );

	return S_OK;
}

STDMETHODIMP CShellPreviewHandlerImpl::SetFont( __RPC__in const LOGFONTW *plf )
{
	if ( m_pPreviewHandlerVisuals == nullptr )
		return E_FAIL;

	HRESULT hr;
	hr = m_pPreviewHandlerVisuals->SetFont(plf);
	if ( FAILED( hr ) )
		return hr;

	return S_OK;
}

STDMETHODIMP CShellPreviewHandlerImpl::SetTextColor( COLORREF color )
{
	if ( m_pPreviewHandlerVisuals == nullptr )
		return E_FAIL;

	HRESULT hr;
	hr = m_pPreviewHandlerVisuals->SetTextColor(color);
	if ( FAILED( hr ) )
		return hr;

	m_clrText = color;
	return S_OK;
}

HRESULT WINAPI CShellPreviewHandlerImpl::UpdateRegistry(_In_ BOOL bRegister) throw()
{
	ATL::_ATL_REGMAP_ENTRY regMapEntries[] =
	{
		{ L"CLSID_SHELLPREVIEWHANDLER", L"{2DEB1AB8-DECA-42C2-B576-3116D9CA8FA8}" },
		{ nullptr, nullptr }
	};

	return g_AtlModule.UpdateRegistryFromResource(IDR_REGISTRY_SHELLPREVIEWHANDLERIMPL, bRegister, regMapEntries);
}