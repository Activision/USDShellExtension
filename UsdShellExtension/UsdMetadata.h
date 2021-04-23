#pragma once

HRESULT ReadUsdMetadata( const pxr::SdfLayerRefPtr &rootLayer, const pxr::VtDictionary &metaData, IPropertyStoreCache *pPropertyStoreCache );
HRESULT WriteUsdMetadata( pxr::SdfLayerRefPtr &rootLayer, pxr::VtDictionary &metaData, IPropertyStoreCache *pPropertyStoreCache, bool &bIsDirty );
HRESULT IsMetadataPropertyWritable( REFPROPERTYKEY key );

inline HRESULT StoreStringValue( IPropertyStoreCache *pPropertyStoreCache, REFPROPERTYKEY key, LPCWSTR sValue )
{
	HRESULT hr;

	PROPVARIANT propvar = {};
	hr = InitPropVariantFromString( sValue, &propvar );
	if ( SUCCEEDED( hr ) )
	{
		hr = PSCoerceToCanonicalValue( key, &propvar );
		if ( SUCCEEDED( hr ) )
		{
			hr = pPropertyStoreCache->SetValueAndState( key, &propvar, PSC_NORMAL );
		}

		PropVariantClear( &propvar );
	}

	return hr;
}

inline HRESULT StoreStringValue( IPropertyStoreCache *pPropertyStoreCache, REFPROPERTYKEY key, LPCSTR sValue )
{
	return StoreStringValue( pPropertyStoreCache, key, ATL::CA2W( sValue, CP_UTF8 ) );
}

inline HRESULT StoreUInt64Value( IPropertyStoreCache *pPropertyStoreCache, REFPROPERTYKEY key, uint64_t nValue )
{
	HRESULT hr;

	PROPVARIANT propvar = {};
	hr = InitPropVariantFromUInt64( nValue, &propvar );
	if ( SUCCEEDED( hr ) )
	{
		hr = pPropertyStoreCache->SetValueAndState( key, &propvar, PSC_NORMAL );

		PropVariantClear( &propvar );
	}

	return hr;
}

inline HRESULT StoreDateValue( IPropertyStoreCache *pPropertyStoreCache, REFPROPERTYKEY key, const FILETIME *pft )
{
	HRESULT hr;

	PROPVARIANT propvar = {};
	hr = InitPropVariantFromFileTime( pft, &propvar );
	if ( SUCCEEDED( hr ) )
	{
		hr = pPropertyStoreCache->SetValueAndState( key, &propvar, PSC_NORMAL );

		PropVariantClear( &propvar );
	}

	return hr;
}

inline HRESULT GetValueAsUTF8( IPropertyStoreCache *pPropertyStoreCache, REFPROPERTYKEY key, std::string &value )
{
	HRESULT hr;

	PROPVARIANT propVariant;
	hr = pPropertyStoreCache->GetValue( key, &propVariant );
	if ( SUCCEEDED( hr ) )
	{
		LPCWSTR sUTF16Value = PropVariantToStringWithDefault( propVariant, L"" );
		value = ATL::CW2A( sUTF16Value, CP_UTF8 );

		return hr;
	}

	return hr;
}
