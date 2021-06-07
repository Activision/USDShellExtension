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

HRESULT ReadUsdMetadata( const pxr::SdfLayerRefPtr &rootLayer, const pxr::VtDictionary &customLayerData, IPropertyStoreCache *pPropertyStoreCache );
HRESULT WriteUsdMetadata( pxr::SdfLayerRefPtr &rootLayer, pxr::VtDictionary &customLayerData, IPropertyStoreCache *pPropertyStoreCache, bool &bIsDirty );
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
