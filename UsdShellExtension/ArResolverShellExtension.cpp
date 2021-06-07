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
#include "ArResolverShellExtension.h"

// NOTE
// /Zc:inline- is set for this file in the vcxproj
// Enabling /Zc:inline strips out USD plug-in registration
// https://developercommunity.visualstudio.com/t/zcinline-removes-extern-symbols-inside-anonymous-n/914943

PXR_NAMESPACE_OPEN_SCOPE

AR_DEFINE_RESOLVER(ArResolverShellExtension, ArResolver)

std::shared_ptr<ArAsset> ArResolverShellExtension::OpenAsset(const std::string& resolvedPath)
{
	// Deny writes.
	// The shell extension is used by many processes and threads at the same time. 
	// fopen on Windows will allow writes with reads
	FILE* f = _wfsopen( ATL::CA2W( resolvedPath.c_str(), CP_UTF8 ), L"rb", _SH_SECURE );
	if (!f) {
		return nullptr;
	}

	return std::shared_ptr<ArAsset>(new ArFilesystemAsset(f));
}

PXR_NAMESPACE_CLOSE_SCOPE

