#include "stdafx.h"
#include "UsdPythonToolsImpl.h"
#include "Module.h"
#include "shared\PythonUtil.h"
#include "shared\emb.h"
#include "shared\environment.h"
#include "shared\EventViewerLog.h"
#include "UsdPythonToolsLocalServer_h.h"

#include <vector>


HRESULT CUsdPythonToolsImpl::FinalConstruct()
{
	SetupPythonEnvironment();

	return __super::FinalConstruct();
}

void CUsdPythonToolsImpl::FinalRelease()
{
	__super::FinalRelease();
}

static std::wstring FindRelativeFile(LPCWSTR sToolFileName)
{
	const std::vector<CString> &PathList = GetUsdPathList();

	for ( const CString &sDir : PathList )
	{
		TCHAR sFilePath[512];
		_tcscpy_s( sFilePath, sDir.GetString() );

		::PathCchAppend( sFilePath, ARRAYSIZE( sFilePath ), sToolFileName );

		DWORD nAttribs = ::GetFileAttributes( sFilePath );
		if ( (nAttribs != INVALID_FILE_ATTRIBUTES) && !(nAttribs & FILE_ATTRIBUTE_DIRECTORY) )
			return sFilePath;
	}

	return L"";
}

static FILE *OpenRelativeFile(LPCWSTR sToolFileName, std::wstring& sOutFilePath)
{
	sOutFilePath = FindRelativeFile( sToolFileName );

	FILE *fp = _wfsopen( sOutFilePath.c_str(), L"rb", _SH_SECURE );
	if ( fp == nullptr )
		return nullptr;

	return fp;
}

static HRESULT RunDiskPythonScript( LPCWSTR sToolFileName, std::vector<const TPyChar *> &ArgV, std::string &sStdOut, int& exitCode )
{
	exitCode = 0;

	std::wstring sFilePath;
	FILE *fp = OpenRelativeFile( sToolFileName, sFilePath );
	if (fp == nullptr)
		return E_FAIL;

	fseek( fp, 0, SEEK_END );
	int sizeOfFile = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	CStringA pyScript;
	LPSTR pScriptData = pyScript.GetBufferSetLength( sizeOfFile );
	fread( pScriptData, 1, sizeOfFile, fp );
	pyScript.ReleaseBuffer( sizeOfFile );

	fclose( fp );
	fp = nullptr;

    PyImport_AppendInittab("emb", emb::PyInit_emb);
	CPyInterpreter pyInterpreter;
    PyImport_ImportModule("emb");

    emb::stdout_write_type writeStdOut = [&sStdOut] (std::string s) { sStdOut += s; };
    emb::set_stdout(writeStdOut);
    emb::stdout_write_type writeStdErr = [&sStdOut] (std::string s) { sStdOut += s; };
    emb::set_stderr(writeStdErr);

	PyAppendSysPath( GetUsdPythonPathList() );
	PySetEnvironmentVariable( L"PATH", GetUsdPath() );
	PySetEnvironmentVariable( L"PYTHONPATH", GetUsdPythonPath() );

	PySys_SetArgvEx( static_cast<int>(ArgV.size()), const_cast<TPyChar**>(&ArgV[0]), 1 );

	PyObject* pMainModule = PyImport_AddModule("__main__");
    PyObject* pGlobalDict = PyModule_GetDict(pMainModule);

	CPyObject result = CPyObject( PyRun_String( pyScript.GetString(), Py_file_input, pGlobalDict, pGlobalDict ) );

	HRESULT hr = S_OK;
	if ( PyErr_Occurred() )
	{
		CPyException e;
        if ( !e.IsExceptionSystemExit() )
        {
			CString sErrorMsg = e.tracebackW();
			if ( sErrorMsg.IsEmpty() )
				sErrorMsg.Format( _T( "[Error]\n%hs" ), e.what() );

			if ( !sStdOut.empty() )
				sErrorMsg.AppendFormat( _T( "\n\n[STDOUT]\n%hs" ), sStdOut.c_str() );

			LogEventMessage( PYTHONTOOLS_CATEGORY, sErrorMsg.GetString(), LogEventType::Error );

            hr = E_FAIL;
        }
		else
		{
			PySystemExitObject* pSystemExitObject = reinterpret_cast<PySystemExitObject*>(e.GetValue());
			if ( pSystemExitObject && pSystemExitObject->code )
			{
				if ( PyLong_Check( pSystemExitObject->code ) )
				{
					exitCode = PyLong_AsLong( pSystemExitObject->code );
				}
				else if ( PyUnicode_Check( pSystemExitObject->code ) )
				{
					// unexpected
					sStdOut += e.what();
					exitCode = -1;
					LogEventMessage( PYTHONTOOLS_CATEGORY, e.whatW(), LogEventType::Error );
					hr = E_FAIL;
				}
			}
		}
	}

    emb::reset_stdout();
	emb::reset_stderr();

	return hr;
}

static HRESULT RunResourcePythonScript( UINT nResourceId, std::vector<const TPyChar *> &ArgV, std::string &sStdOut, int& exitCode )
{
	exitCode = 0;

	HRSRC hrscPy = ::FindResource( g_hInstance, MAKEINTRESOURCE( nResourceId ), _T("PYTHON") );
	if ( hrscPy == nullptr )
		return E_FAIL;

	HGLOBAL hPy = ::LoadResource( g_hInstance, hrscPy );
	if ( hPy == nullptr )
		return E_FAIL;

	void* pScriptData = ::LockResource( hPy );
	if ( pScriptData == nullptr )
		return E_FAIL;

	DWORD nSize = SizeofResource( g_hInstance, hrscPy );

	CStringA pyScript;
	pyScript.SetString( reinterpret_cast<LPCSTR>(pScriptData), nSize );

	PyImport_AppendInittab("emb", emb::PyInit_emb);
	CPyInterpreter pyInterpreter;
    PyImport_ImportModule("emb");

    emb::stdout_write_type writeStdOut = [&sStdOut] (std::string s) { sStdOut += s; };
    emb::set_stdout(writeStdOut);
    emb::stdout_write_type writeStdErr = [&sStdOut] (std::string s) { sStdOut += s; };
    emb::set_stderr(writeStdErr);

	PyAppendSysPath( GetUsdPythonPathList() );
	PySetEnvironmentVariable( L"PATH", GetUsdPath() );
	PySetEnvironmentVariable( L"PYTHONPATH", GetUsdPythonPath() );

	PySys_SetArgvEx( static_cast<int>(ArgV.size()), const_cast<TPyChar**>(&ArgV[0]), 1 );

	PyObject* pMainModule = PyImport_AddModule("__main__");
    PyObject* pGlobalDict = PyModule_GetDict(pMainModule);

	CPyObject result = CPyObject( PyRun_String( pyScript.GetString(), Py_file_input, pGlobalDict, pGlobalDict ) );

	HRESULT hr = S_OK;
	if ( PyErr_Occurred() )
	{
		CPyException e;
        if ( !e.IsExceptionSystemExit() )
        {
			CString sErrorMsg = e.tracebackW();
			if ( sErrorMsg.IsEmpty() )
				sErrorMsg.Format( _T( "[Error]\n%hs" ), e.what() );

			if ( !sStdOut.empty() )
				sErrorMsg.AppendFormat( _T( "\n\n[STDOUT]\n%hs" ), sStdOut.c_str() );

			LogEventMessage( PYTHONTOOLS_CATEGORY, sErrorMsg.GetString(), LogEventType::Error );

            hr = E_FAIL;
        }
		else
		{
			PySystemExitObject* pSystemExitObject = reinterpret_cast<PySystemExitObject*>(e.GetValue());
			if ( pSystemExitObject && pSystemExitObject->code )
			{
				if ( PyLong_Check( pSystemExitObject->code ) )
				{
					exitCode = PyLong_AsLong( pSystemExitObject->code );
				}
				else if ( PyUnicode_Check( pSystemExitObject->code ) )
				{
					// unexpected
					sStdOut += e.what();
					exitCode = -1;
					LogEventMessage( PYTHONTOOLS_CATEGORY, e.whatW(), LogEventType::Error );
					hr = E_FAIL;
				}
			}
		}
	}

    emb::reset_stdout();
	emb::reset_stderr();

	return hr;
}

STDMETHODIMP CUsdPythonToolsImpl::Record( IN BSTR usdStagePath, IN int imageWidth, IN BSTR renderer, OUT BSTR *outputImagePath )
{
	DEBUG_RECORD_ENTRY();

	HRESULT hr;

	TPyChar sPathToHostExe[MAX_PATH];
#if PY_MAJOR_VERSION >= 3
	::GetModuleFileNameW( nullptr, sPathToHostExe, ARRAYSIZE( sPathToHostExe ) );
#else
	::GetModuleFileNameA( nullptr, sPathToHostExe, ARRAYSIZE( sPathToHostExe ) );
#endif
	Py_SetProgramName( sPathToHostExe );

	wchar_t sTempPath[MAX_PATH];
	::GetTempPathW( ARRAYSIZE( sTempPath ), sTempPath );
	wchar_t sTempFileName[MAX_PATH];

	// search for a unique temp file name
	std::vector<CStringW> tempFileList;
	for ( ;; )
	{
		::GetTempFileNameW( sTempPath, L"usd", 0, sTempFileName );
		tempFileList.push_back( sTempFileName );
		::PathCchRenameExtension( sTempFileName, ARRAYSIZE( sTempFileName ), L"png" );
		if ( GetFileAttributesW( sTempFileName ) == INVALID_FILE_ATTRIBUTES )
			break;
	}

	// delete the temp files that GetTempFileNameW created
	for ( const CStringW &str : tempFileList )
		DeleteFileW( str );

    std::string sStdOut;

	CStringW sImageWidth;
	sImageWidth.Format( L"%d", imageWidth );

	// locate usdrecord
	std::wstring sUsdRecordAbsolutePath = FindRelativeFile( L"usdrecord" );
	if ( sUsdRecordAbsolutePath.empty() )
	{
		LogEventMessage( PYTHONTOOLS_CATEGORY, L"Failed to locate usdrecord", LogEventType::Error );
		return E_FAIL;
	}

	std::vector<const TPyChar *> ArgV;
	// Set the first argument as the absolute path to the usdrecord python script
	// We will use argv[0] to load it using importlib
	CW2Py pyUsdRecordAbsolutePath( sUsdRecordAbsolutePath.c_str() );
	ArgV.push_back( pyUsdRecordAbsolutePath );
	ArgV.push_back( _Tpy("--imageWidth") );
	CW2Py pyImageWidth( sImageWidth );
	ArgV.push_back( pyImageWidth );

	CW2Py pyRenderer(renderer);
	if ( renderer != nullptr && renderer[0] != '\0' )
	{
		ArgV.push_back( _Tpy("--renderer") );
		ArgV.push_back( pyRenderer );
	}

	CW2Py pyUsdStagePath(usdStagePath);
	ArgV.push_back( pyUsdStagePath );
	CW2Py pyTempFileName(sTempFileName);
	ArgV.push_back( pyTempFileName );

	int exitCode = 0;
	hr = RunResourcePythonScript( IDR_PYTHON_THUMBNAIL, ArgV, sStdOut, exitCode );
	if ( FAILED( hr ) )
		return hr;

	if ( exitCode != 0 )
	{
		CString sErrorMsg;
		sErrorMsg.Format( _T( "Error generating thumbnail for %ls\n\n%hs\n\nExit Code: %d" ), usdStagePath, sStdOut.c_str(), exitCode );

		LogEventMessage( PYTHONTOOLS_CATEGORY, sErrorMsg.GetString(), LogEventType::Error );

		return E_FAIL;
	}

	CComBSTR bstrOutputFile( sTempFileName );
	*outputImagePath = bstrOutputFile.Detach();

	return S_OK;
}

STDMETHODIMP CUsdPythonToolsImpl::View( IN BSTR usdStagePath, IN BSTR renderer )
{
	DEBUG_RECORD_ENTRY();

	HRESULT hr;

	TPyChar sPathToHostExe[MAX_PATH];
#if PY_MAJOR_VERSION >= 3
	::GetModuleFileNameW( nullptr, sPathToHostExe, ARRAYSIZE( sPathToHostExe ) );
#else
	::GetModuleFileNameA( nullptr, sPathToHostExe, ARRAYSIZE( sPathToHostExe ) );
#endif
	Py_SetProgramName( sPathToHostExe );

    std::string sStdOut;

	std::vector<const TPyChar *> ArgV;
	ArgV.push_back( sPathToHostExe );

	CW2Py pyRenderer(renderer);
	if ( renderer != nullptr && renderer[0] != '\0' )
	{
		ArgV.push_back( _Tpy("--renderer") );
		ArgV.push_back( pyRenderer );
	}

	CW2Py pyUsdStagePath(usdStagePath);
	ArgV.push_back( pyUsdStagePath );

	int exitCode = 0;
	hr = RunDiskPythonScript( L"usdview", ArgV, sStdOut, exitCode );
	if ( FAILED( hr ) )
		return hr;

	if ( exitCode != 0 )
	{
		CString sErrorMsg;
		sErrorMsg.Format( _T( "Error launching usdview for %ls\n\n%hs\n\nExit Code: %d" ), usdStagePath, sStdOut.c_str(), exitCode );

		LogEventMessage( PYTHONTOOLS_CATEGORY, sErrorMsg.GetString(), LogEventType::Error );

		return E_FAIL;
	}

	return S_OK;
}

HRESULT WINAPI CUsdPythonToolsImpl::UpdateRegistry(_In_ BOOL bRegister) throw()
{
	ATL::_ATL_REGMAP_ENTRY regMapEntries[] =
	{
		{ L"APPID", L"{8777F2C4-2318-408A-85D8-F65E15811971}" },
		{ L"CLSID_USDPYTHONTOOLS", L"{67F43831-59C3-450E-8956-AA76273F3E9F}" },
		{ nullptr, nullptr }
	};

	return g_AtlModule.UpdateRegistryFromResource(IDR_REGISTRY_USDTOOLSIMPL, bRegister, regMapEntries);
}