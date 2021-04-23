#pragma once

#include "ShellExt_h.h"
#include "resource.h"

static constexpr wchar_t s_ApplicationName[] = L"Activision USD Shell Extension";

// Using an ATL module here because it removes a lot of the 
// boiler-plate COM work that we would have to implement ourselves.

class CShellExtModule : public CAtlDllModuleT<CShellExtModule>
{
public:
	DECLARE_LIBID( LIBID_UsdShellExtensionLib )

	static HRESULT WINAPI UpdateRegistry(_In_ BOOL bRegister) throw();
};

extern CShellExtModule g_AtlModule;
extern HMODULE g_hInstance;

#define E_PYTHON_NOT_INSTALLED		MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0200)
#define E_PYOPENGL_NOT_INSTALLED	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0201)
#define E_PYSIDE2_NOT_INSTALLED		MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0202)

class CCommandLineArgs
{
public:
	~CCommandLineArgs()
	{
		if ( argv )
			LocalFree( argv );
	}

	int argc = 0;
	LPWSTR *argv = nullptr;
};

