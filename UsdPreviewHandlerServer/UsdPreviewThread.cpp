#include "stdafx.h"
#include "USDPreviewThread.h"
#include "Module.h"
#include "shared\PythonUtil.h"
#include "shared\emb.h"
#include "shared\environment.h"
#include "shared\EventViewerLog.h"

CUSDPreviewThread::~CUSDPreviewThread()
{
	Term();
}

bool CUSDPreviewThread::Init()
{
	return true;
}

void CUSDPreviewThread::Term()
{
	WaitForExit();
}

static bool GetRendererFromConfig( LPCTSTR system, CStringW &out )
{
	TCHAR sModulePath[MAX_PATH];
	::GetModuleFileName( g_hInstance, sModulePath, ARRAYSIZE( sModulePath ) );
	::PathCchRemoveFileSpec( sModulePath, ARRAYSIZE( sModulePath ) );
	::PathCchAppend( sModulePath, ARRAYSIZE( sModulePath ), _T( "UsdShellExtension.cfg" ) );
	
	TCHAR sRenderer[128];
	sRenderer[0] = '\0';
	::GetPrivateProfileString( _T( "RENDERER" ), system, _T( "GL" ), sRenderer, ARRAYSIZE( sRenderer ), sModulePath );

	if ( sRenderer[0] == '\0' )
		return false;

	out = sRenderer;

	return true;
}

HRESULT CUSDPreviewThread::LaunchPreviewWindow( HWND hWndParent, LPCTSTR pPathToUsdStage )
{
	m_hParent = hWndParent;
	m_sPathToUsdStage = pPathToUsdStage;

	m_hThread = ::CreateThread( nullptr, 0, ThreadProc, this, 0, nullptr );
	if ( m_hThread == nullptr )
		return false;

	return S_OK;
}

void CUSDPreviewThread::WaitForExit()
{
	if ( m_hThread == nullptr )
		return;

	if ( ::WaitForSingleObject( m_hThread, 1000 ) == WAIT_TIMEOUT )
	{
		TerminateThread( m_hThread, (DWORD)-1 );
	}

	m_hThread = nullptr;
}

static CStringW LocateUsdviewq()
{
	const std::vector<CString> &PathList = GetUsdPythonPathList();

	for ( const CString &sDir : PathList )
	{
		TCHAR sFilePath[512];
		_tcscpy_s( sFilePath, sDir.GetString() );

		::PathCchAppend( sFilePath, ARRAYSIZE( sFilePath ), _T("pxr\\Usdviewq") );

		DWORD nAttribs = ::GetFileAttributes( sFilePath );
		if ( (nAttribs != INVALID_FILE_ATTRIBUTES) && (nAttribs & FILE_ATTRIBUTE_DIRECTORY) )
			return sFilePath;
	}

	return L"";
}


DWORD WINAPI CUSDPreviewThread::ThreadProc( _In_ LPVOID lpParameter )
{
	CUSDPreviewThread *pThis = reinterpret_cast<CUSDPreviewThread *>(lpParameter);

	TPyChar sPathToHostExe[MAX_PATH];
#if PY_MAJOR_VERSION >= 3
	::GetModuleFileNameW(nullptr, sPathToHostExe, ARRAYSIZE(sPathToHostExe));
#else
	::GetModuleFileNameA(nullptr, sPathToHostExe, ARRAYSIZE(sPathToHostExe));
#endif

	Py_SetProgramName(sPathToHostExe);

    PyImport_AppendInittab("emb", emb::PyInit_emb);
	CPyInterpreter pyInterpreter;
    PyImport_ImportModule("emb");

	CStringW shWndParent;
	shWndParent.Format(L"%d", pThis->m_hParent);

	CStringW sRenderer;
	GetRendererFromConfig( _T( "PREVIEW" ), sRenderer );

	CStringW sUsdViewqFolder;
	sUsdViewqFolder = LocateUsdviewq();

	// UsdStageView initializes the renderer from this environment variable
	_tputenv_s( _T("HD_DEFAULT_RENDERER"), sRenderer );

	std::vector<const TPyChar *> ArgV;
	ArgV.push_back( sPathToHostExe );
	ArgV.push_back( _Tpy( "--hwnd" ) );
	CW2Py pyhWndParent( shWndParent );
	ArgV.push_back( pyhWndParent );
	ArgV.push_back( _Tpy( "--usdviewqDir" ) );
	CW2Py pyUsdViewqFolder( sUsdViewqFolder );
	ArgV.push_back( pyUsdViewqFolder );
	CW2Py pyPathToUsdStage( pThis->m_sPathToUsdStage );
	ArgV.push_back( pyPathToUsdStage );

	PyAppendSysPath( GetUsdPythonPathList() );
	PySetEnvironmentVariable( L"PATH", GetUsdPath() );
	PySetEnvironmentVariable( L"PYTHONPATH", GetUsdPythonPath() );

	PySys_SetArgvEx( static_cast<int>(ArgV.size()), const_cast<TPyChar**>(&ArgV[0]), 1 );

	HRSRC hrscPy = ::FindResource( g_hInstance, MAKEINTRESOURCE( IDR_PYTHON_PREVIEWHANLDER ), _T("PYTHON") );
	if ( hrscPy == nullptr )
		return static_cast<DWORD>(-1);

	HGLOBAL hPy = ::LoadResource( g_hInstance, hrscPy );
	if ( hPy == nullptr )
		return static_cast<DWORD>(-1);

	void* pScriptData = ::LockResource( hPy );
	if ( pScriptData == nullptr )
		return static_cast<DWORD>(-1);

	DWORD nSize = SizeofResource( g_hInstance, hrscPy );

	CStringA pyScript;
	pyScript.SetString( reinterpret_cast<LPCSTR>(pScriptData), nSize );

    PyObject* pMainModule = PyImport_AddModule("__main__");
    PyObject* pGlobalDict = PyModule_GetDict(pMainModule);

    std::string buffer;
    emb::stdout_write_type writeStdOut = [&buffer] (std::string s) { buffer += s; };
    emb::set_stdout(writeStdOut);
    emb::stdout_write_type writeStdErr = [&buffer] (std::string s) { buffer += s; };
    emb::set_stderr(writeStdErr);

	CPyObject result = CPyObject( PyRun_String( pyScript.GetString(), Py_file_input, pGlobalDict, pGlobalDict ) );

	emb::reset_stdout();
	emb::reset_stderr();

	int exitCode = 0;
	if ( PyErr_Occurred() )
	{
		CPyException e;
        if ( !e.IsExceptionSystemExit() )
        {
			CString sErrorMsg = e.tracebackW();
			if ( !buffer.empty() )
				sErrorMsg.AppendFormat( _T( "\n\n[STDOUT]\n%hs" ), buffer.c_str() );

			if ( sErrorMsg.IsEmpty() )
			{
				sErrorMsg = e.typeW();
			}

			LogEventMessage( PREVIEWHANDLER_CATEGORY, sErrorMsg.GetString(), LogEventType::Error );

            return static_cast<DWORD>(-1);
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
					buffer += e.what();
					exitCode = -1;
					LogEventMessage( PREVIEWHANDLER_CATEGORY, e.whatW(), LogEventType::Error );
				}
			}
		}
	}

	return exitCode;
}

