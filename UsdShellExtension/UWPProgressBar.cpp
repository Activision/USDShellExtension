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
#include "UWPProgressBar.h"

static float GetPerformanceFrequency()
{
	LARGE_INTEGER li;
	QueryPerformanceFrequency( &li );
	return (float)li.QuadPart;
}

static float GetTimeInterval()
{
	static float s_fFreq = GetPerformanceFrequency();
	return s_fFreq;
}

static float GetTimePrecise()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter( &li );

	return (float)li.QuadPart / (GetTimeInterval() / 1000.0f);
}

void CUWPProgressBar::Init()
{
	m_LastUpdateTime = GetTimePrecise();

	m_ptProgressBarDot[0].Y = -10.0f;
	m_ptProgressBarDot[0].X = 0;
	for ( int i = 1; i < ARRAYSIZE( m_ptProgressBarDot ); ++i )
	{
		m_ptProgressBarDot[i].Y = m_ptProgressBarDot[i - 1].Y - 0.5f;
		m_ptProgressBarDot[i].X = 0.0f;
	}
}

void CUWPProgressBar::UpdateProgressBar()
{
	float tCurrent = GetTimePrecise();
	float fDelta = (float)(tCurrent - m_LastUpdateTime);
	m_LastUpdateTime = tCurrent;

	fDelta /= 100.0f;

	for ( Gdiplus::PointF &pt : m_ptProgressBarDot )
	{
		pt.Y += fDelta;

		// x = (y^3 / 32) + y
		// the resulting ranges are:
		// x: [-30,+30]
		// y: [-10,+10]
		pt.X = ((pt.Y * pt.Y * pt.Y) / 32.0f) + pt.Y;

		// if we are past our range, go back to -10.0f
		if ( pt.Y > 10.0f )
			pt.Y -= 20.0f;
	}
}

void CUWPProgressBar::DrawProgressBar( HWND hWnd, Gdiplus::Graphics &gfx, Gdiplus::RectF &rcArea, Gdiplus::Color &backgroundColor )
{
	float fDpiWindow = static_cast<float>(::GetDpiForWindow( hWnd ));
	float fDpiScale = fDpiWindow / USER_DEFAULT_SCREEN_DPI;

	Gdiplus::SolidBrush brush( backgroundColor );
	gfx.FillRectangle( &brush, rcArea );

	Gdiplus::SizeF dot;
	dot.Width = kfProgressBarDotSize;
	dot.Height = kfProgressBarDotSize;

	// DPI adjust
	dot.Width *= fDpiScale;
	dot.Height *= fDpiScale;

	for ( Gdiplus::PointF &pt : m_ptProgressBarDot )
	{
		// dots do not look good in a width below 720 or so
		float fScaledWidth = std::max<float>( 720.0f, (float)rcArea.Width );

		// pt.X [-30.0f, 30.0f]
		float scaledX = (((pt.X + 30.0f) * fScaledWidth) / 60.0f);
		scaledX += ((float)rcArea.Width - fScaledWidth) / 2.0f;
		if ( scaledX < 0 || (scaledX + dot.Width) >= rcArea.Width )
			continue;

		Gdiplus::RectF rcDot(
			rcArea.X + scaledX,
			rcArea.Y + ((rcArea.Height - dot.Height) / 2.0f),
			dot.Width,
			dot.Height );

		Gdiplus::Color clrDot;
		BYTE bAlpha = (BYTE)(((100.0f - fabsf( pt.X )) / 100.0f) * 255.0f);
		//clrDot.SetValue( Gdiplus::Color::MakeARGB(bAlpha, 52, 198, 243) );
		clrDot.SetValue( Gdiplus::Color::MakeARGB( bAlpha, 200, 200, 200 ) );
		Gdiplus::SolidBrush brushDot( clrDot );

		gfx.FillEllipse( &brushDot, rcDot );
	}
}

void CUWPProgressBar::SetBackgroundColor( COLORREF color, bool bWindowsExplorerUsingLightTheme )
{
	UNREFERENCED_PARAMETER( color );

	m_bWindowsExplorerUsingLightTheme = bWindowsExplorerUsingLightTheme;
}