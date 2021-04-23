#include "stdafx.h"
#include "ShellThumbnailProviderImpl.h"
#include "Module.h"
#include "resource.h"

#import "UsdPythonToolsLocalServer.tlb" raw_interfaces_only 


HRESULT CShellThumbnailProviderImpl::FinalConstruct()
{
	return __super::FinalConstruct();
}

void CShellThumbnailProviderImpl::FinalRelease()
{
	__super::FinalRelease();
}

static bool GetRendererFromConfig( CComBSTR &outBstr )
{
	TCHAR sModulePath[MAX_PATH];
	::GetModuleFileName( g_hInstance, sModulePath, ARRAYSIZE( sModulePath ) );
	::PathCchRenameExtension( sModulePath, ARRAYSIZE( sModulePath ), L"cfg" );
	
	TCHAR sRenderer[128];
	sRenderer[0] = '\0';
	::GetPrivateProfileString( _T( "RENDERER" ), _T( "THUMBNAIL" ), _T( "" ), sRenderer, ARRAYSIZE( sRenderer ), sModulePath );

	if ( sRenderer[0] == '\0' )
		return false;

	outBstr = sRenderer;

	return true;
}

// IInitializeWithFile
STDMETHODIMP CShellThumbnailProviderImpl::Initialize(__RPC__in_string LPCWSTR pszFilePath, DWORD grfMode)
{
	UNREFERENCED_PARAMETER( grfMode );

	if ( pszFilePath == nullptr )
		return E_POINTER;

	m_usdStagePath = pszFilePath;

	return S_OK;
}

// IThumbnailProvider
STDMETHODIMP CShellThumbnailProviderImpl::GetThumbnail( UINT cx, __RPC__deref_out_opt HBITMAP *phbmp, __RPC__out WTS_ALPHATYPE *pdwAlpha)
{
	if ( phbmp == nullptr )
		return E_POINTER;

	HRESULT hr;

	CComPtr<UsdPythonToolsLib::IUsdPythonTools> pUSDTools;
	hr = pUSDTools.CoCreateInstance( __uuidof(UsdPythonToolsLib::UsdPythonTools) );
	if ( FAILED( hr ) )
		return hr;

	CComBSTR bstrRenderer;
	bool bRendererIsSet = GetRendererFromConfig( bstrRenderer );

	BSTR sThumbnailImage = nullptr;
	hr = pUSDTools->Record( CComBSTR(m_usdStagePath), cx, bRendererIsSet ? bstrRenderer : nullptr, &sThumbnailImage );
	if ( FAILED( hr ) )
		return hr;

	CComBSTR bstrThumbnailImage;
	bstrThumbnailImage.Attach( sThumbnailImage );

	CImage imgBmp;
	hr = imgBmp.Load( bstrThumbnailImage );
	if ( FAILED( hr ) )
		return hr;

	*phbmp = imgBmp.Detach();

	if ( pdwAlpha )
		*pdwAlpha = WTSAT_ARGB;

	return S_OK;
}


HRESULT WINAPI CShellThumbnailProviderImpl::UpdateRegistry(_In_ BOOL bRegister) throw()
{
	ATL::_ATL_REGMAP_ENTRY regMapEntries[] =
	{
		{ L"CLSID_SHELLTHUMBNAILPROVIDER", L"{2877CB72-1427-43B0-A2B4-9CB1B2BA8C53}" },
		{ nullptr, nullptr }
	};

	return g_AtlModule.UpdateRegistryFromResource(IDR_REGISTRY_SHELLTHUMBNAILPROVIDERIMPL, bRegister, regMapEntries);
}