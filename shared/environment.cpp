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
#include "environment.h"

#include <Python.h>
#include <shlobj.h>

std::vector<CStringW> g_UsdPathList;
std::vector<CStringW> g_UsdPythonPathList;

CStringW g_UsdPath;
CStringW g_UsdPythonPath;
CStringW g_UsdEditor;

const std::vector<CStringW> &GetUsdPathList()
{
	return g_UsdPathList;
}

const std::vector<CStringW> &GetUsdPythonPathList()
{
	return g_UsdPythonPathList;
}

const CStringW &GetUsdPath()
{
	return g_UsdPath;
}

const CStringW &GetUsdPythonPath()
{
	return g_UsdPythonPath;
}

const CStringW &GetUsdEditor()
{
	return g_UsdEditor;
}

static std::vector<CStringW> TranslatePathsToList(LPCWSTR paths)
{
	std::vector<CStringW> pathList;
	CStringW sPaths(paths);

	CStringW sToken;
	int pos = 0;
	sToken = sPaths.Tokenize(L";", pos);
	if ( !sToken.IsEmpty() )
	{
		while ( !sToken.IsEmpty() )
		{
			pathList.push_back( sToken );
			sToken = sPaths.Tokenize( L";", pos );
		}
	}
	else
	{
		pathList.push_back( paths );
	}

	return pathList;
}

static CStringW AppendEnvironmentVariable( LPCWSTR sEnvironmentVariable, LPCWSTR sValue )
{
	// CRT environment variables
	size_t sizeRequired = 0;
	CStringW sGetBuffer;
	if ( _wgetenv_s( &sizeRequired, nullptr, 0, sEnvironmentVariable ) == 0 )
	{
		if ( sizeRequired > 0 )
		{
			LPWSTR pGetBuffer = sGetBuffer.GetBuffer( static_cast<int>(sizeRequired) );
			_wgetenv_s( &sizeRequired, pGetBuffer, sizeRequired, sEnvironmentVariable );
			sGetBuffer.ReleaseBufferSetLength( static_cast<int>(sizeRequired) );
		}
	}

	CStringW sSetBuffer = sValue;
	sSetBuffer += L";";
	sSetBuffer += sGetBuffer;
	_wputenv_s( sEnvironmentVariable, sSetBuffer );

	return sSetBuffer;
}

static bool GetPythonInstallationPathFromRegistry(LPTSTR sBuffer, DWORD nBufferSizeInChars)
{
	CString sPythonRegKeyInstallPath;
	sPythonRegKeyInstallPath.Format(_T("SOFTWARE\\Python\\PythonCore\\%hs\\InstallPath"), _CRT_STRINGIZE(PYTHONVERSION));

	LSTATUS ls;

	CRegKey regPythonInstallPath;
	ls = regPythonInstallPath.Open(HKEY_CURRENT_USER, sPythonRegKeyInstallPath, KEY_READ);
	if (ls != ERROR_SUCCESS)
	{
		ls = regPythonInstallPath.Open(HKEY_LOCAL_MACHINE, sPythonRegKeyInstallPath, KEY_READ);
		if (ls != ERROR_SUCCESS)
		{
			return false;
		}
	}

	ULONG nChars = nBufferSizeInChars;
	ls = regPythonInstallPath.QueryStringValue(_T(""), sBuffer, &nChars);
	if (ls != ERROR_SUCCESS)
		return false;

	return true;
}

static void SetupPathEnvironmentVariable(LPCWSTR sUSD_Path, LPCWSTR sPython_Path)
{
	CStringW sSetBuffer;

	if ( sPython_Path[0] == '\0' )
	{
		TCHAR sValue[512];
		bool bFound = GetPythonInstallationPathFromRegistry( sValue, ARRAYSIZE(sValue) );
#if (PY_MAJOR_VERSION == 2) && (PY_MINOR_VERSION == 7)
		if (bFound == false)
		{
			_tcscpy_s(sValue, ARRAYSIZE(sValue), _T("C:\\Python27\\"));
			DWORD nAttribs = ::GetFileAttributes(sBuffer);
			if ((nAttribs != INVALID_FILE_ATTRIBUTES) && (nAttribs & FILE_ATTRIBUTE_DIRECTORY))
				bResult = true;
		}
#endif
		if (bFound)
		{
			if (!sSetBuffer.IsEmpty())
				sSetBuffer += L";";
			sSetBuffer += sValue;
		}
	}
	else
	{
		if ( !sSetBuffer.IsEmpty() )
			sSetBuffer += L";";
		sSetBuffer += sPython_Path;
	}

	if ( !sSetBuffer.IsEmpty() )
		sSetBuffer += L";";
	sSetBuffer += sUSD_Path;

	g_UsdPath = AppendEnvironmentVariable( L"PATH", sSetBuffer );
}

static void SetupPythonPathEnvironmentVariable(LPCWSTR sUSD_PythonPath, LPCWSTR sPython_PythonPath)
{
	CStringW sSetBuffer = sUSD_PythonPath;
	if ( !sSetBuffer.IsEmpty() )
		sSetBuffer += L";";
	sSetBuffer += sPython_PythonPath;

	g_UsdPythonPath = AppendEnvironmentVariable( L"PYTHONPATH", sSetBuffer );
}

static void SetupUsdEditorEnvironmentVariable(LPCWSTR sUSD_EditorPath)
{
	if ( sUSD_EditorPath && sUSD_EditorPath[0] != '\0' )
	{
		AppendEnvironmentVariable( L"USD_EDITOR", sUSD_EditorPath );
		g_UsdEditor = sUSD_EditorPath;
	}
}

static void SetupUsdPluginPathEnvironmentVariable(LPCWSTR sUSD_PluginPath)
{
	if ( sUSD_PluginPath && sUSD_PluginPath[0] != '\0' )
	{
		AppendEnvironmentVariable( L"PXR_PLUGINPATH_NAME", sUSD_PluginPath );
	}
}

void GetPrivateProfileStringAndExpandEnvironmentStrings( LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, CStringW& lpReturnedString, const std::vector<CStringW> &ConfigFileList )
{
	// a default string that a user wouldn't enter by hand
	const wchar_t sDefault[] = L"{80CA827C-F8A2-42E3-B582-BF7940014D71}";
	wchar_t sBuffer[8196];

	for ( const CStringW &sConfigFile : ConfigFileList )
	{
		sBuffer[0] = '\0';
		::GetPrivateProfileStringW( lpAppName, lpKeyName, sDefault, sBuffer, ARRAYSIZE(sBuffer), sConfigFile );
		if ( sBuffer[0] != '\0' && wcscmp( sBuffer, sDefault ) )
			break;
	}

	// if we never found an entry, use the passed in default
	if ( sBuffer[0] == '\0' || wcscmp( sBuffer, sDefault ) == 0 )
	{
		wcscpy_s( sBuffer, lpDefault );
	}

	DWORD nLengthInChars = ::ExpandEnvironmentStringsW( sBuffer, lpReturnedString.GetBuffer(), lpReturnedString.GetAllocLength() );
	if ( static_cast<int>(nLengthInChars) > lpReturnedString.GetAllocLength() )
	{
		::ExpandEnvironmentStringsW( sBuffer, lpReturnedString.GetBuffer(nLengthInChars), nLengthInChars );
	}

	lpReturnedString.ReleaseBuffer( nLengthInChars );
}

std::vector<CStringW> BuildConfigFileList( HMODULE hCurrentModule )
{
	std::vector<CStringW> ConfigFileList;

	// 1. current user config file
	{
		wchar_t *pFolderPath = nullptr;
		if ( ::SHGetKnownFolderPath( FOLDERID_LocalAppData, KF_FLAG_DEFAULT, nullptr, &pFolderPath ) == S_OK )
		{
			wchar_t sConfigPath[1024];
			wcscpy_s( sConfigPath, pFolderPath );
			CoTaskMemFree( pFolderPath );
			pFolderPath = nullptr;
			::PathCchAppend( sConfigPath, ARRAYSIZE( sConfigPath ), L"Activision" );
			::PathCchAppend( sConfigPath, ARRAYSIZE( sConfigPath ), L"UsdShellExtension" );
			::PathCchAppend( sConfigPath, ARRAYSIZE( sConfigPath ), L"UsdShellExtension.ini" );
			ConfigFileList.push_back( sConfigPath );
		}
	}

	// 2. all users config file
	{
		wchar_t *pFolderPath = nullptr;
		if ( ::SHGetKnownFolderPath( FOLDERID_ProgramData, KF_FLAG_DEFAULT, nullptr, &pFolderPath ) == S_OK )
		{
			wchar_t sConfigPath[1024];
			wcscpy_s( sConfigPath, pFolderPath );
			CoTaskMemFree( pFolderPath );
			pFolderPath = nullptr;
			::PathCchAppend( sConfigPath, ARRAYSIZE( sConfigPath ), L"Activision" );
			::PathCchAppend( sConfigPath, ARRAYSIZE( sConfigPath ), L"UsdShellExtension" );
			::PathCchAppend( sConfigPath, ARRAYSIZE( sConfigPath ), L"UsdShellExtension.ini" );
			ConfigFileList.push_back( sConfigPath );
		}
	}

	// 3. config file next to module
	{
		wchar_t sConfigPath[1024];
		::GetModuleFileNameW( hCurrentModule, sConfigPath, ARRAYSIZE( sConfigPath ) );
		::PathCchRemoveFileSpec( sConfigPath, ARRAYSIZE( sConfigPath ) );
		::PathCchAppend( sConfigPath, ARRAYSIZE( sConfigPath ), L"UsdShellExtension.ini" );
		ConfigFileList.push_back( sConfigPath );
	}

	return ConfigFileList;
}

void SetupPythonEnvironment( HMODULE hCurrentModule )
{
	std::vector<CStringW> ConfigFileList = BuildConfigFileList(hCurrentModule);

	CStringW sUSD_Path;
	GetPrivateProfileStringAndExpandEnvironmentStrings( L"USD", L"PATH", L"", sUSD_Path, ConfigFileList );

	CStringW sUSD_PythonPath;
	GetPrivateProfileStringAndExpandEnvironmentStrings( L"USD", L"PYTHONPATH", L"", sUSD_PythonPath, ConfigFileList );

	CStringW sUSD_EditorPath;
	GetPrivateProfileStringAndExpandEnvironmentStrings( L"USD", L"EDITOR", L"", sUSD_EditorPath, ConfigFileList );

	CStringW sUSD_PluginPath;
	GetPrivateProfileStringAndExpandEnvironmentStrings( L"USD", L"PXR_PLUGINPATH_NAME", L"", sUSD_PluginPath, ConfigFileList );

	CStringW sPython_Path;
	GetPrivateProfileStringAndExpandEnvironmentStrings( L"PYTHON", L"PATH", L"", sPython_Path, ConfigFileList );

	CStringW sPython_PythonPath;
	GetPrivateProfileStringAndExpandEnvironmentStrings( L"PYTHON", L"PYTHONPATH", L"", sPython_PythonPath, ConfigFileList );

	g_UsdPathList = TranslatePathsToList(sUSD_Path);
	g_UsdPythonPathList = TranslatePathsToList(sUSD_PythonPath);

	SetupPathEnvironmentVariable( sUSD_Path, sPython_Path );
	SetupPythonPathEnvironmentVariable( sUSD_PythonPath, sPython_PythonPath );
	SetupUsdEditorEnvironmentVariable( sUSD_EditorPath );
	SetupUsdPluginPathEnvironmentVariable( sUSD_PluginPath );
}