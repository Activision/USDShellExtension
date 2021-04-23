#include "stdafx.h"
#include "ShellContextMenu.h"


bool CShellContextMenu::Init( HMODULE hModule, UINT menuResourceId )
{
	m_hModule = hModule;

	m_hMenu = ::LoadMenu( m_hModule, MAKEINTRESOURCE( menuResourceId ) );
	if ( m_hMenu == nullptr )
		return false;

	m_hMenu = ::GetSubMenu( m_hMenu, 0 );

	return true;
}

void CShellContextMenu::Term()
{
}


bool CShellContextMenu::BuildMenu( HMENU mnuDest, UINT indexMenu, UINT &outIdCmdNext, const std::set<UINT> *pCommandsToExclude, std::deque<HBITMAP> &&subMenuIcons )
{
	m_ShellCmdToShellData.clear();
	m_ShellVerbToLocalCmd.clear();
	m_ShellCmdList.clear();

	if ( !CopyMenu( mnuDest, m_hMenu, indexMenu, outIdCmdNext, pCommandsToExclude, subMenuIcons ) )
		return false;

	return true;
}

bool CShellContextMenu::GetHelpTextA( UINT offset, LPSTR pszName, UINT cchMax )
{
	if ( offset >= m_ShellCmdList.size() )
		return false;

	UINT idCmd = m_ShellCmdList[offset];

	std::map<UINT, ShellData>::iterator itr;
	itr = m_ShellCmdToShellData.find( idCmd );
	if ( itr == m_ShellCmdToShellData.end() )
		return false;

	strcpy_s( pszName, cchMax, ATL::CT2A( itr->second.help ) );

	return true;
}

bool CShellContextMenu::GetHelpTextW( UINT offset, LPWSTR pszName, UINT cchMax )
{
	if ( offset >= m_ShellCmdList.size() )
		return false;

	UINT idCmd = m_ShellCmdList[offset];

	std::map<UINT, ShellData>::iterator itr;
	itr = m_ShellCmdToShellData.find( idCmd );
	if ( itr == m_ShellCmdToShellData.end() )
		return false;

	wcscpy_s( pszName, cchMax, ATL::CT2W( itr->second.help ) );

	return true;
}

bool CShellContextMenu::GetVerbTextA( UINT offset, LPSTR pszName, UINT cchMax )
{
	// Verbs are used by ShellExecute. 
	// ShellExecute( hWnd, "MyVerb", ... );

	if ( offset >= m_ShellCmdList.size() )
		return false;

	UINT idCmd = m_ShellCmdList[offset];

	std::map<UINT, ShellData>::iterator itr;
	itr = m_ShellCmdToShellData.find( idCmd );
	if ( itr == m_ShellCmdToShellData.end() )
		return false;

	strcpy_s( pszName, cchMax, ATL::CT2A( itr->second.verb ) );

	return true;
}

bool CShellContextMenu::GetVerbTextW( UINT offset, LPWSTR pszName, UINT cchMax )
{
	// Verbs are used by ShellExecute. 
	// ShellExecute( hWnd, "MyVerb", ... );

	if ( offset >= m_ShellCmdList.size() )
		return false;

	UINT idCmd = m_ShellCmdList[offset];

	std::map<UINT, ShellData>::iterator itr;
	itr = m_ShellCmdToShellData.find( idCmd );
	if ( itr == m_ShellCmdToShellData.end() )
		return false;

	wcscpy_s( pszName, cchMax, ATL::CT2W( itr->second.verb ) );

	return true;
}

bool CShellContextMenu::ShellCommandOffsetToLocalId( UINT offset, UINT &outLocalCmdId )
{
	if ( offset >= m_ShellCmdList.size() )
		return false;

	return ShellCommandIdToLocalId( m_ShellCmdList[offset], outLocalCmdId );
}

bool CShellContextMenu::ShellCommandIdToLocalId( UINT idCmd, UINT &outLocalCmdId )
{
	std::map<UINT, ShellData>::iterator itr;
	itr = m_ShellCmdToShellData.find( idCmd );
	if ( itr == m_ShellCmdToShellData.end() )
		return false;

	outLocalCmdId = itr->second.idLocal;

	return true;
}

bool CShellContextMenu::ShellCommandVerbToLocalId( LPCWSTR pszVerb, UINT &outLocalCmdId )
{
	std::map<CString, UINT>::iterator itr;
	itr = m_ShellVerbToLocalCmd.find( pszVerb );
	if ( itr == m_ShellVerbToLocalCmd.end() )
		return false;

	outLocalCmdId = itr->second;

	return true;
}


bool CShellContextMenu::CopyMenu( HMENU menuDest, HMENU menuSrc, UINT indexMenu, UINT &idCmdNext, const std::set<UINT> *pCommandsToExclude, std::deque<HBITMAP> &subMenuIcons )
{
	int srcMenuItemCount = ::GetMenuItemCount( menuSrc );
	for ( int srcMenuItemPosition = 0; srcMenuItemPosition < srcMenuItemCount; ++srcMenuItemPosition )
	{
		wchar_t menuItemText[256];

		MENUITEMINFO miiSrc = {};
		miiSrc.cbSize = sizeof( miiSrc );
		miiSrc.fMask = MIIM_TYPE | MIIM_ID | MIIM_SUBMENU;
		miiSrc.dwTypeData = menuItemText;
		miiSrc.cch = ARRAYSIZE( menuItemText );
		::GetMenuItemInfo( menuSrc, srcMenuItemPosition, TRUE, &miiSrc );

		const bool isSeparator = ((miiSrc.fType & MF_SEPARATOR) == MF_SEPARATOR);
		const bool isSubMenu = (miiSrc.hSubMenu != nullptr);
		const bool isCmd = !isSeparator && !isSubMenu;

		MENUITEMINFO miiDst = miiSrc;
		miiDst.fMask = MIIM_TYPE | MIIM_ID | MIIM_SUBMENU;

		if ( isCmd )
		{
			if ( pCommandsToExclude != nullptr )
			{
				if ( pCommandsToExclude->find( miiSrc.wID ) != pCommandsToExclude->end() )
					continue;
			}

			ShellData shellData = {};
			shellData.idLocal = miiSrc.wID;

			miiDst.wID = idCmdNext++;
			m_ShellCmdList.push_back( miiDst.wID );

			LoadVerbAndHelpString( miiSrc.wID, shellData.verb, shellData.help );

			HBITMAP hBmp = LoadIconFromModule( miiSrc.wID );
			if ( hBmp )
			{
				miiDst.hbmpUnchecked = hBmp;
				miiDst.fMask |= MIIM_CHECKMARKS;
			}

			m_ShellCmdToShellData.emplace( miiDst.wID, std::move( shellData ) );
		}

		if ( isSubMenu )
		{
			if ( !subMenuIcons.empty() )
			{
				miiDst.hbmpUnchecked = subMenuIcons.front();
				if ( miiDst.hbmpUnchecked != nullptr )
					miiDst.fMask |= MIIM_CHECKMARKS;

				subMenuIcons.pop_front();
			}

			miiDst.hSubMenu = ::CreatePopupMenu();
			if ( miiDst.hSubMenu )
			{
				CopyMenu( miiDst.hSubMenu, miiSrc.hSubMenu, 0, idCmdNext, pCommandsToExclude, subMenuIcons );
			}
		}

		::InsertMenuItem( menuDest, indexMenu + srcMenuItemPosition, TRUE, &miiDst );
	}

	return true;
}


bool CShellContextMenu::LoadVerbAndHelpString( UINT id, CString &outVerb, CString &outHelp )
{
	// String should be delimited by a new line. If there is no new line then assume it's all just a help string.
	// "[verb]\n[help]"
	// "[help]"

	TCHAR resourceString[512];
	int nLengthOfStringWithoutNullTerminator = ::LoadString( m_hModule, id, resourceString, ARRAYSIZE( resourceString ) );
	if ( nLengthOfStringWithoutNullTerminator == 0 )
		return false;

	const TCHAR delimiters[] = _T( "\n" );
	TCHAR *pContext = nullptr;
	TCHAR *pVerb = _tcstok_s( resourceString, delimiters, &pContext );
	if ( pVerb != nullptr )
	{
		outVerb = pVerb;
		TCHAR *pHelp = _tcstok_s( nullptr, delimiters, &pContext );
		if ( pHelp != nullptr )
		{
			outHelp = pHelp;
		}
	}
	else
	{
		outHelp = resourceString;
	}

	return true;
}

HBITMAP CShellContextMenu::LoadIconFromModule( UINT idCmd )
{
	// load the small icon
	HICON hIcon = reinterpret_cast<HICON>(::LoadImage( m_hModule, MAKEINTRESOURCE( idCmd ), IMAGE_ICON, ::GetSystemMetrics( SM_CXSMICON ), ::GetSystemMetrics( SM_CYSMICON ), 0 ));
	if ( hIcon == nullptr )
		return nullptr;

	ICONINFO ii = {};
	GetIconInfo( hIcon, &ii );
	HBITMAP hBitmap = ii.hbmColor;

	return hBitmap;
}