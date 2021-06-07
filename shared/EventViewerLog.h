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

#include "EventViewerMessages.h"

#define EVENTLOG_SOURCE "Activision USD Shell Extension"

enum class LogEventType
{
	Error = EVENTLOG_ERROR_TYPE,
	Warning = EVENTLOG_WARNING_TYPE,
	Information = EVENTLOG_INFORMATION_TYPE,
};

inline bool LogEventMessage(WORD category, LPCTSTR sEventMessage, LogEventType type)
{
	bool bSuccess = false;

	DWORD eventId;
	switch ( type )
	{
	case LogEventType::Information:
		eventId = MSG_GENERIC_INFORMATION;
		break;
	case LogEventType::Warning:
		eventId = MSG_GENERIC_WARNING;
		break;
	case LogEventType::Error:
		eventId = MSG_GENERIC_ERROR;
		break;
	default:
		type = LogEventType::Error;
		eventId = MSG_GENERIC_ERROR;
		break;
	}

	HANDLE hEventSource = ::RegisterEventSource( nullptr, _T(EVENTLOG_SOURCE) );
	if ( hEventSource )
	{
		bSuccess = (::ReportEvent( hEventSource, static_cast<WORD>(type), category, eventId, nullptr, 1, 0, &sEventMessage, nullptr ) != FALSE);
		::DeregisterEventSource( hEventSource );
	}

	return bSuccess;
}