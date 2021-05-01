;--------------------------------
;Includes

!include "MUI2.nsh"
!include "Library.nsh"
!define LIBRARY_X64

!define NAME_SHORT "UsdShellExtension"
!define NAME_LONG "Activision USD Shell Extension"

!define /ifndef VER_MAJOR 0
!define /ifndef VER_MINOR 0
!define /ifndef VER_REVISION 0
!define /ifndef VER_BUILD 0

!define /ifndef VERSION "Unknown Version"
!define /ifndef USD_VERSION "Unknown Version"
!define /ifndef PYTHON_VERSION "Unknown Version"

; The name of the installer
Name "${NAME_LONG}"

; The file to write
OutFile "${NAME_SHORT}-setup.exe"

; Request application privileges for Windows Vista and higher
RequestExecutionLevel admin

; Build Unicode installer
Unicode True

SetCompress off
SetCompressor zlib

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
!insertmacro MUI_UNPAGE_CONFIRM
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

;--------------------------------

; The stuff to install
Section "Install" 

; Set output path to the installation directory.
SetOutPath "$INSTDIR"

; Put file there

;File tbbmalloc.dll
File UsdShellExtension.cfg
File plugInfo.json

SetOutPath "$INSTDIR\usd"
File /r .\usd\*

SetOutPath "$INSTDIR"

!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED tbb.dll "$INSTDIR\tbb.dll" $INSTDIR
!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED boost_python37-vc142-mt-x64-1_70.dll "$INSTDIR\boost_python37-vc142-mt-x64-1_70.dll" $INSTDIR
!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED python37.dll "$INSTDIR\python37.dll" $INSTDIR
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

; Uninstaller

Section "Uninstall"

!insertmacro UnInstallLib DLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\tbb.dll"
!insertmacro UnInstallLib DLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\boost_python37-vc142-mt-x64-1_70.dll"
!insertmacro UnInstallLib DLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\python37.dll"
!insertmacro UnInstallLib DLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\usd_ms.dll"
!insertmacro UnInstallLib DLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\UsdPreviewHandler.pyd"
!insertmacro UnInstallLib REGEXE NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\UsdPreviewLocalServer.exe"
!insertmacro UnInstallLib REGEXE NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\UsdPythonToolsLocalServer.exe"
!insertmacro UnInstallLib REGEXE NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\UsdSdkToolsLocalServer.exe"
!insertmacro UnInstallLib REGDLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\UsdShellExtension.dll"
  
; Remove registry keys
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UsdShellExtension"
DeleteRegKey HKLM SOFTWARE\Activision\UsdShellExtension

; Remove files and uninstaller
Delete /REBOOTOK "$INSTDIR\UsdShellExtension.cfg"
Delete /REBOOTOK "$INSTDIR\plugInfo.json"
RMDir /r /REBOOTOK "$INSTDIR\usd"


Delete /REBOOTOK "$INSTDIR\UsdShellExtensionInstaller.nsi"
Delete /REBOOTOK "$INSTDIR\uninstall.exe"

; Remove directories
RMDir "$INSTDIR"

SectionEnd


