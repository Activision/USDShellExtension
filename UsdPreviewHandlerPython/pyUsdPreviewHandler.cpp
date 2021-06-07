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

#include "stdafx.h"
#include "UsdPreviewHandlerEvent.h"
#include "UsdPreviewHandlerMgr.h"

using namespace boost::python;

struct UsdPreviewApp
{
	void SetParent( uint64_t nWndParent, uint64_t nWndChild, uint64_t nWndStageView );
	UsdPreviewEventData PeekEvent();
	void LoadComplete();
};

BOOST_PYTHON_MODULE( UsdPreviewHandler )
{
	using namespace boost::python;

	enum_<eUsdPreviewEvent>( "UsdPreviewEvent" )
		.value( "NoMoreEvents", USDPREVIEWEVENT_INVALID)
		.value( "Quit", USDPREVIEWEVENT_QUIT)
		;

	class_<UsdPreviewEventData>( "UsdPreviewEventData", "help", no_init )
		.def_readonly("event", &UsdPreviewEventData::event)
		.def_readonly("data1", &UsdPreviewEventData::data1)
		.def_readonly("data2", &UsdPreviewEventData::data2)
		;

	class_<UsdPreviewApp>( "UsdPreviewApp", "help", init<>() )
		.def( "SetParent", &UsdPreviewApp::SetParent )
		.def( "PeekEvent", &UsdPreviewApp::PeekEvent )
		.def( "LoadComplete", &UsdPreviewApp::LoadComplete )
		;
}

void UsdPreviewApp::SetParent( uint64_t nWndParent, uint64_t nWndChild, uint64_t nWndStageView )
{
	HWND hWndParent = reinterpret_cast<HWND>(nWndParent);
	HWND hWndChild = reinterpret_cast<HWND>(nWndChild);
	HWND hWndStageView = reinterpret_cast<HWND>(nWndStageView);

	CUsdPreviewHandlerMgr::GetSingleton().SetParent(hWndParent, hWndChild, hWndStageView);
}

UsdPreviewEventData UsdPreviewApp::PeekEvent()
{
	return CUsdPreviewHandlerMgr::GetSingleton().PeekEvent();
}

void UsdPreviewApp::LoadComplete()
{
	CUsdPreviewHandlerMgr::GetSingleton().LoadComplete();
}

