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

#include "UsdPythonToolsLocalServer_h.h"

#include "resource.h"

class ATL_NO_VTABLE CUsdPythonToolsImpl :
	public IDispatchImpl<IUsdPythonTools, &IID_IUsdPythonTools, &LIBID_UsdPythonToolsLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUsdPythonToolsImpl, &CLSID_UsdPythonTools>
{
public:
	DECLARE_NOT_AGGREGATABLE(CUsdPythonToolsImpl)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP( CUsdPythonToolsImpl )
		COM_INTERFACE_ENTRY( IUsdPythonTools )
		COM_INTERFACE_ENTRY2( IDispatch, IUsdPythonTools )
	END_COM_MAP()

	HRESULT FinalConstruct();
	void FinalRelease();

	STDMETHODIMP Record( IN BSTR usdStagePath, IN int imageWidth, IN BSTR renderer, OUT BSTR *outputImagePath ) override;
	STDMETHODIMP View( IN BSTR usdStagePath, IN BSTR renderer ) override;

	static HRESULT WINAPI UpdateRegistry(_In_ BOOL bRegister) throw();

private:
};

OBJECT_ENTRY_AUTO( __uuidof(UsdPythonTools), CUsdPythonToolsImpl )