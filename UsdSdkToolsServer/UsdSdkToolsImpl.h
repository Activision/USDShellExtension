#pragma once

#include "UsdSdkToolsLocalServer_h.h"

#include "resource.h"

class ATL_NO_VTABLE CUsdSdkToolsImpl :
	public IDispatchImpl<IUsdSdkTools, &IID_IUsdSdkTools, &LIBID_UsdSdkToolsLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUsdSdkToolsImpl, &CLSID_UsdSdkTools>
{
public:
	DECLARE_NOT_AGGREGATABLE(CUsdSdkToolsImpl)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP( CUsdSdkToolsImpl )
		COM_INTERFACE_ENTRY( IUsdSdkTools )
		COM_INTERFACE_ENTRY2( IDispatch, IUsdSdkTools )
	END_COM_MAP()

	HRESULT FinalConstruct();
	void FinalRelease();

	STDMETHODIMP Cat( IN BSTR usdStagePathInput, IN BSTR usdStagePathOuput, IN eUsdFormat formatOutput = USD_FORMAT_INPUT, IN VARIANT_BOOL flatten = 0 ) override;
	STDMETHODIMP Edit( IN BSTR usdStagePath, IN VARIANT_BOOL force = 0 ) override;
	STDMETHODIMP Package( IN BSTR usdStagePathInput, IN BSTR usdStagePathOuput, IN eUsdPackageType packageType = USD_PACKAGE_DEFAULT, IN VARIANT_BOOL verbose = 0 ) override;
	STDMETHODIMP DisplayStageStats( IN BSTR usdStagePath ) override;

	static HRESULT WINAPI UpdateRegistry(_In_ BOOL bRegister) throw();

private:
};

OBJECT_ENTRY_AUTO( __uuidof(UsdSdkTools), CUsdSdkToolsImpl )