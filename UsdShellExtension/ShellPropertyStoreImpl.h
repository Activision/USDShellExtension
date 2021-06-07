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


class ATL_NO_VTABLE CShellPropertyStoreImpl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CShellPropertyStoreImpl, &CLSID_ShellPropertyStore>,
	public IInitializeWithFile,
	public IPropertyStore,
	public IPropertyStoreCapabilities
{
public:
	DECLARE_NOT_AGGREGATABLE(CShellPropertyStoreImpl)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CShellPropertyStoreImpl)
		COM_INTERFACE_ENTRY(IInitializeWithFile)
		COM_INTERFACE_ENTRY(IPropertyStore)
		COM_INTERFACE_ENTRY(IPropertyStoreCapabilities)
	END_COM_MAP()

	HRESULT FinalConstruct();
	void FinalRelease();

	// IInitializeWithFile
	STDMETHODIMP Initialize( __RPC__in_string LPCWSTR pszFilePath, DWORD grfMode ) override;

	// IPropertyStore
    STDMETHODIMP GetCount(  __RPC__out DWORD *cProps ) override;
    STDMETHODIMP GetAt( DWORD iProp, __RPC__out PROPERTYKEY *pkey ) override;
    STDMETHODIMP GetValue( __RPC__in REFPROPERTYKEY key, __RPC__out PROPVARIANT *pv ) override;
    STDMETHODIMP SetValue( __RPC__in REFPROPERTYKEY key, __RPC__in REFPROPVARIANT propvar ) override;
    STDMETHODIMP Commit() override;

	// IPropertyStoreCapabilities
    STDMETHODIMP IsPropertyWritable( __RPC__in REFPROPERTYKEY key) override;


	static HRESULT WINAPI UpdateRegistry(_In_ BOOL bRegister) throw();

private:
	bool IsPackage() const { return m_bIsPackage; }
	bool IsReadOnly() const { return (m_grfMode & STGM_READWRITE) == 0; }

	CStringW m_usdStagePath;
	DWORD m_grfMode = 0;
	bool m_bIsPackage = false;

	CComPtr<IPropertyStoreCache> m_pPropertyStoreCache;

};

OBJECT_ENTRY_AUTO(__uuidof(ShellPropertyStore), CShellPropertyStoreImpl)

