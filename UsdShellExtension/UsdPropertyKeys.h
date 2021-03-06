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

#define INITGUID
#include <propkeydef.h>

// {3B7AB16C-C876-461C-BBA5-2F1182A834D1}
DEFINE_PROPERTYKEY(PKEY_USD_PROPGROUP, 0x3b7ab16c, 0xc876, 0x461c, 0xbb, 0xa5, 0x2f, 0x11, 0x82, 0xa8, 0x34, 0xd1, PID_FIRST_USABLE + 0);

// {3CEF792B-F66F-42E1-A59D-7BE261E682AE}
DEFINE_PROPERTYKEY(PKEY_USD_DOCUMENTATION, 0x3cef792b, 0xf66f, 0x42e1, 0xa5, 0x9d, 0x7b, 0xe2, 0x61, 0xe6, 0x82, 0xae, PID_FIRST_USABLE + 1);

// {89379215-052D-429D-9A74-6D220E41C86D}
DEFINE_PROPERTYKEY(PKEY_USD_CUSTOMLAYERDATA, 0x89379215, 0x52d, 0x429d, 0x9a, 0x74, 0x6d, 0x22, 0xe, 0x41, 0xc8, 0x6d, PID_FIRST_USABLE + 2);


#undef INITGUID