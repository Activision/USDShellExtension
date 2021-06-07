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

#include "USDPreviewLocalServer_h.h"
#include "resource.h"

// Using an ATL module here because it removes a lot of the 
// boiler-plate COM work that we would have to implement ourselves.

class CUSDPreviewLocalServerModule : public CAtlExeModuleT<CUSDPreviewLocalServerModule>
{
public:
	DECLARE_LIBID( LIBID_USDPreview )
	DECLARE_REGISTRY_APPID_RESOURCEID( IDR_REGISTRY_MODULE, "{067AAB34-285D-40D1-92EF-A5ED449EFEEE}" )

	// Override CAtlExeModuleT::PreMessageLoop to change REGCLS_MULTIPLEUSE to REGCLS_SINGLEUSE.
	// REGCLS_SINGLEUSE will create a new Windows process for every instance of our COM classes, 
	// REGCLS_MULTIPLEUSE will reuse the same Windows process.
	HRESULT PreMessageLoop( _In_ int /*nShowCmd*/ ) throw()
	{
		HRESULT hr = S_OK;
		CUSDPreviewLocalServerModule* pT = static_cast<CUSDPreviewLocalServerModule*>(this);

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

extern CUSDPreviewLocalServerModule g_AtlModule;
extern HMODULE g_hInstance;

