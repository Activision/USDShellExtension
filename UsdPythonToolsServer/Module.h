#pragma once

#include "UsdPythonToolsLocalServer_h.h"
#include "resource.h"

// Using an ATL module here because it removes a lot of the 
// boiler-plate COM work that we would have to implement ourselves.

class CUsdPythonToolsLocalServerModule : public CAtlExeModuleT<CUsdPythonToolsLocalServerModule>
{
public:
	DECLARE_LIBID( LIBID_UsdPythonToolsLib )
	DECLARE_REGISTRY_APPID_RESOURCEID( IDR_REGISTRY_MODULE, "{8777F2C4-2318-408A-85D8-F65E15811971}" )

	// Override CAtlExeModuleT::PreMessageLoop to change REGCLS_MULTIPLEUSE to REGCLS_SINGLEUSE.
	// REGCLS_SINGLEUSE will create a new Windows process for every instance of our COM classes, 
	// REGCLS_MULTIPLEUSE will reuse the same Windows process.
	HRESULT PreMessageLoop( _In_ int /*nShowCmd*/ ) throw()
	{
		HRESULT hr = S_OK;
		CUsdPythonToolsLocalServerModule* pT = static_cast<CUsdPythonToolsLocalServerModule*>(this);

#ifndef _ATL_NO_COM_SUPPORT
		hr = pT->RegisterClassObjects(CLSCTX_LOCAL_SERVER,
			REGCLS_SINGLEUSE | // <- !!!
			REGCLS_SUSPENDED);

		if (FAILED(hr))
			return hr;

		if (hr == S_OK)
		{
			if (m_bDelayShutdown)
			{
				CHandle h(pT->StartMonitor());
				if (h.m_h == NULL)
				{
					hr = E_FAIL;
				}
				else
				{
					hr = CoResumeClassObjects();
					ATLASSERT(SUCCEEDED(hr));
					if (FAILED(hr))
					{
						::SetEvent(m_hEventShutdown); // tell monitor to shutdown
						::WaitForSingleObject(h, m_dwTimeOut * 2);
					}
				}
			}
			else
			{
				hr = CoResumeClassObjects();
				ATLASSERT(SUCCEEDED(hr));
			}

			if (FAILED(hr))
				pT->RevokeClassObjects();
		}
		else
		{
			m_bDelayShutdown = false;
		}

#endif	// _ATL_NO_COM_SUPPORT

		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
};

extern CUsdPythonToolsLocalServerModule g_AtlModule;
extern HMODULE g_hInstance;
// Helpful for debugging zombie processes.
// Set to the last method that was called in IUSDTools
extern CString g_DebugIUsdPythonToolsMethod;

#define DEBUG_RECORD_ENTRY() \
	g_DebugIUsdPythonToolsMethod = _T(__FUNCTION__)
