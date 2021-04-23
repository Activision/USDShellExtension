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

