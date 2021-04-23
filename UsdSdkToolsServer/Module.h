#pragma once

#include "UsdSdkToolsLocalServer_h.h"
#include "resource.h"

static constexpr wchar_t s_ApplicationName[] = L"Activision USD SDK Tools Server";

// Using an ATL module here because it removes a lot of the 
// boiler-plate COM work that we would have to implement ourselves.

class CUsdSdkToolsLocalServerModule : public CAtlExeModuleT<CUsdSdkToolsLocalServerModule>
{
public:
	DECLARE_LIBID( LIBID_UsdSdkToolsLib )
	DECLARE_REGISTRY_APPID_RESOURCEID( IDR_REGISTRY_MODULE, "{123A65E6-B4B4-4B46-BEF5-D0FCE7173261}" )

	// Override CAtlExeModuleT::PreMessageLoop to change REGCLS_MULTIPLEUSE to REGCLS_SINGLEUSE.
	// REGCLS_SINGLEUSE will create a new Windows process for every instance of our COM classes, 
	// REGCLS_MULTIPLEUSE will reuse the same Windows process.
	HRESULT PreMessageLoop( _In_ int /*nShowCmd*/ ) throw()
	{
		HRESULT hr = S_OK;
		CUsdSdkToolsLocalServerModule* pT = static_cast<CUsdSdkToolsLocalServerModule*>(this);

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

extern CUsdSdkToolsLocalServerModule g_AtlModule;
extern HMODULE g_hInstance;
// Helpful for debugging zombie processes.
// Set to the last method that was called in IUsdSdkTools
extern CString g_DebugIUsdSdkToolsMethod;

#define DEBUG_RECORD_ENTRY() \
	g_DebugIUsdSdkToolsMethod = _T(__FUNCTION__)
