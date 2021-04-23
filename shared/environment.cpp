#include "stdafx.h"
#include "environment.h"

std::vector<CString> g_UsdPaths;
std::vector<CString> g_UsdPythonPaths;

const std::vector<CString> &GetUsdPathList()
{
	return g_UsdPaths;
}

const std::vector<CString> &GetUsdPythonPathList()
{
	return g_UsdPythonPaths;
}

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

static void SetupPathEnvironmentVariable(LPCTSTR sUSD_Path, LPCTSTR sPython_Path)
{
	TCHAR sGetEnvironment[8196];
	TCHAR sSetEnvironment[8196];

	_tcscpy_s( sSetEnvironment, sUSD_Path );

	if ( sPython_Path[0] == '\0' )
	{
#if defined(PYTHONVERSION)
		CString sPythonRegKeyInstallPath;
		sPythonRegKeyInstallPath.Format( _T( "SOFTWARE\\Python\\PythonCore\\%hs\\InstallPath" ), PYTHONVERSION );

		LSTATUS ls;
		CRegKey regPythonInstallPath;
		ls = regPythonInstallPath.Open( HKEY_CURRENT_USER, sPythonRegKeyInstallPath, KEY_READ );
		if ( ls == ERROR_SUCCESS )
		{
			TCHAR sValue[512];
			ULONG nChars = ARRAYSIZE( sValue );
			ls = regPythonInstallPath.QueryStringValue( _T( "" ), sValue, &nChars );
			if ( ls == ERROR_SUCCESS )
			{
				if ( sSetEnvironment[0] != '\0' )
					_tcscat_s( sSetEnvironment, _T( ";" ) );
				_tcscat_s( sSetEnvironment, sValue );
			}
		}
#endif
	}
	else
	{
		if ( sSetEnvironment[0] != '\0' )
			_tcscat_s( sSetEnvironment, _T( ";" ) );
		_tcscat_s( sSetEnvironment, sPython_Path );
	}

	size_t sizeRequired = 0;
	sGetEnvironment[0] = '\0';
	if (_tgetenv_s(&sizeRequired, sGetEnvironment, _T("PATH")) == 0)
	{
		_tcscat_s(sSetEnvironment, _T(";"));
		_tcscat_s(sSetEnvironment, sGetEnvironment);
	}
	_tputenv_s(_T("PATH"), sSetEnvironment);
}

static void SetupPythonPathEnvironmentVariable(LPCTSTR sUSD_PythonPath, LPCTSTR sPython_PythonPath)
{
	TCHAR sGetEnvironment[8196];
	TCHAR sSetEnvironment[8196];

	_tcscpy_s( sSetEnvironment, sUSD_PythonPath );
	if ( sSetEnvironment[0] != '\0' )
		_tcscat_s( sSetEnvironment, _T( ";" ) );
	_tcscat_s( sSetEnvironment, sPython_PythonPath );

	size_t sizeRequired = 0;
	sGetEnvironment[0] = '\0';
	if (_tgetenv_s(&sizeRequired, sGetEnvironment, _T("PYTHONPATH")) == 0)
	{
		_tcscat_s(sSetEnvironment, _T(";"));
		_tcscat_s(sSetEnvironment, sGetEnvironment);
	}

	_tputenv_s(_T("PYTHONPATH"), sSetEnvironment);
}

static void SetupUsdEditorEnvironmentVariable(LPCTSTR sUSD_EditorPath)
{
	if ( sUSD_EditorPath[0] != '\0' )
	{
		_tputenv_s( _T( "USD_EDITOR" ), sUSD_EditorPath );
	}
}

static void SetupUsdPluginPathEnvironmentVariable(LPCTSTR sUSD_PluginPath)
{
	TCHAR sGetEnvironment[8196];
	TCHAR sSetEnvironment[8196];

	_tcscpy_s( sSetEnvironment, sUSD_PluginPath );

	size_t sizeRequired = 0;
	sGetEnvironment[0] = '\0';
	if (_tgetenv_s(&sizeRequired, sGetEnvironment, _T("PXR_PLUGINPATH_NAME")) == 0)
	{
		_tcscat_s(sSetEnvironment, _T(";"));
		_tcscat_s(sSetEnvironment, sGetEnvironment);
	}

	_tputenv_s(_T("PXR_PLUGINPATH_NAME"), sSetEnvironment);
}

static void GetPrivateProfileStringAndExpandEnvironmentStrings( LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpReturnedString, DWORD nSize, LPCTSTR lpFileName )
{
	TCHAR sBuffer[8196];
	sBuffer[0] = '\0';
	GetPrivateProfileString( lpAppName, lpKeyName, lpDefault, sBuffer, ARRAYSIZE(sBuffer), lpFileName );
	ExpandEnvironmentStrings( sBuffer, lpReturnedString, nSize );
}

void SetupPythonEnvironment()
{
	TCHAR sModulePath[MAX_PATH];
	GetModuleFileName( nullptr, sModulePath, ARRAYSIZE( sModulePath ) );
	PathCchRemoveFileSpec( sModulePath, ARRAYSIZE( sModulePath ) );
	PathCchAppend( sModulePath, ARRAYSIZE( sModulePath ), _T( "UsdShellExtension.cfg" ) );

	TCHAR sUSD_Path[2048];
	GetPrivateProfileStringAndExpandEnvironmentStrings( _T( "USD" ), _T( "PATH" ), _T( "" ), sUSD_Path, ARRAYSIZE( sUSD_Path ), sModulePath );

	TCHAR sUSD_PythonPath[2048];
	GetPrivateProfileStringAndExpandEnvironmentStrings( _T( "USD" ), _T( "PYTHONPATH" ), _T( "" ), sUSD_PythonPath, ARRAYSIZE( sUSD_PythonPath ), sModulePath );

	TCHAR sUSD_EditorPath[MAX_PATH];
	GetPrivateProfileStringAndExpandEnvironmentStrings( _T( "USD" ), _T( "EDITOR" ), _T( "" ), sUSD_EditorPath, ARRAYSIZE( sUSD_EditorPath ), sModulePath );

	TCHAR sUSD_PluginPath[2048];
	GetPrivateProfileStringAndExpandEnvironmentStrings( _T( "USD" ), _T( "PXR_PLUGINPATH_NAME" ), _T( "" ), sUSD_PluginPath, ARRAYSIZE( sUSD_PluginPath ), sModulePath );

	TCHAR sPython_Path[2048];
	GetPrivateProfileStringAndExpandEnvironmentStrings( _T( "PYTHON" ), _T( "PATH" ), _T( "" ), sPython_Path, ARRAYSIZE( sPython_Path ), sModulePath );

	TCHAR sPython_PythonPath[2048];
	GetPrivateProfileStringAndExpandEnvironmentStrings( _T( "PYTHON" ), _T( "PYTHONPATH" ), _T( "" ), sPython_PythonPath, ARRAYSIZE( sPython_PythonPath ), sModulePath );

	g_UsdPaths = TranslatePathsToList(sUSD_Path);
	g_UsdPythonPaths = TranslatePathsToList(sUSD_PythonPath);

	SetupPathEnvironmentVariable( sUSD_Path, sPython_Path );
	SetupPythonPathEnvironmentVariable( sUSD_PythonPath, sPython_PythonPath );
	SetupUsdEditorEnvironmentVariable( sUSD_EditorPath );
	SetupUsdPluginPathEnvironmentVariable( sUSD_PluginPath );

#if defined(PYTHONDLL)
	LoadLibrary( _T(PYTHONDLL) );
#endif
}