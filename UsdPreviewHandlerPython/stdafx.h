#pragma once

#pragma warning(push)
#pragma warning(disable: 4244 4459)

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/list.hpp>
#include <boost/python.hpp>

#pragma warning(pop)

#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <Uxtheme.h>

#include <atlbase.h>
#include <atlcom.h>
#include <atlconv.h>
#include <atlstr.h>
#include <atlwin.h>