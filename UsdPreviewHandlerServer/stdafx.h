#pragma once

#ifndef STRICT
#define STRICT
#endif

#pragma warning(push)
#pragma warning(disable: 4244 4459)

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/list.hpp>
#include <boost/python.hpp>

#pragma warning(pop)

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

#include <atlbase.h>
#include <atlcom.h>
#include <atlconv.h>
#include <atlctl.h>
#include <atlstr.h>
#include <atlsafe.h>
#include <comutil.h>

#include <stdint.h>

