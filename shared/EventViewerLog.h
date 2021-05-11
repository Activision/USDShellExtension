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