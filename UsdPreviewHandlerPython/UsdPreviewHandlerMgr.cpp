#include "stdafx.h"
#include "UsdPreviewHandlerMgr.h"
#include "UsdPreviewHandlerEvent.h"

CUsdPreviewHandlerMgr CUsdPreviewHandlerMgr::s_Singleton;

UsdPreviewEventData CUsdPreviewHandlerMgr::PeekEvent()
{
	UsdPreviewEventData eventData = {};

	std::lock_guard<std::mutex> guard(m_EventQueueMutex);
	if (m_EventQueue.empty())
		return eventData;

	eventData = m_EventQueue.front();
	m_EventQueue.pop();

	return eventData;
}

void CUsdPreviewHandlerMgr::LoadComplete()
{
	m_bLoadComplete = true;

	if( m_bQuit == false )
		SetParent( m_hWndParent, m_hWndChild, m_hWndStageView );
}

void CUsdPreviewHandlerMgr::PostEvent(eUsdPreviewEvent event, intptr_t data1, intptr_t data2)
{
	switch (event)
	{
	case USDPREVIEWEVENT_RESIZE:
		OnResize(static_cast<int>(data1), static_cast<int>(data2));
		return;
	case USDPREVIEWEVENT_RESIZERECT:
		OnResizeRect(*reinterpret_cast<RECT*>(data1));
		return;
	case USDPREVIEWEVENT_SETWINDOW:
		OnResizeRect(*reinterpret_cast<RECT*>(data2));
		break;
	case USDPREVIEWEVENT_QUIT:
		OnQuit();
		break;
	}

	UsdPreviewEventData eventData = {};
	eventData.event = event;
	eventData.data1 = data1;
	eventData.data2 = data2;

	std::lock_guard<std::mutex> guard(m_EventQueueMutex);
	m_EventQueue.push(std::move(eventData));
}

void CUsdPreviewHandlerMgr::SetParent(HWND hWndParent, HWND hWndChild, HWND hWndStageView)
{
	if ( m_bLoadComplete )
	{
		// The parent passed in here is actually the load screen dialog.
		// We want to swap the load screen dialog here with our dialog.

		HWND hWndLoadScreen = hWndParent;

		RECT rcClient;
		::GetClientRect( hWndLoadScreen, &rcClient );

		HWND hWndPreviewParent = ::GetParent( hWndLoadScreen );
		::SetParent( hWndChild, hWndPreviewParent );

		::SetWindowLong( hWndChild, GWL_STYLE, WS_POPUP|WS_CLIPCHILDREN );
		::SetWindowPos( hWndChild, HWND_TOP, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, SWP_FRAMECHANGED );
		::ShowWindow( hWndChild, SW_SHOW );

		::ShowWindow( hWndLoadScreen, SW_HIDE );

		m_StageViewWnd.Init( hWndStageView );
	}

	m_hWndParent = hWndParent;
	m_hWndChild = hWndChild;
	m_hWndStageView = hWndStageView;
}

void CUsdPreviewHandlerMgr::OnResize(int cx, int cy)
{
	if (m_bLoadComplete && m_hWndChild != nullptr)
		::SetWindowPos(m_hWndChild, nullptr, 0, 0, cx, cy, 0);
}

void CUsdPreviewHandlerMgr::OnResizeRect(const RECT& rc)
{
	if (m_bLoadComplete && m_hWndChild != nullptr)
		::SetWindowPos(m_hWndChild, nullptr, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 0);
}

void CUsdPreviewHandlerMgr::OnQuit()
{
	m_bQuit = true;

	// clear out our data
	m_hWndChild = nullptr;
	m_hWndParent = nullptr;
	m_hWndStageView = nullptr;
}

HWND CUsdPreviewHandlerMgr::GetPreviewWindow() const
{
	return m_hWndChild;
}

bool CUsdPreviewHandlerMgr::IsValid() const
{
	return m_hWndChild != nullptr;
}

// pushes an event to Python
extern "C" __declspec(dllexport) 
void UsdPreviewPushEvent(eUsdPreviewEvent event, intptr_t data1, intptr_t data2)
{
	CUsdPreviewHandlerMgr::GetSingleton().PostEvent(event, data1, data2);
}

// pushes an event to Python
extern "C" __declspec(dllexport)
HWND UsdPreviewGetPreviewWindow()
{
	return CUsdPreviewHandlerMgr::GetSingleton().GetPreviewWindow();
}