#pragma once

#include "resource.h"
#include "UWPProgressBar.h"

class CUsdLoadScreenDlg : public CDialogImpl<CUsdLoadScreenDlg>
{
public:
	enum { IDD = IDD_LOADSCREEN };

	void SetBackgroundColor( COLORREF color );

private:
	void OnFinalMessage( _In_ HWND hWnd ) override;

	void CreateBackBuffer( UINT width, UINT height );
	void Draw( Gdiplus::Graphics &gfx );

	void InvalidateProgressBar( Gdiplus::RectF &rcArea );

	BEGIN_MSG_MAP( CUsdLoadScreenDlg )
		MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
		MESSAGE_HANDLER( WM_PAINT, OnPaint )
		MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBkgnd )
		MESSAGE_HANDLER( WM_SIZE, OnSize )
		MESSAGE_HANDLER( WM_CTLCOLORDLG, OnCtlColorDlg )
		MESSAGE_HANDLER( WM_TIMER, OnTimer )
	END_MSG_MAP()

	LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled );
	LRESULT OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled );
	LRESULT OnEraseBkgnd( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled );
	LRESULT OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled );
	LRESULT OnCtlColorDlg( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled );
	LRESULT OnTimer( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled );

	CUWPProgressBar m_ProgressBar;

	CImage m_imgBackBuffer;
	HBRUSH m_hBackground = nullptr;
	ULONG_PTR m_gdiplusToken = 0;
	Gdiplus::Image *m_pImgLoadScreen = nullptr;
	Gdiplus::Image *m_pImgLogo = nullptr;
	Gdiplus::Color m_Background;

	HDC m_hDCBackbuffer = nullptr;
	Gdiplus::Graphics *m_pGfx = nullptr;
	Gdiplus::RectF m_rcProgressArea;

	bool m_bWindowsExplorerUsingLightTheme = true;
};