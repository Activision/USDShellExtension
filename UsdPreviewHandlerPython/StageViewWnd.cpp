#include "stdafx.h"
#include "StageViewWnd.h"
#include "Module.h"
#include "resource.h"


void CStageViewWnd::Init( HWND hWndToSubclass )
{
	if ( m_hWnd )
		Term();

	SubclassWindow( hWndToSubclass );
}

void CStageViewWnd::Term()
{
	UnsubclassWindow();
}

