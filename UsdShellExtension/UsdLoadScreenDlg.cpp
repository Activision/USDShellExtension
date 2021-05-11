#include "stdafx.h"
#include "UsdLoadScreenDlg.h"
#include "Module.h"
#include "resource.h"

#include <algorithm>

static constexpr UINT s_TimerIdProgress = 100;

static Gdiplus::Image *LoadImageFromResourcePNG( UINT nId )
{
	HRSRC hrscPng = ::FindResource( g_hInstance, MAKEINTRESOURCE( nId ), _T( "PNG" ) );
	if ( hrscPng != nullptr )
	{
		HGLOBAL hPng = ::LoadResource( g_hInstance, hrscPng );
		if ( hPng != nullptr )
		{
			void *pImageData = ::LockResource( hPng );
			if ( pImageData != nullptr )
			{
				DWORD nSize = ::SizeofResource( g_hInstance, hrscPng );

				CComPtr<IStream> pImageStream;
				pImageStream.p = ::SHCreateMemStream( reinterpret_cast<BYTE *>(pImageData), nSize );

				Gdiplus::Image *img = Gdiplus::Image::FromStream( pImageStream );
				return img;
			}
		}
	}

	return nullptr;
}

LRESULT CUsdLoadScreenDlg::OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
{
	UNREFERENCED_PARAMETER( uMsg );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );
	UNREFERENCED_PARAMETER( bHandled );

	SetWindowText( _T( "UsdLoadScreenDlg" ) );

	Gdiplus::GdiplusStartupInputEx gdiplusStartupInput;
	Gdiplus::GdiplusStartup( &m_gdiplusToken, &gdiplusStartupInput, nullptr );

	m_pImgLoadScreen = LoadImageFromResourcePNG( IDB_LOADSCREEN );
	m_pImgLogo = LoadImageFromResourcePNG( IDB_LOGO );

	RECT rcClient;
	GetClientRect( &rcClient );

	m_ProgressBar.Init();

	CreateBackBuffer( rcClient.right - rcClient.left, rcClient.bottom - rcClient.top );
	if ( m_pGfx )
		Draw( *m_pGfx );

	SetTimer( s_TimerIdProgress, 1, nullptr );

	return 0;
}

LRESULT CUsdLoadScreenDlg::OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
{
	UNREFERENCED_PARAMETER( uMsg );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );
	UNREFERENCED_PARAMETER( bHandled );

	PAINTSTRUCT ps;
	HDC hDCFrontBuffer = BeginPaint( &ps );

	if ( !m_imgBackBuffer.IsNull() )
	{
		POINT ptSrc = { ps.rcPaint.left, ps.rcPaint.top };
		m_imgBackBuffer.BitBlt( hDCFrontBuffer, ps.rcPaint, ptSrc, SRCCOPY );
	}

	EndPaint( &ps );

	return 0;
}

LRESULT CUsdLoadScreenDlg::OnEraseBkgnd( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
{
	UNREFERENCED_PARAMETER( uMsg );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );
	UNREFERENCED_PARAMETER( bHandled );

	return (LRESULT)::GetStockObject( NULL_BRUSH );
}

LRESULT CUsdLoadScreenDlg::OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
{
	UNREFERENCED_PARAMETER( uMsg );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( bHandled );

	UINT width = LOWORD( lParam );
	UINT height = HIWORD( lParam );

	CreateBackBuffer( width, height );
	if ( m_pGfx )
		Draw( *m_pGfx );
	Invalidate( FALSE );

	return 0;
}

LRESULT CUsdLoadScreenDlg::OnCtlColorDlg( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
{
	UNREFERENCED_PARAMETER( uMsg );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );
	UNREFERENCED_PARAMETER( bHandled );

	return (LRESULT)::GetStockObject( NULL_BRUSH );
}

LRESULT CUsdLoadScreenDlg::OnTimer( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
{
	UNREFERENCED_PARAMETER( uMsg );
	UNREFERENCED_PARAMETER( lParam );
	UNREFERENCED_PARAMETER( bHandled );

	switch ( wParam )
	{
	case s_TimerIdProgress:
		if ( m_pGfx && m_hWnd )
		{
			m_ProgressBar.UpdateProgressBar();
			m_ProgressBar.DrawProgressBar( *m_pGfx, m_rcProgressArea, m_Background );
			InvalidateProgressBar( m_rcProgressArea );
		}
		break;
	}

	return 0;
}

void CUsdLoadScreenDlg::SetBackgroundColor( COLORREF color )
{
	if ( m_hBackground )
	{
		::DeleteObject( m_hBackground );
		m_hBackground = nullptr;
	}

	m_bWindowsExplorerUsingLightTheme = true;

	CRegKey regDarkMode;
	LRESULT lr = regDarkMode.Open( HKEY_CURRENT_USER, _T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize" ), KEY_READ );
	if ( lr == ERROR_SUCCESS )
	{
		DWORD nSystemUsesLightTheme = 0;
		lr = regDarkMode.QueryDWORDValue( _T( "SystemUsesLightTheme" ), nSystemUsesLightTheme );
		if ( lr == ERROR_SUCCESS )
		{
			m_bWindowsExplorerUsingLightTheme = (nSystemUsesLightTheme != 0);
		}
	}

	// Colors hard-coded to Windows 10 2003 Explorer
	if ( m_bWindowsExplorerUsingLightTheme == false )
		color = RGB( 32, 32, 32 );

	m_Background.SetFromCOLORREF( color );

	m_hBackground = ::CreateSolidBrush( color );

	m_ProgressBar.SetBackgroundColor( color, m_bWindowsExplorerUsingLightTheme );

	if ( m_pGfx )
	{
		Draw( *m_pGfx );
		Invalidate( FALSE );
	}
}

void CUsdLoadScreenDlg::OnFinalMessage( _In_ HWND hWnd )
{
	UNREFERENCED_PARAMETER( hWnd );

	if ( m_hBackground )
	{
		::DeleteObject( m_hBackground );
		m_hBackground = nullptr;
	}

	delete m_pImgLoadScreen;
	m_pImgLoadScreen = nullptr;

	if ( m_pGfx )
	{
		delete m_pGfx;
		m_pGfx = nullptr;
	}
	if ( m_hDCBackbuffer )
	{
		m_imgBackBuffer.ReleaseDC();
		m_hDCBackbuffer = nullptr;
	}
	m_imgBackBuffer.Destroy();

	Gdiplus::GdiplusShutdown( m_gdiplusToken );
}

void CUsdLoadScreenDlg::CreateBackBuffer( UINT width, UINT height )
{
	if ( m_pGfx )
	{
		delete m_pGfx;
		m_pGfx = nullptr;
	}
	if ( m_hDCBackbuffer )
	{
		m_imgBackBuffer.ReleaseDC();
		m_hDCBackbuffer = nullptr;
	}
	m_imgBackBuffer.Destroy();

	if ( width == 0 || height == 0 )
		return;

	m_imgBackBuffer.Create( width, height, 32, 0 );
	m_hDCBackbuffer = m_imgBackBuffer.GetDC();
	m_pGfx = Gdiplus::Graphics::FromHDC( m_hDCBackbuffer );

	m_pGfx->SetCompositingMode( Gdiplus::CompositingModeSourceOver );
	m_pGfx->SetCompositingQuality( Gdiplus::CompositingQualityGammaCorrected );
	m_pGfx->SetInterpolationMode( Gdiplus::InterpolationModeHighQualityBicubic );
	m_pGfx->SetSmoothingMode( Gdiplus::SmoothingModeAntiAlias );
}

void CUsdLoadScreenDlg::Draw( Gdiplus::Graphics &gfx )
{
	if ( m_imgBackBuffer.IsNull() )
		return;

	Gdiplus::SolidBrush brush( m_Background );
	gfx.FillRectangle( &brush, 0, 0, static_cast<INT>(m_imgBackBuffer.GetWidth()), static_cast<INT>(m_imgBackBuffer.GetHeight()) );

	if ( m_pImgLoadScreen != nullptr )
	{
		Gdiplus::SizeF margins;
		margins.Width = 50.0f;
		margins.Height = 50.0f;

		Gdiplus::SizeF sizeWindow;
		sizeWindow.Width = std::max<Gdiplus::REAL>( 0.0f, m_imgBackBuffer.GetWidth() - (margins.Width * 2) );
		sizeWindow.Height = std::max<Gdiplus::REAL>( 0.0f, m_imgBackBuffer.GetHeight() - (margins.Height * 2) );

		// initialize the size of the image we draw to the ideal size
		Gdiplus::SizeF sizeImageOriginal;
		sizeImageOriginal.Width = static_cast<Gdiplus::REAL>(m_pImgLoadScreen->GetWidth());
		sizeImageOriginal.Height = static_cast<Gdiplus::REAL>(m_pImgLoadScreen->GetHeight());

		Gdiplus::SizeF sizeImageResized = sizeImageOriginal;

		if ( sizeImageOriginal.Width > sizeWindow.Width )
		{
			float fAspectRatio = sizeImageOriginal.Height / sizeImageOriginal.Width;
			Gdiplus::SizeF sizeNew;
			sizeNew.Width = sizeWindow.Width;
			sizeNew.Height = sizeWindow.Width * fAspectRatio;

			if ( sizeNew.Width < sizeImageResized.Width || sizeNew.Height < sizeImageResized.Height )
				sizeImageResized = sizeNew;
		}

		if ( sizeImageOriginal.Height > sizeWindow.Height )
		{
			float fAspectRatio = sizeImageOriginal.Width / sizeImageOriginal.Height;
			Gdiplus::SizeF sizeNew;
			sizeNew.Height = sizeWindow.Height;
			sizeNew.Width = sizeWindow.Height * fAspectRatio;

			if ( sizeNew.Width < sizeImageResized.Width || sizeNew.Height < sizeImageResized.Height )
				sizeImageResized = sizeNew;
		}


		Gdiplus::RectF rcSrc(
			0, 0,
			sizeImageOriginal.Width,
			sizeImageOriginal.Height );

		Gdiplus::RectF rcDst(
			((sizeWindow.Width - sizeImageResized.Width) / 2.0f) + margins.Width,
			((sizeWindow.Height - sizeImageResized.Height) / 2.0f) + margins.Height,
			sizeImageResized.Width,
			sizeImageResized.Height );

		gfx.DrawImage( m_pImgLoadScreen, rcDst, rcSrc, Gdiplus::UnitPixel );

		m_rcProgressArea = Gdiplus::RectF(
			2.0f,
			rcDst.GetBottom(),
			static_cast<Gdiplus::REAL>(m_imgBackBuffer.GetWidth() - 4),
			margins.Height
		);
		m_ProgressBar.DrawProgressBar( gfx, m_rcProgressArea, m_Background );
	}

	if ( m_pImgLogo != nullptr )
	{
		float fDpiWindow = static_cast<float>(::GetDpiForWindow( m_hWnd ));
		float fDpiScale = fDpiWindow / USER_DEFAULT_SCREEN_DPI;

		Gdiplus::SizeF margins;
		margins.Width = 10.0f;
		margins.Height = 10.0f;

		Gdiplus::SizeF sizeWindow;
		sizeWindow.Width = std::max<Gdiplus::REAL>( 0.0f, m_imgBackBuffer.GetWidth() - (margins.Width * 2) );
		sizeWindow.Height = std::max<Gdiplus::REAL>( 0.0f, m_imgBackBuffer.GetHeight() - (margins.Height * 2) );

		// initialize the size of the image we draw to the ideal size
		Gdiplus::SizeF sizeImageOriginal;
		sizeImageOriginal.Width = static_cast<Gdiplus::REAL>(m_pImgLogo->GetWidth());
		sizeImageOriginal.Height = static_cast<Gdiplus::REAL>(m_pImgLogo->GetHeight());

		Gdiplus::SizeF sizeImageResized = sizeImageOriginal;
		sizeImageResized.Width /= 6.0f;
		sizeImageResized.Height /= 6.0f;

		// DPI adjust
		sizeImageResized.Width *= fDpiScale;
		sizeImageResized.Height *= fDpiScale;

		Gdiplus::RectF rcSrc(
			0, 0,
			sizeImageOriginal.Width,
			sizeImageOriginal.Height );

		Gdiplus::RectF rcDst(
			sizeWindow.Width - sizeImageResized.Width,
			sizeWindow.Height - sizeImageResized.Height,
			sizeImageResized.Width,
			sizeImageResized.Height );

		// do not draw if there is no room
		if ( m_rcProgressArea.GetBottom() < rcDst.GetTop() )
		{
			// invert colors for the light theme
			const float alpha = 0.25f;
			Gdiplus::ColorMatrix matrixLightTheme =
			{
				-1, 0, 0, 0, 0,
				0, -1, 0, 0, 0,
				0, 0, -1, 0, 0,
				0, 0, 0, alpha, 0,
				0, 0, 0, 0, 1
			};
			Gdiplus::ColorMatrix matrixDarkTheme =
			{
				1, 0, 0, 0, 0,
				0, 1, 0, 0, 0,
				0, 0, 1, 0, 0,
				0, 0, 0, alpha, 0,
				0, 0, 0, 0, 1
			};

			Gdiplus::ImageAttributes attrib;
			attrib.SetColorMatrix( m_bWindowsExplorerUsingLightTheme ? &matrixLightTheme : &matrixDarkTheme );

			gfx.DrawImage( m_pImgLogo, rcDst, rcSrc, Gdiplus::UnitPixel, &attrib );
		}
	}
}

void CUsdLoadScreenDlg::InvalidateProgressBar( Gdiplus::RectF &rcArea )
{
	RECT rc;
	rc.left = static_cast<LONG>(rcArea.GetLeft());
	rc.top = static_cast<LONG>(rcArea.GetTop());
	rc.right = static_cast<LONG>(rcArea.GetRight());
	rc.bottom = static_cast<LONG>(rcArea.GetBottom());
	InvalidateRect( &rc, FALSE );
}