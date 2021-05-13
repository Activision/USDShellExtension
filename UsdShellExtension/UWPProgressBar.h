#pragma once

// This class attempts to replicate the UWP XAML progress bar

class CUWPProgressBar
{
public:
	void Init();

	void UpdateProgressBar();
	void DrawProgressBar( HWND hWnd, Gdiplus::Graphics &gfx, Gdiplus::RectF &rcDst, Gdiplus::Color &backgroundColor );
	void SetBackgroundColor( COLORREF color, bool bWindowsExplorerUsingLightTheme );

private:
	static constexpr size_t knProgressBarDotCount = 5;
	static constexpr float kfProgressBarDotSize = 4.0f;
	Gdiplus::PointF m_ptProgressBarDot[knProgressBarDotCount];
	float m_LastUpdateTime = 0;
	bool m_bWindowsExplorerUsingLightTheme = true;
};
