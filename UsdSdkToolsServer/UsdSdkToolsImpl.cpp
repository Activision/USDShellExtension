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
#include "UsdSdkToolsImpl.h"
#include "Module.h"
#include "shared\environment.h"
#include "shared\EventViewerLog.h"

#include <vector>

HRESULT CUsdSdkToolsImpl::FinalConstruct()
{
	SetupPythonEnvironment( g_hInstance );

	return __super::FinalConstruct();
}

void CUsdSdkToolsImpl::FinalRelease()
{
	__super::FinalRelease();
}

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

	pxr::ArSetPreferredResolver( "ArResolverShellExtension" );
}

STDMETHODIMP CUsdSdkToolsImpl::Cat( IN BSTR usdStagePathInput, IN BSTR usdStagePathOuput, IN eUsdFormat formatOutput, IN VARIANT_BOOL flatten )
{
	DEBUG_RECORD_ENTRY();

	RegisterUsdPlugins();

	pxr::SdfLayer::FileFormatArguments fileFormat;
	if ( formatOutput == USD_FORMAT_USDA )
		fileFormat["target"] = "usda";
	else if ( formatOutput == USD_FORMAT_USDC )
		fileFormat["target"] = "usdc";

	if ( flatten == VARIANT_FALSE )
	{
		std::string usdStagePathInputA = static_cast<LPCSTR>(ATL::CW2A( usdStagePathInput, CP_UTF8 ));
		pxr::SdfLayerRefPtr rootLayer = pxr::SdfLayer::OpenAsAnonymous( usdStagePathInputA, false );
		if ( rootLayer == nullptr )
			return E_FAIL;

		std::string usdStagePathOuputA = static_cast<LPCSTR>(ATL::CW2A( usdStagePathOuput, CP_UTF8 ));
		if ( !rootLayer->Export( usdStagePathOuputA, std::string(), fileFormat ) )
			return E_FAIL;
	}
	else
	{
		std::string usdStagePathInputA = static_cast<LPCSTR>(ATL::CW2A( usdStagePathInput, CP_UTF8 ));
		pxr::UsdStageRefPtr stage = pxr::UsdStage::Open( usdStagePathInputA );
		if ( stage == nullptr )
			return E_FAIL;

		stage->Flatten();

		std::string usdStagePathOuputA = static_cast<LPCSTR>(ATL::CW2A( usdStagePathOuput, CP_UTF8 ));
		if ( !stage->Export( usdStagePathOuputA, true, fileFormat ) )
			return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CUsdSdkToolsImpl::Edit( IN BSTR usdStagePath, IN VARIANT_BOOL force )
{
	DEBUG_RECORD_ENTRY();

	RegisterUsdPlugins();

	DWORD inputFileAttribs = GetFileAttributes( usdStagePath );
	if ( inputFileAttribs == INVALID_FILE_ATTRIBUTES )
		return E_INVALIDARG;

	std::string usdStagePathA = static_cast<LPCSTR>(ATL::CW2A( usdStagePath, CP_UTF8 ));
	pxr::SdfLayerRefPtr rootLayer = pxr::SdfLayer::OpenAsAnonymous( usdStagePathA, false );
	if ( rootLayer == nullptr )
		return E_FAIL;

	CStringW usdStagePathOuputW;
	usdStagePathOuputW = usdStagePath;
	usdStagePathOuputW += L"-edit.usda";

	std::string exportString;
	if ( !rootLayer->ExportToString( &exportString ) )
		return E_FAIL;

	std::ofstream fileOut(usdStagePathOuputW.GetString(), std::ofstream::out|std::ofstream::trunc);
	if (!fileOut.is_open())
		return E_FAIL;

	fileOut << exportString;

	fileOut.close();


	WIN32_FILE_ATTRIBUTE_DATA wfadBefore = {};
	::GetFileAttributesEx( usdStagePathOuputW, GetFileExInfoStandard, &wfadBefore );

	// hide the file we're editing
	::SetFileAttributes( usdStagePathOuputW, wfadBefore.dwFileAttributes | FILE_ATTRIBUTE_HIDDEN );

	CStringW sEditor = GetUsdEditor();
	if ( sEditor.IsEmpty() )
		sEditor = L"notepad.exe";

	CStringW sCommandLine;
	sCommandLine.Format( L"%s \"%s\"", sEditor.GetString(), usdStagePathOuputW.GetString() );

	STARTUPINFO si = {};
	si.cb = sizeof( si );
	PROCESS_INFORMATION pi = {};
	if ( !::CreateProcess( nullptr, sCommandLine.GetBuffer(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi ) )
	{
		return E_FAIL;
	}

	::WaitForSingleObject( pi.hProcess, INFINITE );

	::CloseHandle( pi.hProcess );
	::CloseHandle( pi.hThread );

	WIN32_FILE_ATTRIBUTE_DATA wfadAfter = {};
	::GetFileAttributesEx( usdStagePathOuputW, GetFileExInfoStandard, &wfadAfter );

	if ( ::CompareFileTime( &wfadAfter.ftLastWriteTime, &wfadBefore.ftLastWriteTime ) != 0 )
	{
		if ( (inputFileAttribs & FILE_ATTRIBUTE_READONLY) )
		{
			if ( force == VARIANT_FALSE )
			{
				::DeleteFileW( usdStagePathOuputW );
				return S_OK;
			}

			::SetFileAttributes( usdStagePath, inputFileAttribs & ~FILE_ATTRIBUTE_READONLY );
		}

		std::ifstream fileIn(usdStagePathOuputW.GetString(), std::ifstream::in);
		if (!fileIn.is_open())
			return E_FAIL;

		std::ostringstream importString;
		importString << fileIn.rdbuf();
		fileIn.close();

		if ( rootLayer->ImportFromString( importString.str() ) == false )
			return E_FAIL;

		if ( rootLayer->Export( usdStagePathA ) == false )
			return E_FAIL;
	}

	::DeleteFileW( usdStagePathOuputW );

	return S_OK;
}

static void pause()
{
	std::cout << std::endl;
	std::cout << "Press any key to continue..." << std::endl;
	_getch();
}

STDMETHODIMP CUsdSdkToolsImpl::Package( IN BSTR usdStagePathInput, IN BSTR usdStagePathOuput, IN eUsdPackageType packageType, IN VARIANT_BOOL verbose )
{
	DEBUG_RECORD_ENTRY();

	// create and display a console
	if (AllocConsole())
	{
		FILE *fout = nullptr;
		freopen_s(&fout, "CONOUT$", "w", stdout);
		FILE *ferr = nullptr;
		freopen_s(&ferr, "CONOUT$", "w", stderr);
	}

	SetConsoleTitleW( L"USD Package" );

	// ask the user to press a key to exit
	atexit( pause );

	RegisterUsdPlugins();

	if ( verbose != VARIANT_FALSE )
		pxr::TfDebug::SetDebugSymbolsByName( "USDUTILS_CREATE_USDZ_PACKAGE", 1 );

	DWORD inputFileAttribs = GetFileAttributes( usdStagePathInput );
	if ( inputFileAttribs == INVALID_FILE_ATTRIBUTES )
		return E_INVALIDARG;

	std::string usdStagePathInputA = static_cast<LPCSTR>(ATL::CW2A( usdStagePathInput, CP_UTF8 ));
	std::string usdStagePathOuputA = static_cast<LPCSTR>(ATL::CW2A( usdStagePathOuput, CP_UTF8 ));

	if ( packageType == USD_PACKAGE_DEFAULT )
	{
		if ( !pxr::UsdUtilsCreateNewUsdzPackage( pxr::SdfAssetPath( usdStagePathInputA ), usdStagePathOuputA ) )
			return E_FAIL;
	}
	else if( packageType == USD_FORMAT_APPLE_ARKIT )
	{
		if ( !pxr::UsdUtilsCreateNewARKitUsdzPackage( pxr::SdfAssetPath( usdStagePathInputA ), usdStagePathOuputA ) )
			return E_FAIL;
	}
	else
	{
		return E_INVALIDARG;
	}

	return S_OK;
}

static void PrintDictionary( const pxr::VtDictionary &dict, int indent )
{
	for ( const std::pair<std::string, pxr::VtValue>& stat : dict )
	{
		for ( int i = 0; i < indent; ++i )
			std::cout << "  ";

		if ( stat.second.GetTypeid() == typeid(pxr::VtDictionary) )
		{
			std::cout << stat.first << std::endl;
			pxr::VtDictionary nestedDict = stat.second.Get<pxr::VtDictionary>();
			PrintDictionary( nestedDict, indent + 1 );
		}
		else
		{
			if ( stat.second.GetTypeid() == typeid(size_t) )
				std::cout << stat.first << " = " << stat.second.Get<size_t>() << std::endl;
			else if ( stat.second.GetTypeid() == typeid(double) )
				std::cout << stat.first << " = " << stat.second.Get<double>() << std::endl;
			else
				std::cout << stat.first << " = " << "[UNKNOWN TYPE]" << std::endl;
		}
	}
}

STDMETHODIMP CUsdSdkToolsImpl::DisplayStageStats( IN BSTR usdStagePath )
{
	DEBUG_RECORD_ENTRY();

	// create and display a console
	if (AllocConsole())
	{
		FILE *fout = nullptr;
		freopen_s(&fout, "CONOUT$", "w", stdout);
		FILE *ferr = nullptr;
		freopen_s(&ferr, "CONOUT$", "w", stderr);
	}

	SetConsoleTitleW( L"USD Stage Stats" );

	// ask the user to press a key to exit
	atexit( pause );

	RegisterUsdPlugins();

	std::string sError;
	pxr::TfMallocTag::Initialize( &sError );

	std::string usdStagePathA = static_cast<LPCSTR>(ATL::CW2A( usdStagePath, CP_UTF8 ));

	std::cout << usdStagePathA << std::endl;
	std::cout << std::endl;

	pxr::VtDictionary dictStats;
	pxr::UsdStageRefPtr stage = pxr::UsdUtilsComputeUsdStageStats( usdStagePathA, &dictStats );
	if ( stage == nullptr )
		return E_FAIL;

	PrintDictionary( dictStats, 0 );

	return S_OK;
}

HRESULT WINAPI CUsdSdkToolsImpl::UpdateRegistry(_In_ BOOL bRegister) throw()
{
	ATL::_ATL_REGMAP_ENTRY regMapEntries[] =
	{
		{ L"APPID", L"{123A65E6-B4B4-4B46-BEF5-D0FCE7173261}" },
		{ L"CLSID_USDSDKTOOLS", L"{5F016739-AF12-4899-B710-3FB5C242A11D}" },
		{ nullptr, nullptr }
	};

	return g_AtlModule.UpdateRegistryFromResource(IDR_REGISTRY_USDTOOLSIMPL, bRegister, regMapEntries);
}