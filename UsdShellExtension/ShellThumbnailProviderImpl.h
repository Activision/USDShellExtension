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

