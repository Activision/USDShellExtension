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

#include "stdafx.h"
#include "Module.h"

#pragma warning(push)
#pragma warning(disable:4192 4278 4471)
#import "UsdPythonToolsLocalServer.tlb" raw_interfaces_only 
#import "UsdSdkToolsLocalServer.tlb" raw_interfaces_only 
#pragma warning(pop)

static bool GetRendererFromConfig( LPCTSTR system, CComBSTR &outBstr )
{
	TCHAR sModulePath[MAX_PATH];
	::GetModuleFileName( g_hInstance, sModulePath, ARRAYSIZE( sModulePath ) );
	::PathCchRenameExtension( sModulePath, ARRAYSIZE( sModulePath ), L"cfg" );
	
	TCHAR sRenderer[128];
	sRenderer[0] = '\0';
	::GetPrivateProfileString( _T( "RENDERER" ), system, _T( "" ), sRenderer, ARRAYSIZE( sRenderer ), sModulePath );

	if ( sRenderer[0] == '\0' )
		return false;

	outBstr = sRenderer;

	return true;
}

extern "C" __declspec(dllexport)
void CALLBACK OpenInUsdViewW( HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER( hwnd );
	UNREFERENCED_PARAMETER( hinst );
	UNREFERENCED_PARAMETER( nCmdShow );

	CoInitialize( nullptr );

	HRESULT hr;

	CCommandLineArgs args;
	args.argv = ::CommandLineToArgvW( lpszCmdLine, &args.argc );
	if ( args.argc < 1 )
		return;

	if ( ::GetFileAttributesW( args.argv[0] ) == INVALID_FILE_ATTRIBUTES )
		return;

	CComBSTR bstrRenderer;
	bool bRendererIsValid = GetRendererFromConfig( _T( "VIEW" ), bstrRenderer );

	CComPtr<UsdPythonToolsLib::IUsdPythonTools> pUsdPythonTools;
	hr = pUsdPythonTools.CoCreateInstance( __uuidof(UsdPythonToolsLib::UsdPythonTools) );
	if ( FAILED( hr ) )
		return;

	::CoAllowSetForegroundWindow( pUsdPythonTools, nullptr );

	hr = pUsdPythonTools->View( CComBSTR(args.argv[0]), bRendererIsValid ? bstrRenderer : nullptr );
	if ( FAILED( hr ) )
		return;
}

extern "C" __declspec(dllexport)
void CALLBACK CrateWithUsdCatW( HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER( hwnd );
	UNREFERENCED_PARAMETER( hinst );
	UNREFERENCED_PARAMETER( nCmdShow );

	CoInitialize( nullptr );

	HRESULT hr;

	CCommandLineArgs args;
	args.argv = ::CommandLineToArgvW( lpszCmdLine, &args.argc );
	if ( args.argc < 1 )
		return;

	if ( ::GetFileAttributesW( args.argv[0] ) == INVALID_FILE_ATTRIBUTES )
		return;

	CComPtr<UsdSdkToolsLib::IUsdSdkTools> pUSDTools;
	hr = pUSDTools.CoCreateInstance( __uuidof(UsdSdkToolsLib::UsdSdkTools) );
	if ( FAILED( hr ) )
		return;

	wchar_t sOutputFile[512];
	wcscpy_s( sOutputFile, args.argv[0] );
	PathCchRenameExtension( sOutputFile, ARRAYSIZE( sOutputFile ), L"usdc" );

	hr = pUSDTools->Cat( CComBSTR(args.argv[0]), CComBSTR(sOutputFile), UsdSdkToolsLib::USD_FORMAT_USDC, VARIANT_FALSE );
	if ( FAILED( hr ) )
	{
		::MessageBox( hwnd, _T( "USD Crate Error" ), _T( "USD Crate Error" ), MB_ICONERROR );
		return;
	}
}

extern "C" __declspec(dllexport)
void CALLBACK UncrateWithUsdCatW( HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER( hwnd );
	UNREFERENCED_PARAMETER( hinst );
	UNREFERENCED_PARAMETER( nCmdShow );

	CoInitialize( nullptr );

	HRESULT hr;

	CCommandLineArgs args;
	args.argv = ::CommandLineToArgvW( lpszCmdLine, &args.argc );
	if ( args.argc < 1 )
		return;

	if ( ::GetFileAttributesW( args.argv[0] ) == INVALID_FILE_ATTRIBUTES )
		return;

	CComPtr<UsdSdkToolsLib::IUsdSdkTools> pUSDTools;
	hr = pUSDTools.CoCreateInstance( __uuidof(UsdSdkToolsLib::UsdSdkTools) );
	if ( FAILED( hr ) )
		return;

	wchar_t sOutputFile[512];
	wcscpy_s( sOutputFile, args.argv[0] );
	PathCchRenameExtension( sOutputFile, ARRAYSIZE( sOutputFile ), L"usda" );

	hr = pUSDTools->Cat( CComBSTR(args.argv[0]), CComBSTR(sOutputFile), UsdSdkToolsLib::USD_FORMAT_USDA, VARIANT_FALSE );
	if ( FAILED( hr ) )
	{
		::MessageBox( hwnd, _T( "USD Uncrate Error" ), _T( "USD Uncrate Error" ), MB_ICONERROR );
		return;
	}
}

extern "C" __declspec(dllexport)
void CALLBACK FlattenWithUsdCatW( HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER( hwnd );
	UNREFERENCED_PARAMETER( hinst );
	UNREFERENCED_PARAMETER( nCmdShow );

	CoInitialize( nullptr );

	HRESULT hr;

	CCommandLineArgs args;
	args.argv = ::CommandLineToArgvW( lpszCmdLine, &args.argc );
	if ( args.argc < 1 )
		return;

	if ( ::GetFileAttributesW( args.argv[0] ) == INVALID_FILE_ATTRIBUTES )
		return;

	CComPtr<UsdSdkToolsLib::IUsdSdkTools> pUSDTools;
	hr = pUSDTools.CoCreateInstance( __uuidof(UsdSdkToolsLib::UsdSdkTools) );
	if ( FAILED( hr ) )
		return;

	hr = pUSDTools->Cat( CComBSTR(args.argv[0]), CComBSTR(args.argv[0]), UsdSdkToolsLib::USD_FORMAT_INPUT, VARIANT_TRUE );
	if ( FAILED( hr ) )
	{
		::MessageBox( hwnd, _T( "USD Flatten Error" ), _T( "USD Flatten Error" ), MB_ICONERROR );
		return;
	}
}

extern "C" __declspec(dllexport)
void CALLBACK EditInUsdEditW( HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER( hwnd );
	UNREFERENCED_PARAMETER( hinst );
	UNREFERENCED_PARAMETER( nCmdShow );

	CoInitialize( nullptr );

	HRESULT hr;

	CCommandLineArgs args;
	args.argv = ::CommandLineToArgvW( lpszCmdLine, &args.argc );
	if ( args.argc < 1 )
		return;

	if ( ::GetFileAttributesW( args.argv[0] ) == INVALID_FILE_ATTRIBUTES )
		return;

	CComPtr<UsdSdkToolsLib::IUsdSdkTools> pUSDTools;
	hr = pUSDTools.CoCreateInstance( __uuidof(UsdSdkToolsLib::UsdSdkTools) );
	if ( FAILED( hr ) )
		return;

	hr = pUSDTools->Edit( CComBSTR(args.argv[0]), VARIANT_FALSE );
	if ( FAILED( hr ) )
	{
		::MessageBox( hwnd, _T( "USD Edit Error" ), _T( "USD Edit Error" ), MB_ICONERROR );
		return;
	}
}

extern "C" __declspec(dllexport)
void CALLBACK RefreshThumbnailW( HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER( hwnd );
	UNREFERENCED_PARAMETER( hinst );
	UNREFERENCED_PARAMETER( nCmdShow );

	CoInitialize( nullptr );

	HRESULT hr;

	CCommandLineArgs args;
	args.argv = ::CommandLineToArgvW( lpszCmdLine, &args.argc );
	if ( args.argc < 1 )
		return;

	if ( ::GetFileAttributesW( args.argv[0] ) == INVALID_FILE_ATTRIBUTES )
		return;

	CComPtr<IShellItem> pShellItem;
	hr = ::SHCreateItemFromParsingName( args.argv[0], nullptr, IID_PPV_ARGS( &pShellItem.p ) );
	if ( FAILED( hr ) )
		return;

	CComPtr<IThumbnailCache> pThumbnailCache;
	hr = pThumbnailCache.CoCreateInstance( CLSID_LocalThumbnailCache );
	if ( FAILED( hr ) )
		return;

	// According to this old thread, force extraction of any size of thumbnail 
	// should invalidate the entire cache for that IShellItem.
	// https://microsoft.public.platformsdk.shell.narkive.com/ABNbF5AQ/how-to-update-thumbnails-on-vista
	CComPtr<ISharedBitmap> pSharedBitmap;
	hr = pThumbnailCache->GetThumbnail( pShellItem, 256, WTS_FORCEEXTRACTION, &pSharedBitmap.p, nullptr, nullptr );

	// Notify the shell that the item has changed.
	::SHChangeNotify( SHCNE_UPDATEITEM, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, args.argv[0], nullptr );
}

extern "C" __declspec(dllexport)
void CALLBACK PackageDefaultW( HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER( hwnd );
	UNREFERENCED_PARAMETER( hinst );
	UNREFERENCED_PARAMETER( nCmdShow );

	CoInitialize( nullptr );

	HRESULT hr;

	CCommandLineArgs args;
	args.argv = ::CommandLineToArgvW( lpszCmdLine, &args.argc );
	if ( args.argc < 1 )
		return;

	if ( ::GetFileAttributesW( args.argv[0] ) == INVALID_FILE_ATTRIBUTES )
		return;

	CComPtr<UsdSdkToolsLib::IUsdSdkTools> pUSDTools;
	hr = pUSDTools.CoCreateInstance( __uuidof(UsdSdkToolsLib::UsdSdkTools) );
	if ( FAILED( hr ) )
		return;

	wchar_t sOutputFile[512];
	wcscpy_s( sOutputFile, args.argv[0] );
	PathCchRenameExtension( sOutputFile, ARRAYSIZE( sOutputFile ), L"usdz" );

	hr = pUSDTools->Package( CComBSTR(args.argv[0]), CComBSTR(sOutputFile), UsdSdkToolsLib::USD_PACKAGE_DEFAULT, VARIANT_TRUE );
	if ( FAILED( hr ) )
	{
		::MessageBox( hwnd, _T( "USD Package Error" ), _T( "USD Package Error" ), MB_ICONERROR );
		return;
	}
}

extern "C" __declspec(dllexport)
void CALLBACK PackageARKitW( HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER( hwnd );
	UNREFERENCED_PARAMETER( hinst );
	UNREFERENCED_PARAMETER( nCmdShow );

	CoInitialize( nullptr );

	HRESULT hr;

	CCommandLineArgs args;
	args.argv = ::CommandLineToArgvW( lpszCmdLine, &args.argc );
	if ( args.argc < 1 )
		return;

	if ( ::GetFileAttributesW( args.argv[0] ) == INVALID_FILE_ATTRIBUTES )
		return;

	CComPtr<UsdSdkToolsLib::IUsdSdkTools> pUSDTools;
	hr = pUSDTools.CoCreateInstance( __uuidof(UsdSdkToolsLib::UsdSdkTools) );
	if ( FAILED( hr ) )
		return;

	wchar_t sOutputFile[512];
	wcscpy_s( sOutputFile, args.argv[0] );
	PathCchRenameExtension( sOutputFile, ARRAYSIZE( sOutputFile ), L"usdz" );

	hr = pUSDTools->Package( CComBSTR(args.argv[0]), CComBSTR(sOutputFile), UsdSdkToolsLib::USD_FORMAT_APPLE_ARKIT, VARIANT_TRUE );
	if ( FAILED( hr ) )
	{
		::MessageBox( hwnd, _T( "USD Package Error" ), _T( "USD Package Error" ), MB_ICONERROR );
		return;
	}
}


extern "C" __declspec(dllexport)
void CALLBACK StageStatsW( HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER( hwnd );
	UNREFERENCED_PARAMETER( hinst );
	UNREFERENCED_PARAMETER( nCmdShow );

	CoInitialize( nullptr );

	HRESULT hr;

	CCommandLineArgs args;
	args.argv = ::CommandLineToArgvW( lpszCmdLine, &args.argc );
	if ( args.argc < 1 )
		return;

	if ( ::GetFileAttributesW( args.argv[0] ) == INVALID_FILE_ATTRIBUTES )
		return;

	CComPtr<UsdSdkToolsLib::IUsdSdkTools> pUSDTools;
	hr = pUSDTools.CoCreateInstance( __uuidof(UsdSdkToolsLib::UsdSdkTools) );
	if ( FAILED( hr ) )
		return;

	hr = pUSDTools->DisplayStageStats( CComBSTR(args.argv[0]) );
	if ( FAILED( hr ) )
	{
		::MessageBox( hwnd, _T( "USD Stage Stats Error" ), _T( "USD Stage Stats Error" ), MB_ICONERROR );
		return;
	}
}