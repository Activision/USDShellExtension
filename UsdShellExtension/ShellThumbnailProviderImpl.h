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


class ATL_NO_VTABLE CShellThumbnailProviderImpl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CShellThumbnailProviderImpl, &CLSID_ShellThumbnailProvider>,
	public IInitializeWithFile,
	public IThumbnailProvider
{
public:
	DECLARE_NOT_AGGREGATABLE(CShellThumbnailProviderImpl)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CShellThumbnailProviderImpl)
		COM_INTERFACE_ENTRY(IInitializeWithFile)
		COM_INTERFACE_ENTRY(IThumbnailProvider)
	END_COM_MAP()

	HRESULT FinalConstruct();
	void FinalRelease();

	// IInitializeWithFile
	STDMETHODIMP Initialize( __RPC__in_string LPCWSTR pszFilePath, DWORD grfMode ) override;

	// IThumbnailProvider
	STDMETHODIMP GetThumbnail( UINT cx, __RPC__deref_out_opt HBITMAP *phbmp, __RPC__out WTS_ALPHATYPE *pdwAlpha) override;

	static HRESULT WINAPI UpdateRegistry(_In_ BOOL bRegister) throw();

private:
	CStringW m_usdStagePath;
};

OBJECT_ENTRY_AUTO(__uuidof(ShellThumbnailProvider), CShellThumbnailProviderImpl)

