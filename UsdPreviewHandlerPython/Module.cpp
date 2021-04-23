#include "stdafx.h"
#include "Module.h"

HMODULE g_hInstance;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
	UNREFERENCED_PARAMETER( lpReserved );

	switch ( ul_reason_for_call )
	{
	case DLL_PROCESS_ATTACH:
		g_hInstance = hModule;
		break;
	}

	return TRUE;
}