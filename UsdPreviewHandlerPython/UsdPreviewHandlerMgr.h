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

#include "UsdPreviewHandlerMgr.h"
#include "UsdPreviewHandlerEvent.h"
#include "StageViewWnd.h"

#include <queue>
#include <mutex>

class CUsdPreviewHandlerMgr
{
public:
	static CUsdPreviewHandlerMgr& GetSingleton();

	UsdPreviewEventData PeekEvent();
	void PostEvent(eUsdPreviewEvent event, intptr_t data1, intptr_t data2);

	void SetParent(HWND hWndParent, HWND hWndChild, HWND hWndStageView);

	HWND GetPreviewWindow() const;
	void LoadComplete();

	bool IsValid() const;

private:
	void OnResize(int cx, int cy);
	void OnResizeRect(const RECT& rc);
	void OnQuit();

	static CUsdPreviewHandlerMgr s_Singleton;

	std::queue<UsdPreviewEventData> m_EventQueue;
	std::mutex m_EventQueueMutex;

	HWND m_hWndParent = nullptr;
	HWND m_hWndChild = nullptr;
	HWND m_hWndStageView = nullptr;

	bool m_bLoadComplete = false;
	bool m_bQuit = false;

	CStageViewWnd m_StageViewWnd;
};

inline CUsdPreviewHandlerMgr& CUsdPreviewHandlerMgr::GetSingleton()
{
	return s_Singleton;
}