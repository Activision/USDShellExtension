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

#include "resource.h"

class CStageViewWnd : public CWindowImpl<CStageViewWnd>
{
public:
	void Init( HWND hWndToSubclass );
	void Term();

private:

	BEGIN_MSG_MAP(CStageViewWnd)
	END_MSG_MAP()
};