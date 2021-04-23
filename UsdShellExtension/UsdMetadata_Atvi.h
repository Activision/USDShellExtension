#pragma once

HRESULT ReadUsdMetadata_Atvi( const pxr::SdfLayerRefPtr &rootLayer, const pxr::VtDictionary &customLayerData, IPropertyStoreCache *pPropertyStoreCache );
HRESULT WriteUsdMetadata_Atvi( pxr::SdfLayerRefPtr &rootLayer, pxr::VtDictionary &customLayerData, IPropertyStoreCache *pPropertyStoreCache, bool &bIsDirty );
HRESULT IsMetadataPropertyWritable_Atvi( REFPROPERTYKEY key );
