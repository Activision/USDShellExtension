;--------------------------------
;Includes

!include "MUI2.nsh"
!include "Library.nsh"
!include "logiclib.nsh"

!define LIBRARY_X64

!define /ifndef OUT_FILE "UsdShellExtension-setup.exe"
!define NAME_LONG "Activision USD Shell Extension"

!define /ifndef VER_MAJOR 0
!define /ifndef VER_MINOR 0
!define /ifndef VER_REVISION 0
!define /ifndef VER_BUILD 0

!define /ifndef VERSION "${VER_MAJOR}.${VER_MINOR}"
!define /ifndef USD_VERSION "Unknown Version"
!define /ifndef PYTHON_VERSION "Unknown Version"

; The name of the installer
Name "${NAME_LONG}"

; The file to write
OutFile "${OUT_FILE}"

; Request application privileges for Windows Vista and higher
RequestExecutionLevel admin

; Build Unicode installer
Unicode True

;SetCompress off
SetCompressor LZMA

; The default installation directory
InstallDir $PROGRAMFILES\Activision\UsdShellExtension

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Activision\UsdShellExtension" "Install_Dir"

!define MUI_BGCOLOR "FFFFFF"
!define MUI_ICON "..\..\..\..\shared\usd.ico"
!define MUI_UNICON "..\..\..\..\shared\usd.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "..\..\..\..\shared\installerWelcome.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP_STRETCH "NoStretchNoCropNoAlign"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "..\..\..\..\shared\installerWelcome.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP_STRETCH "NoStretchNoCropNoAlign"

!define MUI_WELCOMEPAGE_TITLE "${NAME_LONG}$\r$\nVersion ${VERSION}"
!define MUI_WELCOMEPAGE_TEXT "Setup will guide you through the installation of $(^NameDA).$\r$\n$\r$\nUSD Version: ${USD_VERSION}$\r$\nPython Version: ${PYTHON_VERSION}$\r$\n$\r$\nIt is recommended that you close all other applications before starting Setup. This will make it possible to update relevant system files without having to reboot your computer.$\r$\n$\r$\n$_CLICK"

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING

;--------------------------------
;Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Version information

!ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD
VIProductVersion ${VER_MAJOR}.${VER_MINOR}.${VER_REVISION}.${VER_BUILD}
VIAddVersionKey "FileVersion" "${VERSION}"
VIAddVersionKey "FileDescription" "${NAME_LONG} Setup"
VIAddVersionKey "LegalCopyright" ""
!endif

SetPluginUnload  alwaysoff

!define MAX_PATH 260
!define CCH_RM_SESSION_KEY 33
!define RmForceShutdown 0x1
!define RmShutdownOnlyRegistered 0x10

;--------------------------------
Var RmExplorerSession
Var RmApplicationSession
Var RmWindowsSearchSession

;--------------------------------
!macro ShutdownExplorer UN
Function ${UN}ShutdownExplorer

SetDetailsPrint textonly
DetailPrint "Shutting down Windows Explorer..."
SetDetailsPrint listonly

System::StrAlloc ${CCH_RM_SESSION_KEY}
Pop $0
System::Call 'Rstrtmgr::RmStartSession(*i .R1, i 0, p $0) i.R0'
System::Free $0

StrCpy $RmExplorerSession $R1

DetailPrint "Shutting down Windows Explorer"

StrCpy $0 "$WINDIR\explorer.exe"
System::Call '*(w r0)p.R1 ?2'
System::Call 'Rstrtmgr::RmRegisterResources(i $RmExplorerSession, i 1, p R1, i 0, p n, i 0, p n) i.R0'

System::Call 'Rstrtmgr::RmShutdown(i $RmExplorerSession, i 0, p n) i.R0'

FunctionEnd
!macroend
!insertmacro ShutdownExplorer "" 
!insertmacro ShutdownExplorer "un."


;--------------------------------
!macro RestartExplorer UN 
Function ${UN}RestartExplorer

SetDetailsPrint textonly
DetailPrint "Restarting Windows Explorer..."
SetDetailsPrint listonly

DetailPrint "Restarting Windows Explorer"

System::Call 'Rstrtmgr::RmRestart(i $RmExplorerSession, i 0, p n) i.R0'

System::Call 'Rstrtmgr::RmEndSession(i $RmExplorerSession) i.R0'

FunctionEnd
!macroend
!insertmacro RestartExplorer "" 
!insertmacro RestartExplorer "un."


;--------------------------------
!macro ShutdownWindowsSearch UN 
Function ${UN}ShutdownWindowsSearch 

SetDetailsPrint textonly
DetailPrint "Shutting down Windows Search..."
SetDetailsPrint listonly

System::StrAlloc ${CCH_RM_SESSION_KEY}
Pop $0
System::Call 'Rstrtmgr::RmStartSession(*i .R1, i 0, p $0) i.R0'
System::Free $0

StrCpy $RmWindowsSearchSession $R1

DetailPrint "Shutting down Windows Search"

StrCpy $0 "wsearch"
System::Call '*(w r0)p.R1 ?2'
System::Call 'Rstrtmgr::RmRegisterResources(i $RmWindowsSearchSession, i 0, p n, i 0, p n, i 1, p R1) i.R0'

System::Call 'Rstrtmgr::RmShutdown(i $RmWindowsSearchSession, i 0, p n) i.R0'

FunctionEnd
!macroend
!insertmacro ShutdownWindowsSearch "" 
!insertmacro ShutdownWindowsSearch "un."

;--------------------------------
!macro RestartWindowsSearch UN 
Function ${UN}RestartWindowsSearch 

SetDetailsPrint textonly
DetailPrint "Restarting Windows Search..."
SetDetailsPrint listonly

DetailPrint "Restarting Windows Search"

System::Call 'Rstrtmgr::RmRestart(i $RmWindowsSearchSession, i 0, p n) i.R0'

System::Call 'Rstrtmgr::RmEndSession(i $RmWindowsSearchSession) i.R0'

FunctionEnd
!macroend
!insertmacro RestartWindowsSearch "" 
!insertmacro RestartWindowsSearch "un."


;--------------------------------
!macro ShutdownApplications UN 
Function ${UN}ShutdownApplications 

SetDetailsPrint textonly
DetailPrint "Shutting down applications..."
SetDetailsPrint listonly

System::StrAlloc ${CCH_RM_SESSION_KEY}
Pop $0
System::Call 'Rstrtmgr::RmStartSession(*i .R1, i 0, p $0) i.R0'
System::Free $0

StrCpy $RmApplicationSession $R1

StrCpy $0 "$INSTDIR\UsdShellExtension.dll"
StrCpy $1 "$INSTDIR\tbb.dll"
StrCpy $2 "$INSTDIR\tbbmalloc.dll"
StrCpy $3 "$INSTDIR\${BOOSTDLL}"
StrCpy $4 "$INSTDIR\${PYTHONDLL}"
StrCpy $5 "$INSTDIR\usd_ms.dll"
StrCpy $6 "$INSTDIR\UsdPreviewHandler.pyd"
System::Call '*(w r0, w r1, w r2, w r3, w r4, w r5, w r6)p.R1 ?2'
System::Call 'Rstrtmgr::RmRegisterResources(i $RmApplicationSession, i 6, p R1, i 0, p n, i 1, p R2) i.R0'

DetailPrint "Shutting down applications using the USD Shell Extension"

; only shutdown applications that can restart
System::Call 'Rstrtmgr::RmShutdown(i $RmApplicationSession, i ${RmShutdownOnlyRegistered}, p n) i.R0'

FunctionEnd
!macroend
!insertmacro ShutdownApplications "" 
!insertmacro ShutdownApplications "un."

;--------------------------------
!macro RestartApplications UN 
Function ${UN}RestartApplications 

SetDetailsPrint textonly
DetailPrint "Restarting Applications..."
SetDetailsPrint listonly

DetailPrint "Restarting applications that were using the Shell Extension"

System::Call 'Rstrtmgr::RmRestart(i $RmApplicationSession, i 0, p n) i.R0'

System::Call 'Rstrtmgr::RmEndSession(i $RmApplicationSession) i.R0'

FunctionEnd
!macroend
!insertmacro RestartApplications "" 
!insertmacro RestartApplications "un."


;--------------------------------
!macro ShutdownCOMServers UN 
Function ${UN}ShutdownCOMServers 

SetDetailsPrint textonly
DetailPrint "Shutting down COM servers..."
SetDetailsPrint listonly

System::StrAlloc ${CCH_RM_SESSION_KEY}
Pop $0
System::Call 'Rstrtmgr::RmStartSession(*i .R9, i 0, p $0) i.R0'
System::Free $0

StrCpy $0 "$INSTDIR\UsdPreviewLocalServer.exe"
StrCpy $1 "$INSTDIR\UsdPythonToolsLocalServer.exe"
StrCpy $2 "$INSTDIR\UsdSdkToolsLocalServer.exe"

System::Call '*(w r0, w r1, w r2)p.R1 ?2'
System::Call 'Rstrtmgr::RmRegisterResources(i R9, i 3, p R1, i 0, p n, i 0, p n) i.R0'

System::Call 'Rstrtmgr::RmShutdown(i R9, i ${RmForceShutdown}, p n) i.R0'

System::Call 'Rstrtmgr::RmEndSession(i R9) i.R0'

FunctionEnd
!macroend
!insertmacro ShutdownCOMServers "" 
!insertmacro ShutdownCOMServers "un."


;--------------------------------
Section "-ShutdownProcesses" 

Call ShutdownExplorer
Call ShutdownWindowsSearch
Call ShutdownApplications
Call ShutdownCOMServers

SectionEnd

;--------------------------------

; The stuff to install
Section "Install" 

SetDetailsPrint textonly
DetailPrint "Installing files..."
SetDetailsPrint listonly

${Unless} ${FileExists} "$INSTDIR\UsdShellExtension.cfg"
    SetOutPath "$INSTDIR"
    File UsdShellExtension.cfg
${EndUnless}

SetOutPath "$INSTDIR"
File plugInfo.json

SetOutPath "$INSTDIR\usd"
File /r .\usd\*

SetOutPath "$INSTDIR"

!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED tbb.dll "$INSTDIR\tbb.dll" $INSTDIR
!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED tbbmalloc.dll "$INSTDIR\tbbmalloc.dll" $INSTDIR
!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED ${BOOSTDLL} "$INSTDIR\${BOOSTDLL}" $INSTDIR
${Unless} ${FileExists} "${PYTHONDLL}"
    !insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED ${PYTHONDLL} "$INSTDIR\${PYTHONDLL}" $INSTDIR
${EndUnless}
!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED usd_ms.dll "$INSTDIR\usd_ms.dll" $INSTDIR
!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED UsdPreviewHandler.pyd "$INSTDIR\UsdPreviewHandler.pyd" $INSTDIR
!insertmacro InstallLib REGEXE NOTSHARED REBOOT_NOTPROTECTED UsdPreviewLocalServer.exe "$INSTDIR\UsdPreviewLocalServer.exe" $INSTDIR
!insertmacro InstallLib REGEXE NOTSHARED REBOOT_NOTPROTECTED UsdPythonToolsLocalServer.exe "$INSTDIR\UsdPythonToolsLocalServer.exe" $INSTDIR
!insertmacro InstallLib REGEXE NOTSHARED REBOOT_NOTPROTECTED UsdSdkToolsLocalServer.exe "$INSTDIR\UsdSdkToolsLocalServer.exe" $INSTDIR
!insertmacro InstallLib REGDLL NOTSHARED REBOOT_NOTPROTECTED UsdShellExtension.dll "$INSTDIR\UsdShellExtension.dll" $INSTDIR

; Write the installation path into the registry
WriteRegStr HKLM SOFTWARE\Activision\UsdShellExtension "Install_Dir" "$INSTDIR"

; Write the uninstall keys for Windows
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UsdShellExtension" "DisplayName" "Activision USD Shell Extension"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UsdShellExtension" "UninstallString" '"$INSTDIR\uninstall.exe"'
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UsdShellExtension" "NoModify" 1
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UsdShellExtension" "NoRepair" 1
WriteUninstaller "$INSTDIR\uninstall.exe"


SectionEnd

;--------------------------------
Section "-RestartProcesses" 

Call RestartExplorer
Call RestartWindowsSearch
Call RestartApplications

SectionEnd

;--------------------------------

; Uninstaller

;--------------------------------
Section "-Un.ShutdownProcesses" 

Call un.ShutdownExplorer
Call un.ShutdownWindowsSearch
Call un.ShutdownApplications
Call un.ShutdownCOMServers

SectionEnd

Section "Uninstall"

SetDetailsPrint textonly
DetailPrint "Uninstalling files..."
SetDetailsPrint listonly

!insertmacro UnInstallLib REGDLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\UsdShellExtension.dll"
!insertmacro UnInstallLib REGEXE NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\UsdPreviewLocalServer.exe"
!insertmacro UnInstallLib REGEXE NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\UsdPythonToolsLocalServer.exe"
!insertmacro UnInstallLib REGEXE NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\UsdSdkToolsLocalServer.exe"
!insertmacro UnInstallLib DLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\tbb.dll"
!insertmacro UnInstallLib DLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\tbbmalloc.dll"
!insertmacro UnInstallLib DLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\${BOOSTDLL}"
${Unless} ${FileExists} "$INSTDIR\${PYTHONDLL}"
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\${PYTHONDLL}"
${EndUnless}
!insertmacro UnInstallLib DLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\usd_ms.dll"
!insertmacro UnInstallLib DLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\UsdPreviewHandler.pyd"
  
; Remove registry keys
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UsdShellExtension"
DeleteRegKey HKLM SOFTWARE\Activision\UsdShellExtension

; Remove files and uninstaller
;Delete /REBOOTOK "$INSTDIR\UsdShellExtension.cfg"
Delete /REBOOTOK "$INSTDIR\plugInfo.json"
RMDir /r /REBOOTOK "$INSTDIR\usd"
Delete /REBOOTOK "$INSTDIR\UsdPropertyKeys.propdesc"
Delete /REBOOTOK "$INSTDIR\UsdPropertyKeys_Atvi.propdesc"

Delete /REBOOTOK "$INSTDIR\uninstall.exe"

; Remove directories
RMDir "$INSTDIR"

SectionEnd

;--------------------------------
Section "-Un.RestartProcesses" 

Call un.RestartExplorer
Call un.RestartWindowsSearch
Call un.RestartApplications

SectionEnd
