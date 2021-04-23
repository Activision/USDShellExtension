#include "stdafx.h"
#include "ArResolverShellExtension.h"

// NOTE
// /Zc:inline- is set for this file in the vcxproj
// Enabling /Zc:inline strips out USD plug-in registration
// https://developercommunity.visualstudio.com/t/zcinline-removes-extern-symbols-inside-anonymous-n/914943

PXR_NAMESPACE_OPEN_SCOPE

AR_DEFINE_RESOLVER(ArResolverShellExtension, ArResolver)

std::shared_ptr<ArAsset> ArResolverShellExtension::OpenAsset(const std::string& resolvedPath)
{
	// Allow shared reads.
	// The shell extension is used by many processes and threads at the same time. 
	// fopen on Windows will lock a file down for exclusive read.
	FILE* f = _wfsopen( ATL::CA2W( resolvedPath.c_str(), CP_UTF8 ), L"rb", _SH_DENYWR );
	if (!f) {
		return nullptr;
	}

	return std::shared_ptr<ArAsset>(new ArFilesystemAsset(f));
}

PXR_NAMESPACE_CLOSE_SCOPE

