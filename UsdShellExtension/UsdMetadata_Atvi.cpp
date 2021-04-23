#include "stdafx.h"
#include "UsdMetadata.h"
#include "UsdPropertyKeys_Atvi.h"

#define EXPORT_TIME pxr::TfToken( "atvi:exportTime" )
#define EXPORT_FILENAME pxr::TfToken( "atvi:exportFileName" )
#define EXPORT_APPLICATION pxr::TfToken( "atvi:exportApplication" )
#define EXPORT_USER pxr::TfToken( "atvi:exportUser" )
#define EXPORT_SOURCE pxr::TfToken( "atvi:sourceFileName" )

HRESULT ReadUsdMetadata_Atvi( const pxr::SdfLayerRefPtr &rootLayer, const pxr::VtDictionary& customLayerData, IPropertyStoreCache* pPropertyStoreCache )
{
	UNREFERENCED_PARAMETER( rootLayer );

	HRESULT hr;
	const pxr::VtValue* pValue = nullptr;

	pValue = customLayerData.GetValueAtPath( EXPORT_SOURCE );
	if( pValue )
	{
		hr = StoreStringValue( pPropertyStoreCache, PKEY_USD_ATVI_SOURCEFILENAME, pValue->Get<std::string>().c_str() );
	}

	pValue = customLayerData.GetValueAtPath( EXPORT_APPLICATION );
	if( pValue )
	{
		hr = StoreStringValue( pPropertyStoreCache, PKEY_USD_ATVI_EXPORTAPPLICATION, pValue->Get<std::string>().c_str() );
	}

	pValue = customLayerData.GetValueAtPath( EXPORT_FILENAME );
	if( pValue )
	{
		hr = StoreStringValue( pPropertyStoreCache, PKEY_USD_ATVI_EXPORTFILENAME, pValue->Get<std::string>().c_str() );
	}

	pValue = customLayerData.GetValueAtPath( EXPORT_TIME );
	if( pValue )
	{
		std::istringstream ivalue( pValue->Get<std::string>() );
		//ivalue.imbue( std::locale( setlocale( LC_ALL, nullptr ) ) );

		std::tm timeTM = {};
		ivalue >> std::get_time( &timeTM, "%Y-%m-%d %H:%M" );
		std::time_t timeEpoch = std::mktime( &timeTM );

		COleDateTime timeOleDateTime(timeEpoch);
		
		SYSTEMTIME timeSystemTime;
		timeOleDateTime.GetAsSystemTime( timeSystemTime );

		FILETIME timeFileTime;
		SystemTimeToFileTime( &timeSystemTime, &timeFileTime );

		hr = StoreDateValue( pPropertyStoreCache, PKEY_USD_ATVI_EXPORTDATE, &timeFileTime );
	}

	pValue = customLayerData.GetValueAtPath( EXPORT_USER );
	if ( pValue )
	{
		hr = StoreStringValue( pPropertyStoreCache, PKEY_USD_ATVI_EXPORTUSER, pValue->Get<std::string>().c_str() );
	}

	return S_OK;
}

HRESULT WriteUsdMetadata_Atvi( pxr::SdfLayerRefPtr &rootLayer, pxr::VtDictionary& customLayerData, IPropertyStoreCache *pPropertyStoreCache, bool &bIsDirty )
{
	UNREFERENCED_PARAMETER( rootLayer );
	UNREFERENCED_PARAMETER( customLayerData );
	UNREFERENCED_PARAMETER( pPropertyStoreCache );
	UNREFERENCED_PARAMETER( bIsDirty );

	// All ATVI metadata is read-only
	return S_OK;
}

HRESULT IsMetadataPropertyWritable_Atvi( REFPROPERTYKEY key )
{
	UNREFERENCED_PARAMETER( key );

	// All ATVI metadata is read-only
	return S_FALSE;
}