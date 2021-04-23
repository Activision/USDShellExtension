#pragma once

enum eUsdPreviewEvent
{
	USDPREVIEWEVENT_INVALID,
	USDPREVIEWEVENT_QUIT,
	USDPREVIEWEVENT_RESIZE,
	USDPREVIEWEVENT_RESIZERECT,
	USDPREVIEWEVENT_SETWINDOW,
};

struct UsdPreviewEventData
{
	eUsdPreviewEvent event;
	intptr_t data1;
	intptr_t data2;
};

typedef void (*FNUSDPREVIEWPUSHEVENT)(eUsdPreviewEvent event, intptr_t data1, intptr_t data2);

typedef HWND (*FNUSDPREVIEWGETPREVIEWWINDOW)();