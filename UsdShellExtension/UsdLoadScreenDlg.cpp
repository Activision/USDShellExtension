#include "stdafx.h"
#include "UsdLoadScreenDlg.h"
#include "Module.h"
#include "resource.h"

#include <algorithm>

static constexpr UINT s_TimerIdProgress = 100;

LRESULT CUsdLoadScreenDlg::OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
{
	UNREFERENCED_PARAMETER( uMsg );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );
	UNREFERENCED_PARAMETER( bHandled );

	SetWindowText( _T( "UsdLoadScreenDlg" ) );

	Gdiplus::GdiplusStartupInputEx gdiplusStartupInput;
	Gdiplus::GdiplusStartup( &m_gdiplusToken, &gdiplusStartupInput, nullptr );

	HRSRC hrscPng = ::FindResource( g_hInstance, MAKEINTRESOURCE( IDB_LOADSCREEN ), _T("PNG") );
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
				pImageStream.p = ::SHCreateMemStream( reinterpret_cast<BYTE*>(pImageData), nSize );

				m_pImgLoadScreen = Gdiplus::Image::FromStream( pImageStream );
			}
		}
	}

	RECT rcClient;
	GetClientRect( &rcClient );

	m_ProgressBar.Init();

	CreateBackBuffer( rcClient.right - rcClient.left, rcClient.bottom - rcClient.top );
	if ( m_pGfx )
		Draw( *m_pGfx );

	// 60 fps
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

	EndPaint(&ps);

	return 0;
}

LRESULT CUsdLoadScreenDlg::OnEraseBkgnd( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
{
	UNREFERENCED_PARAMETER( uMsg );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );
	UNREFERENCED_PARAMETER( bHandled );

	return (LRESULT)::GetStockObject(NULL_BRUSH);
}

LRESULT CUsdLoadScreenDlg::OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
{
	UNREFERENCED_PARAMETER( uMsg );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( bHandled );

	UINT width = LOWORD( lParam );
	UINT height = HIWORD( lParam );

	CreateBackBuffer( width, height );
	if (m_pGfx)
		Draw(*m_pGfx);
	Invalidate( FALSE );

	return 0;
}

LRESULT CUsdLoadScreenDlg::OnCtlColorDlg( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
{
	UNREFERENCED_PARAMETER( uMsg );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );
	UNREFERENCED_PARAMETER( bHandled );

	return (LRESULT)::GetStockObject(NULL_BRUSH);
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

	bool bWindowsExplorerUsingLightTheme = true;

	CRegKey regDarkMode;
	LRESULT lr = regDarkMode.Open( HKEY_CURRENT_USER, _T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize" ), KEY_READ );
	if ( lr == ERROR_SUCCESS )
	{
		DWORD nSystemUsesLightTheme = 0;
		lr = regDarkMode.QueryDWORDValue( _T( "SystemUsesLightTheme" ), nSystemUsesLightTheme );
		if ( lr == ERROR_SUCCESS )
		{
			bWindowsExplorerUsingLightTheme = (nSystemUsesLightTheme != 0);
		}
	}

	// Colors hard-coded to Windows 10 2003 Explorer
	if (bWindowsExplorerUsingLightTheme == false)
		color = RGB( 32, 32, 32 );

	m_Background.SetFromCOLORREF( color );

	m_hBackground = ::CreateSolidBrush( color );

	if ( m_pGfx )
	{
		Draw( *m_pGfx );
		Invalidate( FALSE );
	}
}

void CUsdLoadScreenDlg::OnFinalMessage(_In_ HWND hWnd)
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

	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

void CUsdLoadScreenDlg::CreateBackBuffer(UINT width, UINT height)
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

void CUsdLoadScreenDlg::Draw(Gdiplus::Graphics& gfx)
{
	if ( m_imgBackBuffer.IsNull() )
		return;

	Gdiplus::SolidBrush brush( m_Background );
	gfx.FillRectangle( &brush, 0, 0, static_cast<INT>(m_imgBackBuffer.GetWidth()), static_cast<INT>(m_imgBackBuffer.GetHeight()) );

	if ( m_pImgLoadScreen != nullptr )
	{
		SIZE margins;
		margins.cx = 50;
		margins.cy = 50;

		SIZE sizeWindow;
		sizeWindow.cx = std::max<LONG>( 0, m_imgBackBuffer.GetWidth() - (margins.cx * 2) );
		sizeWindow.cy = std::max<LONG>( 0, m_imgBackBuffer.GetHeight() - (margins.cy * 2) );

		// initialize the size of the image we draw to the ideal size
		SIZE sizeImage;
		sizeImage.cx = m_pImgLoadScreen->GetWidth();
		sizeImage.cy = m_pImgLoadScreen->GetHeight();

		if ( static_cast<INT>(m_pImgLoadScreen->GetWidth()) > sizeWindow.cx )
		{
			float fAspectRatio = (float)m_pImgLoadScreen->GetHeight() / (float)m_pImgLoadScreen->GetWidth();
			SIZE sizeNew;
			sizeNew.cx = sizeWindow.cx;
			sizeNew.cy = (LONG)((float)sizeWindow.cx * fAspectRatio);

			if ( sizeNew.cx < sizeImage.cx || sizeNew.cy < sizeImage.cy )
				sizeImage = sizeNew;
		}

		if ( static_cast<INT>(m_pImgLoadScreen->GetHeight()) > sizeWindow.cy )
		{
			float fAspectRatio = (float)m_pImgLoadScreen->GetWidth() / (float)m_pImgLoadScreen->GetHeight();
			SIZE sizeNew;
			sizeNew.cy = sizeWindow.cy;
			sizeNew.cx = (LONG)((float)sizeWindow.cy * fAspectRatio);

			if ( sizeNew.cx < sizeImage.cx || sizeNew.cy < sizeImage.cy )
				sizeImage = sizeNew;
		}


		Gdiplus::RectF rcSrc(
			0, 0,
			static_cast<Gdiplus::REAL>(m_pImgLoadScreen->GetWidth()),
			static_cast<Gdiplus::REAL>(m_pImgLoadScreen->GetHeight()) );

		Gdiplus::RectF rcDst(
			static_cast<Gdiplus::REAL>(((sizeWindow.cx - sizeImage.cx) / 2) + margins.cx),
			static_cast<Gdiplus::REAL>(((sizeWindow.cy - sizeImage.cy) / 2) + margins.cy),
			static_cast<Gdiplus::REAL>(sizeImage.cx),
			static_cast<Gdiplus::REAL>(sizeImage.cy) );

		gfx.DrawImage( m_pImgLoadScreen, rcDst, rcSrc, Gdiplus::UnitPixel );

		m_rcProgressArea = Gdiplus::RectF(
			2,
			rcDst.GetBottom(),
			static_cast<Gdiplus::REAL>(m_imgBackBuffer.GetWidth() - 4),
			static_cast<Gdiplus::REAL>(margins.cy)
		);
		m_ProgressBar.DrawProgressBar( gfx, m_rcProgressArea, m_Background );
	}
}

void CUsdLoadScreenDlg::InvalidateProgressBar( Gdiplus::RectF& rcArea )
{
	RECT rc;
	rc.left = static_cast<LONG>(rcArea.GetLeft());
	rc.top = static_cast<LONG>(rcArea.GetTop());
	rc.right = static_cast<LONG>(rcArea.GetRight());
	rc.bottom = static_cast<LONG>(rcArea.GetBottom());
	InvalidateRect( &rc, FALSE );
}