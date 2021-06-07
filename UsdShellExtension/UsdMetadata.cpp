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

#include "stdafx.h"
#include "UsdMetadata.h"
#include "UsdPropertyKeys.h"

static std::string DictionaryToString(const pxr::VtDictionary &dict, std::string prefix = "")
{
	// Exports all values on separate lines
	// Format: dict.key=value

	std::string result;

	for ( const std::pair<std::string, pxr::VtValue>& stat : dict )
	{
		if ( stat.second.GetTypeid() == typeid(pxr::VtDictionary) )
		{
			pxr::VtDictionary nestedDict = stat.second.Get<pxr::VtDictionary>();
			std::string nestedPrefix = prefix;
			if ( !nestedPrefix.empty() )
				nestedPrefix += ".";
			nestedPrefix += stat.first;
			result += DictionaryToString( nestedDict, nestedPrefix );
		}
		else
		{
			std::string name = prefix;
			if ( !name.empty() )
				name += ".";
			name += stat.first;

			std::stringstream ss;
			if ( stat.second.GetTypeid() == typeid(size_t) )
				ss << name << "=" << stat.second.Get<size_t>() << std::endl;
			else if ( stat.second.GetTypeid() == typeid(double) )
				ss << name << "=" << stat.second.Get<double>() << std::endl;
			else if ( stat.second.GetTypeid() == typeid(std::string) )
				ss << name << "=" << stat.second << std::endl;
			else
				ss << name << "=" << "[UNKNOWN TYPE]" << std::endl;

			result += ss.str();
		}
	}

	return result;
}

HRESULT ReadUsdMetadata( const pxr::SdfLayerRefPtr &rootLayer, const pxr::VtDictionary &customLayerData, IPropertyStoreCache* pPropertyStoreCache )
{
	HRESULT hr;

	std::string sComment = rootLayer->GetComment();
	hr = StoreStringValue( pPropertyStoreCache, PKEY_Comment, sComment.c_str() );

	std::string sDocumentation = rootLayer->GetDocumentation();
	hr = StoreStringValue( pPropertyStoreCache, PKEY_USD_DOCUMENTATION, sDocumentation.c_str() );

	std::string sCustomLayerData = DictionaryToString( customLayerData );
	hr = StoreStringValue( pPropertyStoreCache, PKEY_USD_CUSTOMLAYERDATA, sCustomLayerData.c_str() );

	return S_OK;
}

HRESULT WriteUsdMetadata( pxr::SdfLayerRefPtr &rootLayer, pxr::VtDictionary &customLayerData, IPropertyStoreCache *pPropertyStoreCache, bool &bIsDirty )
{
	UNREFERENCED_PARAMETER( customLayerData );

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

	return S_OK;
}

HRESULT IsMetadataPropertyWritable( REFPROPERTYKEY key )
{
	if ( IsEqualPropertyKey( key, PKEY_Comment ) /*||
		 IsEqualPropertyKey( key, PKEY_USD_DOCUMENTATION )*/ )
		return S_OK;

	return S_FALSE;
}