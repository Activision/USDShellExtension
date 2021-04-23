#pragma once

#include <map>
#include <deque>
#include <vector>
#include <set>

// This is a helper class that converts menus stored in our resource file 
// to a menu that can be displayed by the Windows Shell

class CShellContextMenu
{
public:
	bool Init( HMODULE hModule, UINT menuResourceId );
	void Term();

	// Last parameter is a deque of icons for every submenu as it is encountered by BuildMenu.
	// BuildMenu traversal is depth first.
	bool BuildMenu( HMENU mnuDest, UINT indexMenu, UINT &outIdCmdNext, const std::set<UINT> *pCommandsToExclude, std::deque<HBITMAP> &&subMenuIcons );

	bool GetHelpTextA( UINT offset, LPSTR pszName, UINT cchMax );
	bool GetHelpTextW( UINT offset, LPWSTR pszName, UINT cchMax );
	bool GetVerbTextA( UINT offset, LPSTR pszName, UINT cchMax );
	bool GetVerbTextW( UINT offset, LPWSTR pszName, UINT cchMax );

	bool ShellCommandOffsetToLocalId( UINT offset, UINT &outLocalCmdId );
	bool ShellCommandIdToLocalId( UINT idCmd, UINT &outLocalCmdId );
	bool ShellCommandVerbToLocalId( LPCWSTR pszVerb, UINT &outLocalCmdId );

	HBITMAP LoadIconFromModule( UINT idCmd );

private:
	bool CopyMenu( HMENU menuDest, HMENU menuSrc, UINT indexMenu, UINT &idCmdNext, const std::set<UINT> *pCommandsToExclude, std::deque<HBITMAP> &subMenuIcons );
	bool LoadVerbAndHelpString( UINT id, CString &outVerb, CString &outHelp );

	HMODULE m_hModule = nullptr;
	HMENU m_hMenu = nullptr;

	struct ShellData
	{
		UINT idLocal;
		CString verb;
		CString help;
	};

	std::map<UINT, ShellData> m_ShellCmdToShellData;
	std::map<CString, UINT> m_ShellVerbToLocalCmd;
	std::vector<UINT> m_ShellCmdList;
};