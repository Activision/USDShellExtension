#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOMINMAX

#define GDIPVER 0x0110

#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0A00
#include <SDKDDKVer.h>

#include <windows.h>
#include <shellapi.h>
#include <shellscalingapi.h>

#pragma warning( push )
#pragma warning( disable : 4244 4305 5033 4100 4201 4245 4127 )

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/sdf/tokens.h>
#include <pxr/base/plug/registry.h>
#include <pxr/usd/usdUtils/introspection.h>
#include <pxr/usd/sdf/fileFormat.h>
#include <pxr/usd/usd/usdFileFormat.h>
#include <pxr/usd/ar/defaultResolver.h>
#include <pxr/usd/ar/defaultResolverContext.h>
#include <pxr/usd/ar/defineResolver.h>
#include <pxr/usd/ar/filesystemAsset.h>
#include <pxr/base/arch/fileSystem.h>


#pragma warning( pop )


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#include <atlbase.h>
#include <atlcom.h>
#include <atlconv.h>
#include <atlctl.h>
#include <atlstr.h>
#include <atlimage.h>
#include <atlcomtime.h>

#include <shlobj_core.h>
#include <shobjidl_core.h>
#include <pathcch.h>
#include <thumbcache.h>
#include <propkey.h>
#include <propvarutil.h>

#include <stdint.h>
#include <iomanip>

#include <gdiplus.h>