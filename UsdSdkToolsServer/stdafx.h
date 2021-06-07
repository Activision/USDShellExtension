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

#ifndef STRICT
#define STRICT
#endif

#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0A00
#include <SDKDDKVer.h>

#define _ATL_APARTMENT_THREADED
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
// Use the C++ standard templated min/max
#define NOMINMAX
#define NOBITMAP
// Include <mcx.h> if you need this
#define NOMCX
// Include <winsvc.h> if you need this
#define NOSERVICE

#include <windows.h>
#include <PathCch.h>

#pragma warning( push )
#pragma warning( disable : 4244 4305 5033 4100 4201 4245 4127 )

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/sdf/tokens.h>
#include <pxr/base/plug/registry.h>
#include <pxr/usd/usdUtils/introspection.h>
#include <pxr/usd/sdf/fileFormat.h>
#include <pxr/usd/usd/usdFileFormat.h>
#include <pxr/usd/usd/usdaFileFormat.h>
#include <pxr/usd/ar/defaultResolver.h>
#include <pxr/usd/ar/defaultResolverContext.h>
#include <pxr/usd/ar/defineResolver.h>
#include <pxr/usd/ar/filesystemAsset.h>
#include <pxr/usd/usdUtils/dependencies.h>
#include <pxr/base/arch/fileSystem.h>


#pragma warning( pop )

#include <atlbase.h>
#include <atlcom.h>
#include <atlconv.h>
#include <atlctl.h>
#include <atlstr.h>
#include <atlsafe.h>
#include <comutil.h>

#include <stdint.h>
#include <conio.h>
#include <iostream>
#include <fstream>