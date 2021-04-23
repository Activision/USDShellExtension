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