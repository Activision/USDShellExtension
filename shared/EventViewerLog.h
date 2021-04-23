#pragma once

enum class LogEventType
{
	Error = EVENTLOG_ERROR_TYPE,
	Warning = EVENTLOG_WARNING_TYPE,
	Information = EVENTLOG_INFORMATION_TYPE,
};

inline bool LogEventMessage(LPCTSTR sEventSource, LPCTSTR sEventMessage, LogEventType type)
{
	bool bSuccess = false;

	HANDLE hEventSource = ::RegisterEventSource( nullptr, sEventSource );
	if ( hEventSource )
	{
		bSuccess = (::ReportEvent( hEventSource, static_cast<WORD>(type), 0, 256, nullptr, 1, 0, &sEventMessage, nullptr ) != FALSE);
		::DeregisterEventSource( hEventSource );
	}

	return bSuccess;
}