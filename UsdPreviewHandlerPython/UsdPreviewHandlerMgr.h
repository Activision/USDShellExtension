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