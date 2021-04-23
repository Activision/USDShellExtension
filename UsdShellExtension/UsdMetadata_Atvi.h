#pragma once

HRESULT ReadUsdMetadata_Atvi( const pxr::SdfLayerRefPtr &rootLayer, const pxr::VtDictionary &metaData, IPropertyStoreCache *pPropertyStoreCache );
HRESULT WriteUsdMetadata_Atvi( pxr::SdfLayerRefPtr &rootLayer, pxr::VtDictionary &metaData, IPropertyStoreCache *pPropertyStoreCache, bool &bIsDirty );
HRESULT IsMetadataPropertyWritable_Atvi( REFPROPERTYKEY key );
