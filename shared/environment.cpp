#include "stdafx.h"
#include "environment.h"

#include <Python.h>

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

static void SetupPathEnvironmentVariable(LPCWSTR sUSD_Path, LPCWSTR sPython_Path)
{
	CStringW sSetBuffer;

	if ( sPython_Path[0] == '\0' )
	{
#if PY_MAJOR_VERSION >= 3
	#if defined(PYTHONVERSION)
		CStringW sPythonRegKeyInstallPath;
		sPythonRegKeyInstallPath.Format( L"SOFTWARE\\Python\\PythonCore\\%hs\\InstallPath", _CRT_STRINGIZE(PYTHONVERSION) );

		LSTATUS ls;
		CRegKey regPythonInstallPath;
		ls = regPythonInstallPath.Open( HKEY_CURRENT_USER, sPythonRegKeyInstallPath, KEY_READ );
		if ( ls == ERROR_SUCCESS )
		{
			TCHAR sValue[512];
			ULONG nChars = ARRAYSIZE( sValue );
			ls = regPythonInstallPath.QueryStringValue( L"", sValue, &nChars );
			if ( ls == ERROR_SUCCESS )
			{
				if ( !sSetBuffer.IsEmpty() )
					sSetBuffer += L";";
				sSetBuffer += sValue;
			}
		}
	#endif
#elif PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION == 7
		static constexpr wchar_t python27InstallPath[] = L"C:\\Python27\\";
		DWORD nAttrib = ::GetFileAttributesW( python27InstallPath );
		if ( (nAttrib != INVALID_FILE_ATTRIBUTES) && (nAttrib & FILE_ATTRIBUTE_DIRECTORY) )
		{
			if ( !sSetBuffer.IsEmpty() )
				sSetBuffer += L";";
			sSetBuffer += python27InstallPath;
		}
#endif
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

static void GetPrivateProfileStringAndExpandEnvironmentStrings( LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName )
{
	wchar_t sBuffer[8196];
	sBuffer[0] = '\0';
	::GetPrivateProfileStringW( lpAppName, lpKeyName, lpDefault, sBuffer, ARRAYSIZE(sBuffer), lpFileName );
	::ExpandEnvironmentStringsW( sBuffer, lpReturnedString, nSize );
}

void SetupPythonEnvironment()
{
	wchar_t sModulePath[MAX_PATH];
	::GetModuleFileNameW( nullptr, sModulePath, ARRAYSIZE( sModulePath ) );
	::PathCchRemoveFileSpec( sModulePath, ARRAYSIZE( sModulePath ) );
	::PathCchAppend( sModulePath, ARRAYSIZE( sModulePath ), L"UsdShellExtension.ini" );

	wchar_t sUSD_Path[2048];
	GetPrivateProfileStringAndExpandEnvironmentStrings( L"USD", L"PATH", L"", sUSD_Path, ARRAYSIZE( sUSD_Path ), sModulePath );

	wchar_t sUSD_PythonPath[2048];
	GetPrivateProfileStringAndExpandEnvironmentStrings( L"USD", L"PYTHONPATH", L"", sUSD_PythonPath, ARRAYSIZE( sUSD_PythonPath ), sModulePath );

	wchar_t sUSD_EditorPath[MAX_PATH];
	GetPrivateProfileStringAndExpandEnvironmentStrings( L"USD", L"EDITOR", L"", sUSD_EditorPath, ARRAYSIZE( sUSD_EditorPath ), sModulePath );

	wchar_t sUSD_PluginPath[2048];
	GetPrivateProfileStringAndExpandEnvironmentStrings( L"USD", L"PXR_PLUGINPATH_NAME", L"", sUSD_PluginPath, ARRAYSIZE( sUSD_PluginPath ), sModulePath );

	wchar_t sPython_Path[2048];
	GetPrivateProfileStringAndExpandEnvironmentStrings( L"PYTHON", L"PATH", L"", sPython_Path, ARRAYSIZE( sPython_Path ), sModulePath );

	wchar_t sPython_PythonPath[2048];
	GetPrivateProfileStringAndExpandEnvironmentStrings( L"PYTHON", L"PYTHONPATH", L"", sPython_PythonPath, ARRAYSIZE( sPython_PythonPath ), sModulePath );

	g_UsdPathList = TranslatePathsToList(sUSD_Path);
	g_UsdPythonPathList = TranslatePathsToList(sUSD_PythonPath);

	SetupPathEnvironmentVariable( sUSD_Path, sPython_Path );
	SetupPythonPathEnvironmentVariable( sUSD_PythonPath, sPython_PythonPath );
	SetupUsdEditorEnvironmentVariable( sUSD_EditorPath );
	SetupUsdPluginPathEnvironmentVariable( sUSD_PluginPath );
}