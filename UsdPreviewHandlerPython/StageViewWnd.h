#pragma once

#include "resource.h"

class CStageViewWnd : public CWindowImpl<CStageViewWnd>
{
public:
	void Init( HWND hWndToSubclass );
	void Term();

private:

	BEGIN_MSG_MAP(CStageViewWnd)
	END_MSG_MAP()
};