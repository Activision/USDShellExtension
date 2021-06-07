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

#pragma once

#include "ShellExt_h.h"
#include "resource.h"

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

