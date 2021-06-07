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

#ifndef ATVI_VERSION_DESCRIPTION
#	define ATVI_VERSION_DESCRIPTION	""
#endif

#define __ATVI_VERSION_PRODUCT_STRING_INTERNAL3(s) \
    #s

#define __ATVI_VERSION_PRODUCT_STRING_INTERNAL2(p, s, b, f) \
	__ATVI_VERSION_PRODUCT_STRING_INTERNAL3(p.s.b.f)

#define __ATVI_VERSION_PRODUCT_STRING_INTERNAL(p, s, b, f) \
	__ATVI_VERSION_PRODUCT_STRING_INTERNAL2(p, s, b, f)

#define ATVI_VERSION_PRODUCT_STRING	__ATVI_VERSION_PRODUCT_STRING_INTERNAL(ATVI_VERSION_MAJOR, ATVI_VERSION_MINOR, ATVI_VERSION_REVISION, ATVI_VERSION_BUILD)


