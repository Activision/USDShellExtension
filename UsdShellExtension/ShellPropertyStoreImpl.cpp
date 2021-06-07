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
#include "ShellPropertyStoreImpl.h"
#include "Module.h"
#include "resource.h"
#include "UsdMetadata.h"
#include "UsdPropertyKeys.h"
#include "ArResolverShellExtension.h"

static void RegisterUsdPlugins()
{
	static bool sUsdPluginsRegistered = false;
	static std::mutex sUsdPluginsRegisteredLock;

	std::lock_guard<std::mutex> guard( sUsdPluginsRegisteredLock );

	// Plugins should be registered only once per session.
	if( sUsdPluginsRegistered )
		return;

	sUsdPluginsRegistered = true;

	TCHAR sModulePath[MAX_PATH];
	GetModuleFileName( g_hInstance, sModulePath, ARRAYSIZE( sModulePath ) );

	std::vector<std::string> pathsToPlugInfo;

	// add the folder that contains the shell extension
	PathCchRemoveFileSpec( sModulePath, ARRAYSIZE( sModulePath ) );
	pathsToPlugInfo.push_back( static_cast<LPCSTR>(ATL::CW2A(sModulePath, CP_UTF8)) );

	// add the bare-bones usd plugins
	PathCchAppend( sModulePath, ARRAYSIZE( sModulePath ), L"usd" );
	pathsToPlugInfo.push_back( static_cast<LPCSTR>(ATL::CW2A(sModulePath, CP_UTF8)) );

	pxr::PlugRegistry::GetInstance().RegisterPlugins( pathsToPlugInfo );

	pxr::ArSetPreferredResolver(pxr::TfType::GetCanonicalTypeName(typeid(pxr::ArResolverShellExtension)));
}

HRESULT CShellPropertyStoreImpl::FinalConstruct()
{
	return __super::FinalConstruct();
}

void CShellPropertyStoreImpl::FinalRelease()
{
	__super::FinalRelease();
}


// IInitializeWithFile
STDMETHODIMP CShellPropertyStoreImpl::Initialize(__RPC__in_string LPCWSTR pszFilePath, DWORD grfMode)
{
	if ( pszFilePath == nullptr )
		return E_POINTER;

	m_usdStagePath = pszFilePath;
	m_grfMode = grfMode;

	DWORD nAttribs = GetFileAttributesW( pszFilePath );
	if ( nAttribs == INVALID_FILE_ATTRIBUTES )
		return E_INVALIDARG;

	// Avoid opening files that are reparse points.
	// TedFS stubs are such files. An open here would cause a resolve.
	if ( nAttribs & FILE_ATTRIBUTE_REPARSE_POINT )
		return E_FAIL;

	HRESULT hr;
	hr = PSCreateMemoryPropertyStore(IID_PPV_ARGS(&m_pPropertyStoreCache.p));
	if ( FAILED( hr ) )
		return hr;

	RegisterUsdPlugins();

	std::string pszFilePathA = static_cast<LPCSTR>(ATL::CW2A( m_usdStagePath, CP_UTF8 ));

	// open the file for just metadata
	// this is a fast read-only load
	pxr::SdfLayerRefPtr rootLayer = pxr::SdfLayer::OpenAsAnonymous( pszFilePathA, true );
	if ( rootLayer == nullptr )
	{
		return E_FAIL;
	}

	m_bIsPackage = rootLayer->GetFileFormat()->IsPackage();

	pxr::VtDictionary customLayerData = rootLayer->GetCustomLayerData();

	hr = ReadUsdMetadata( rootLayer, customLayerData, m_pPropertyStoreCache );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	return hr;
}

// IPropertyStore
STDMETHODIMP CShellPropertyStoreImpl::Commit()
{
	HRESULT hr = E_FAIL;

	if ( IsReadOnly() || IsPackage() )
		return STG_E_ACCESSDENIED;

	if ( m_pPropertyStoreCache == nullptr )
		return E_UNEXPECTED;

	RegisterUsdPlugins();

	std::string pszFilePathA = static_cast<LPCSTR>(ATL::CW2A( m_usdStagePath, CP_UTF8 ));
	pxr::SdfLayerRefPtr rootLayer = pxr::SdfLayer::FindOrOpen( pszFilePathA );
	if ( rootLayer == nullptr )
		return E_FAIL;

	// save the file back out
	bool bIsDirty = false;
	pxr::VtDictionary customLayerData = rootLayer->GetCustomLayerData();
	hr = WriteUsdMetadata( rootLayer, customLayerData, m_pPropertyStoreCache, bIsDirty );
	if ( FAILED( hr ) )
		return hr;

	// check if anything changed
	if ( bIsDirty == false )
		return S_OK;

	// save our updated metadata
	rootLayer->SetCustomLayerData( customLayerData );

	CStringW usdTempStagePath = m_usdStagePath;
	usdTempStagePath += L".tmp";

	if ( !rootLayer->Export( static_cast<LPCSTR>(ATL::CW2A( usdTempStagePath, CP_UTF8 )) ) )
		return E_FAIL;

	rootLayer = nullptr;

	if ( !::ReplaceFileW( m_usdStagePath, usdTempStagePath, nullptr, REPLACEFILE_IGNORE_MERGE_ERRORS|REPLACEFILE_IGNORE_ACL_ERRORS, nullptr, nullptr ) )
		return E_FAIL;

	return S_OK;
}

STDMETHODIMP CShellPropertyStoreImpl::GetCount( __RPC__out DWORD *cProps )
{
	if ( cProps == nullptr )
		return E_POINTER;

	HRESULT hr = E_UNEXPECTED;
	if ( m_pPropertyStoreCache )
		hr = m_pPropertyStoreCache->GetCount( cProps );

	return hr;
}

STDMETHODIMP CShellPropertyStoreImpl::GetAt( DWORD iProp, __RPC__out PROPERTYKEY *pkey )
{
	if ( pkey == nullptr )
		return E_POINTER;

	HRESULT hr = E_UNEXPECTED;
	if ( m_pPropertyStoreCache )
		hr = m_pPropertyStoreCache->GetAt( iProp, pkey );

	return hr;
}

STDMETHODIMP CShellPropertyStoreImpl::GetValue( __RPC__in REFPROPERTYKEY key, __RPC__out PROPVARIANT *pv )
{
	if ( pv == nullptr )
		return E_POINTER;

	HRESULT hr = E_UNEXPECTED;
	if ( m_pPropertyStoreCache )
		hr = m_pPropertyStoreCache->GetValue( key, pv );

	return hr;
}

STDMETHODIMP CShellPropertyStoreImpl::SetValue( __RPC__in REFPROPERTYKEY key, __RPC__in REFPROPVARIANT propvar )
{
	if ( IsReadOnly() || IsPackage() )
		return STG_E_ACCESSDENIED;

	HRESULT hr = E_UNEXPECTED;
	if ( m_pPropertyStoreCache && IsPropertyWritable(key) == S_OK )
		hr = m_pPropertyStoreCache->SetValueAndState( key, &propvar, PSC_DIRTY );

	return hr;
}

// IPropertyStoreCapabilities
STDMETHODIMP CShellPropertyStoreImpl::IsPropertyWritable( __RPC__in REFPROPERTYKEY key )
{
	if ( IsPackage() )
		return S_FALSE;

	return IsMetadataPropertyWritable( key );
}

HRESULT WINAPI CShellPropertyStoreImpl::UpdateRegistry(_In_ BOOL bRegister) throw()
{
	const wchar_t pPropertyDescription[] = 
		L"val InfoTip = s 'prop:System.ItemType;System.Size;System.DateModified'\n"
		L"val FullDetails = s 'prop:System.PropGroup.FileSystem;System.ItemNameDisplay;System.ItemType;System.ItemFolderPathDisplay;System.Size;System.ItemDate;System.DateCreated;System.DateModified;System.DateAccessed;System.FileAttributes;USD.PropGroup.USD;System.Comment;USD.Documentation;USD.CustomLayerData;USD.PropGroup.Activision'\n"
		L"val PreviewDetails = s 'prop:USD.PropGroup.USD;System.Comment;USD.Documentation;USD.PropGroup.Activision'\n"
		L"val PreviewTitle = s 'prop:System.FileName;System.ItemType'\n";

	ATL::_ATL_REGMAP_ENTRY regMapEntries[] =
	{
		{ L"CLSID_SHELLPROPERTYSTORE", L"{5CE761D0-72AA-4484-B2BF-2A0EBF3ACC0C}" },
		{ L"PROPERTYDESCRIPTIONS", pPropertyDescription },
		{ nullptr, nullptr }
	};

	return g_AtlModule.UpdateRegistryFromResource(IDR_REGISTRY_SHELLPROPERTYSTOREIMPL, bRegister, regMapEntries);
}