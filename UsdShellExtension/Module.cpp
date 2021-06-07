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
#include "resource.h"
#include "shared/EventViewerMessages.h"
#include "shared/EventViewerLog.h"
#include "shared/environment.h"

#include <fstream>

#include <Python.h>

HMODULE g_hInstance;
CShellExtModule g_AtlModule;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
	switch ( ul_reason_for_call )
	{
	case DLL_PROCESS_ATTACH:
		g_hInstance = hModule;
		break;
	}

	return g_AtlModule.DllMain( ul_reason_for_call, lpReserved );
}

static bool s_bSilent = true;
static bool s_bVerify = true;

static std::vector<CString> TranslatePathsToList(LPCTSTR paths)
{
	std::vector<CString> pathList;
	CString sPaths(paths);

	CString sToken;
	int pos = 0;
	sToken = sPaths.Tokenize(_T(";"), pos);
	if ( !sToken.IsEmpty() )
	{
		while ( !sToken.IsEmpty() )
		{
			pathList.push_back( sToken );
			sToken = sPaths.Tokenize( _T( ";" ), pos );
		}
	}
	else
	{
		pathList.push_back( paths );
	}

	return pathList;
}

static bool GetPythonInstallationPath( LPTSTR sBuffer, DWORD nBufferSizeInChars )
{
	std::vector<CStringW> ConfigFileList = BuildConfigFileList( g_hInstance );

	CStringW sPythonPath;
	GetPrivateProfileStringAndExpandEnvironmentStrings( L"PYTHON", L"PATH", L"", sPythonPath, ConfigFileList );
	wcscpy_s( sBuffer, nBufferSizeInChars, sPythonPath );

	if ( sBuffer[0] == '\0' )
	{
#if PY_MAJOR_VERSION >= 3
		CString sPythonRegKeyInstallPath;
		sPythonRegKeyInstallPath.Format( _T( "SOFTWARE\\Python\\PythonCore\\%hs\\InstallPath" ), _CRT_STRINGIZE(PYTHONVERSION) );

		LSTATUS ls;

		CRegKey regPythonInstallPath;
		ls = regPythonInstallPath.Open( HKEY_CURRENT_USER, sPythonRegKeyInstallPath, KEY_READ );
		if ( ls != ERROR_SUCCESS )
			return false;

		ULONG nChars = nBufferSizeInChars;
		ls = regPythonInstallPath.QueryStringValue( _T(""), sBuffer, &nChars );
		if ( ls != ERROR_SUCCESS )
			return false;
#elif (PY_MAJOR_VERSION == 2) && (PY_MINOR_VERSION == 7)
		_tcscpy_s(sBuffer, nBufferSizeInChars, _T("C:\\Python27\\"));
#endif
	}
	else
	{
		std::vector<CString> pathList = TranslatePathsToList( sBuffer );
		wcscpy_s( sBuffer, nBufferSizeInChars, pathList[0] );
	}

	return true;

}

static bool VerifyPythonInstallation()
{
	TCHAR sPython_Path[2048];
	GetPythonInstallationPath( sPython_Path, ARRAYSIZE( sPython_Path ) );
	
#if PY_MAJOR_VERSION >= 3
	::PathCchAppend( sPython_Path, ARRAYSIZE(sPython_Path), _T( PYTHONDLL ) );
	if ( ::GetFileAttributesW( sPython_Path ) == INVALID_FILE_ATTRIBUTES )
		return false;
#else
	DWORD nAttrib = ::GetFileAttributesW( sPython_Path );
	if ( nAttrib == INVALID_FILE_ATTRIBUTES || (nAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0 )
		return false;
#endif

	return true;
}

static bool VerifyPyOpenGLInstallation()
{
	TCHAR sPython_Path[2048];
	GetPythonInstallationPath( sPython_Path, ARRAYSIZE( sPython_Path ) );

	::PathCchAppend( sPython_Path, ARRAYSIZE(sPython_Path), L"Lib" );
	::PathCchAppend( sPython_Path, ARRAYSIZE(sPython_Path), L"site-packages" );
	::PathCchAppend( sPython_Path, ARRAYSIZE(sPython_Path), L"OpenGL" );

	if ( ::GetFileAttributesW( sPython_Path ) == INVALID_FILE_ATTRIBUTES )
		return false;

	return true;
}

static bool VerifyPySideInstallation()
{
	TCHAR sPython_Path[2048];
	GetPythonInstallationPath( sPython_Path, ARRAYSIZE( sPython_Path ) );

	::PathCchAppend( sPython_Path, ARRAYSIZE(sPython_Path), L"Lib" );
	::PathCchAppend( sPython_Path, ARRAYSIZE(sPython_Path), L"site-packages" );
	::PathCchAppend( sPython_Path, ARRAYSIZE(sPython_Path), L"PySide" );

	if ( ::GetFileAttributesW( sPython_Path ) == INVALID_FILE_ATTRIBUTES )
		return false;

	return true;
}

static bool VerifyPySide2Installation()
{
	TCHAR sPython_Path[2048];
	GetPythonInstallationPath( sPython_Path, ARRAYSIZE( sPython_Path ) );

	::PathCchAppend( sPython_Path, ARRAYSIZE(sPython_Path), L"Lib" );
	::PathCchAppend( sPython_Path, ARRAYSIZE(sPython_Path), L"site-packages" );
	::PathCchAppend( sPython_Path, ARRAYSIZE(sPython_Path), L"PySide2" );

	if ( ::GetFileAttributesW( sPython_Path ) == INVALID_FILE_ATTRIBUTES )
		return false;

	return true;
}

static HRESULT RegisterPropDescFile( LPCTSTR sFileName, UINT nResourceId )
{
	HRSRC hrscPd = ::FindResource( g_hInstance, MAKEINTRESOURCE( nResourceId ), _T("XML") );
	if ( hrscPd == nullptr )
		return E_INVALIDARG;

	HGLOBAL hPd = ::LoadResource( g_hInstance, hrscPd );
	if ( hPd == nullptr )
		return E_INVALIDARG;

	void* pXMLData = ::LockResource( hPd );
	if ( pXMLData == nullptr )
		return E_INVALIDARG;

	DWORD nSize = SizeofResource( g_hInstance, hrscPd );

	CStringA sPropDesc;
	sPropDesc.SetString( reinterpret_cast<LPCSTR>(pXMLData), nSize );

	TCHAR sPath[MAX_PATH];
	::GetModuleFileName( g_hInstance, sPath, ARRAYSIZE( sPath ) );

	sPropDesc.Replace( "%MODULE%", ATL::CW2A( sPath, CP_UTF8 ) );
	sPropDesc.Replace( "%LANGMODULE%", ATL::CW2A( sPath, CP_UTF8 ) );

	sPropDesc.Replace( "%IDS_USD_PROPGROUP_USD_LABEL%", _CRT_STRINGIZE( IDS_USD_PROPGROUP_USD_LABEL ) );
	sPropDesc.Replace( "%IDS_USD_DOCUMENTATION_LABEL%", _CRT_STRINGIZE( IDS_USD_DOCUMENTATION_LABEL ) );
	sPropDesc.Replace( "%IDS_USD_DOCUMENTATION_INVITATIONTEXT%", _CRT_STRINGIZE( IDS_USD_DOCUMENTATION_INVITATIONTEXT ) );
	sPropDesc.Replace( "%IDS_USD_DOCUMENTATION_MNEMONICS%", _CRT_STRINGIZE( IDS_USD_DOCUMENTATION_MNEMONICS ) );

	sPropDesc.Replace( "%IDS_USD_CUSTOMLAYERDATA_LABEL%", _CRT_STRINGIZE( IDS_USD_CUSTOMLAYERDATA_LABEL ) );
	sPropDesc.Replace( "%IDS_USD_CUSTOMLAYERDATA_MNEMONICS%", _CRT_STRINGIZE( IDS_USD_CUSTOMLAYERDATA_MNEMONICS ) );

	::PathCchRemoveFileSpec( sPath, ARRAYSIZE( sPath ) );
	::PathCchAppend( sPath, ARRAYSIZE( sPath ), sFileName );

	std::ofstream fileOut( sPath, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary );
	if (!fileOut.is_open())
		return E_FAIL;

	fileOut << sPropDesc;

	fileOut.close();

	HRESULT hr;
	hr = PSRegisterPropertySchema( sPath );

	return hr;
}

static HRESULT UnregisterPropDescFile( LPCTSTR sFileName )
{
	TCHAR sPath[MAX_PATH];
	::GetModuleFileName( g_hInstance, sPath, ARRAYSIZE( sPath ) );
	::PathCchRemoveFileSpec( sPath, ARRAYSIZE( sPath ) );
	::PathCchAppend( sPath, ARRAYSIZE( sPath ), sFileName );

	HRESULT hr;
	hr = PSUnregisterPropertySchema( sPath );

	return hr;
}

static HRESULT InstallEventSource()
{
	TCHAR sPath[MAX_PATH];
	::GetModuleFileName( g_hInstance, sPath, ARRAYSIZE( sPath ) );

	LSTATUS ls;
	CRegKey regEventLog;
	ls = regEventLog.Create( HKEY_LOCAL_MACHINE, _T( "System\\CurrentControlSet\\Services\\Eventlog\\Application\\" ) _T( EVENTLOG_SOURCE ) );
	if ( ls != ERROR_SUCCESS )
		return E_FAIL;

	DWORD TypesSupported = 
		EVENTLOG_SUCCESS |
		EVENTLOG_ERROR_TYPE | 
		EVENTLOG_WARNING_TYPE | 
		EVENTLOG_INFORMATION_TYPE;

	regEventLog.SetDWORDValue( _T( "TypesSupported" ), TypesSupported );
	regEventLog.SetStringValue( _T( "EventMessageFile" ), sPath );
	regEventLog.SetStringValue( _T( "CategoryMessageFile" ), sPath );
	regEventLog.SetDWORDValue( _T( "CategoryCount" ), CATEGORY_COUNT );

	return S_OK;
}

static HRESULT UninstallEventSource()
{
	LSTATUS ls;
	CRegKey regEventLog;
	ls = regEventLog.Open( HKEY_LOCAL_MACHINE, _T( "System\\CurrentControlSet\\Services\\Eventlog\\Application" ) );
	if ( ls != ERROR_SUCCESS )
		return E_FAIL;

	ls = regEventLog.RecurseDeleteKey( _T( EVENTLOG_SOURCE ) );
	if ( ls != ERROR_SUCCESS )
		return E_FAIL;

	return S_OK;
}

// Called to register/install this shell extension.
// Typically called from regsvr32.exe
STDAPI DllRegisterServer()
{
	HRESULT hr;

	if ( s_bVerify )
	{
		if ( !VerifyPythonInstallation() )
		{
			if ( s_bSilent == false )
			{
				::MessageBox( nullptr, 
					_T( "Python " ) _T( _CRT_STRINGIZE(PYTHONVERSION) ) _T( " is not installed on this system. It is required to run this shell extension.\n\n" )
					_T( "Download and install Python " ) _T( _CRT_STRINGIZE(PYTHONVERSION) ) _T( " from python.org" ), 
					_T( "Python " ) _T( _CRT_STRINGIZE(PYTHONVERSION) ) _T( " Not Installed" ), 
					MB_ICONERROR );
			}

			return E_PYTHON_NOT_INSTALLED;
		}

		if ( !VerifyPyOpenGLInstallation() )
		{
			if ( s_bSilent == false )
			{
				::MessageBox( nullptr, 
					_T( "Python " ) _T( _CRT_STRINGIZE(PYTHONVERSION) ) _T( " pyOpenGL is not installed on this system. It is required to run this shell extension.\n\n" )
					_T( "Run the following command from a Python " ) _T( _CRT_STRINGIZE(PYTHONVERSION) ) _T( " command prompt.\n\n" )
					_T( "pip install pyOpenGL" ), 
					_T( "Python " ) _T( _CRT_STRINGIZE(PYTHONVERSION) ) _T( " pyOpenGL Not Installed" ), 
					MB_ICONERROR );
			}

			return E_PYOPENGL_NOT_INSTALLED;
		}

#if PY_MAJOR_VERSION >= 3
		if ( !VerifyPySide2Installation() )
		{
			if ( s_bSilent == false )
			{
				::MessageBox( nullptr, 
					_T( "Python " ) _T( _CRT_STRINGIZE(PYTHONVERSION) ) _T( " pySide2 is not installed on this system. It is required to run this shell extension.\n\n" )
					_T( "Run the following command from a Python " ) _T( _CRT_STRINGIZE(PYTHONVERSION) ) _T( " command prompt.\n\n" )
					_T( "pip install pySide2" ), 
					_T( "Python " ) _T( _CRT_STRINGIZE(PYTHONVERSION) ) _T( " pySide2 Not Installed" ), 
					MB_ICONERROR );
			}

			return E_PYSIDE2_NOT_INSTALLED;
		}
#else
		if ( !VerifyPySideInstallation() )
		{
			if ( s_bSilent == false )
			{
				::MessageBox( nullptr, 
					_T( "Python " ) _T( _CRT_STRINGIZE(PYTHONVERSION) ) _T( " pySide is not installed on this system. It is required to run this shell extension.\n\n" )
					_T( "Run the following command from a Python " ) _T( _CRT_STRINGIZE(PYTHONVERSION) ) _T( " command prompt.\n\n" )
					_T( "pip install pySide" ), 
					_T( "Python " ) _T( _CRT_STRINGIZE(PYTHONVERSION) ) _T( " pySide Not Installed" ), 
					MB_ICONERROR );
			}

			return E_PYSIDE2_NOT_INSTALLED;
		}
#endif
	}

	hr = RegisterPropDescFile(_T("UsdPropertyKeys.propdesc"), IDR_XML_PROPDESC_USD);
	if ( FAILED( hr ) )
	{
		if ( s_bSilent == false )
		{
			CString sError;
			sError.Format( 
				_T( "Failed to install UsdPropertyKeys.propdesc.\n\n" )
				_T( "Error: 0x%.8X" ), 
				hr );

			::MessageBox( nullptr, 
				sError,
				_T( "PSRegisterPropertySchema Failed" ), 
				MB_ICONERROR );
		}
		return hr;
	}

	hr = InstallEventSource();
	if ( FAILED( hr ) )
	{
		return hr;
	}

	hr = g_AtlModule.DllRegisterServer( FALSE );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	hr = g_AtlModule.UpdateRegistry( TRUE );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	// notify the shell that we have updated handlers
	SHChangeNotify( SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL );

	return S_OK;
}

// Called to unregister/uninstall this shell extension.
// Typically called from regsvr32.exe /u
STDAPI DllUnregisterServer()
{
	g_AtlModule.DllUnregisterServer( FALSE );
	g_AtlModule.UpdateRegistry( FALSE );

	UnregisterPropDescFile( _T( "UsdPropertyKeys.propdesc" ) );

	UninstallEventSource();

	// notify the shell that we have updated handlers
	SHChangeNotify( SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL );

	return S_OK;
}

// Called to return an instance of a COM class we registered
STDAPI DllGetClassObject( REFCLSID rclsid, REFIID riid, void **ppv )
{
	HRESULT hr = g_AtlModule.DllGetClassObject( rclsid, riid, ppv );
	return hr;
}

// Called to determine if the DLL can be unloaded
// True if all COM references are at 0
STDAPI DllCanUnloadNow()
{
	return g_AtlModule.DllCanUnloadNow();
}

STDAPI DllInstall( BOOL bInstall, PCWSTR pszCmdLine )
{
	CoInitialize( nullptr );

	CCommandLineArgs args;
	args.argv = ::CommandLineToArgvW( pszCmdLine, &args.argc );

	s_bSilent = false;
	for ( int arg = 0; arg < args.argc; ++arg )
	{
		if ( _wcsicmp( args.argv[arg], L"-silent" ) == 0 )
			s_bSilent = true;
		else if ( _wcsicmp( args.argv[arg], L"-s" ) == 0 )
			s_bSilent = true;
		else if ( _wcsicmp( args.argv[arg], L"/silent" ) == 0 )
			s_bSilent = true;
		else if ( _wcsicmp( args.argv[arg], L"/s" ) == 0 )
			s_bSilent = true;
		else if ( _wcsicmp( args.argv[arg], L"-force" ) == 0 )
			s_bVerify = false;
		else if ( _wcsicmp( args.argv[arg], L"-f" ) == 0 )
			s_bVerify = false;
		else if ( _wcsicmp( args.argv[arg], L"/force" ) == 0 )
			s_bVerify = false;
		else if ( _wcsicmp( args.argv[arg], L"/f" ) == 0 )
			s_bVerify = false;
	}

	if ( bInstall )
		return DllRegisterServer();

	return DllUnregisterServer();
}

HRESULT WINAPI CShellExtModule::UpdateRegistry( _In_ BOOL bRegister ) throw()
{
	ATL::_ATL_REGMAP_ENTRY regMapEntries[] =
	{
		{ L"IDS_FILE_USD", _CRT_WIDE(_CRT_STRINGIZE(IDS_FILE_USD)) },
		{ L"IDS_FILE_USDA", _CRT_WIDE(_CRT_STRINGIZE(IDS_FILE_USDA)) },
		{ L"IDS_FILE_USDC", _CRT_WIDE(_CRT_STRINGIZE(IDS_FILE_USDC)) },
		{ L"IDS_FILE_USDZ", _CRT_WIDE(_CRT_STRINGIZE(IDS_FILE_USDZ)) },
		{ L"IDI_ICON_USD", _CRT_WIDE(_CRT_STRINGIZE(IDI_ICON_USD)) },
		{ L"IDS_SHELL_VIEW", _CRT_WIDE(_CRT_STRINGIZE(IDS_SHELL_VIEW)) },
		{ L"IDS_SHELL_EDIT", _CRT_WIDE(_CRT_STRINGIZE(IDS_SHELL_EDIT)) },
		{ L"IDS_SHELL_CRATE", _CRT_WIDE(_CRT_STRINGIZE(IDS_SHELL_CRATE)) },
		{ L"IDS_SHELL_UNCRATE", _CRT_WIDE(_CRT_STRINGIZE(IDS_SHELL_UNCRATE)) },
		{ L"IDS_SHELL_FLATTEN", _CRT_WIDE(_CRT_STRINGIZE(IDS_SHELL_FLATTEN)) },
		{ L"IDS_SHELL_REFRESHTHUMBNAIL", _CRT_WIDE(_CRT_STRINGIZE(IDS_SHELL_REFRESHTHUMBNAIL)) },
		{ L"IDS_SHELL_STATS", _CRT_WIDE(_CRT_STRINGIZE(IDS_SHELL_STATS)) },
		{ nullptr, nullptr }
	};

	return g_AtlModule.UpdateRegistryFromResource(IDR_REGISTRY_MODULE, bRegister, regMapEntries);
}