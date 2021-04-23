#include "stdafx.h"
#include "UsdMetadata.h"
#include "UsdPropertyKeys.h"
#include "UsdMetadata_Atvi.h"

HRESULT ReadUsdMetadata( const pxr::SdfLayerRefPtr &rootLayer, const pxr::VtDictionary &metaData, IPropertyStoreCache* pPropertyStoreCache )
{
	HRESULT hr;

	std::string sComment = rootLayer->GetComment();
	hr = StoreStringValue( pPropertyStoreCache, PKEY_Comment, sComment.c_str() );

	std::string sDocumentation = rootLayer->GetDocumentation();
	hr = StoreStringValue( pPropertyStoreCache, PKEY_USD_DOCUMENTATION, sDocumentation.c_str() );

	hr = ReadUsdMetadata_Atvi( rootLayer, metaData, pPropertyStoreCache );
	if ( FAILED( hr ) )
		return hr;

	return S_OK;
}

HRESULT WriteUsdMetadata( pxr::SdfLayerRefPtr &rootLayer, pxr::VtDictionary &metaData, IPropertyStoreCache *pPropertyStoreCache, bool &bIsDirty )
{
	HRESULT hr;
	PSC_STATE state;

	hr = pPropertyStoreCache->GetState( PKEY_Comment, &state);
	if ( SUCCEEDED( hr ) && state == PSC_DIRTY )
	{
		std::string sValue;
		hr = GetValueAsUTF8( pPropertyStoreCache, PKEY_Comment, sValue );
		if ( SUCCEEDED( hr ) )
		{
			rootLayer->SetComment( sValue );
			bIsDirty = true;
		}
	}

	hr = pPropertyStoreCache->GetState( PKEY_USD_DOCUMENTATION, &state);
	if ( SUCCEEDED( hr ) && state == PSC_DIRTY )
	{
		std::string sValue;
		hr = GetValueAsUTF8( pPropertyStoreCache, PKEY_USD_DOCUMENTATION, sValue );
		if ( SUCCEEDED( hr ) )
		{
			rootLayer->SetDocumentation( sValue );
			bIsDirty = true;
		}
	}

	hr = WriteUsdMetadata_Atvi( rootLayer, metaData, pPropertyStoreCache, bIsDirty );
	if ( FAILED( hr ) )
		return hr;

	return S_OK;
}

HRESULT IsMetadataPropertyWritable( REFPROPERTYKEY key )
{
	HRESULT hr;

	if ( IsEqualPropertyKey( key, PKEY_Comment ) /*||
		 IsEqualPropertyKey( key, PKEY_USD_DOCUMENTATION )*/ )
		return S_OK;

	hr = IsMetadataPropertyWritable_Atvi( key );
	if ( hr == S_OK )
		return hr;

	return S_FALSE;
}