;--------------------------------
;Includes

!include "MUI2.nsh"
!include "Library.nsh"
!include "logiclib.nsh"

!define LIBRARY_X64

!define /ifndef OUT_FILE "UsdShellExtension-setup.exe"
!define NAME_LONG "Activision USD Shell Extension"

!define /ifndef VER_MAJOR 0
!define /ifndef VER_MINOR 00
!define /ifndef VER_REVISION 00
!define /ifndef VER_BUILD 00

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
;Utilities
!include "${__FILEDIR__}\UsdConfigUtils.nsh"
!include "${__FILEDIR__}\RestartManager.nsh"

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING

;--------------------------------
;Pages
!include "${__FILEDIR__}\UsdPathPage.nsh"
!include "${__FILEDIR__}\UsdConfigPage.nsh"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_INSTFILES
Page custom USDPathPage USDPathPageLeave
Page custom USDConfigPage USDConfigPageLeave
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

;--------------------------------
Section "-ShutdownProcesses" 

Call ShutdownExplorer
Call ShutdownWindowsSearch
Call ShutdownApplications
Call ShutdownCOMServers

SectionEnd

;--------------------------------
Section "-UninstallPrevious" 

ReadRegStr $R0 HKLM \
"Software\Microsoft\Windows\CurrentVersion\Uninstall\UsdShellExtension" \
"UninstallString"
StrCmp $R0 "" done

SetDetailsPrint textonly
DetailPrint "Uninstalling previous installation..."
SetDetailsPrint listonly
DetailPrint "Uninstalling previous installation"

ClearErrors
ExecWait '$R0 /S _?=$INSTDIR' ;Do not copy the uninstaller to a temp file
IfErrors no_remove_uninstaller done
;You can either use Delete /REBOOTOK in the uninstaller or add some code
;here to remove the uninstaller. Use a registry key to check
;whether the user has chosen to uninstall. If you are using an uninstaller
;components page, make sure all sections are uninstalled.
no_remove_uninstaller:

done:

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
Section "-UpdateConfigFile" 

; In order to support updates to the config file and allow for us 
; to support going back to older versions of the shell extension, 
; we will "patch" the existing config file using GetPrivateProfileStringW 
; to determine if a value is already set and SetPrivateProfileStringW 
; to enter a blank / default value if no value was set.

SetDetailsPrint textonly
DetailPrint "Updating config file..."
SetDetailsPrint listonly


!insertmacro PatchConfigFile "$INSTDIR\UsdShellExtension.cfg" "USD" "PATH" ""
!insertmacro PatchConfigFile "$INSTDIR\UsdShellExtension.cfg" "USD" "PYTHONPATH" ""
!insertmacro PatchConfigFile "$INSTDIR\UsdShellExtension.cfg" "USD" "PXR_PLUGINPATH_NAME" ""
!insertmacro PatchConfigFile "$INSTDIR\UsdShellExtension.cfg" "USD" "EDITOR" ""

!insertmacro PatchConfigFile "$INSTDIR\UsdShellExtension.cfg" "RENDERER" "PREVIEW" "GL"
!insertmacro PatchConfigFile "$INSTDIR\UsdShellExtension.cfg" "RENDERER" "THUMBNAIL" "GL"
!insertmacro PatchConfigFile "$INSTDIR\UsdShellExtension.cfg" "RENDERER" "VIEW" "GL"

!insertmacro PatchConfigFile "$INSTDIR\UsdShellExtension.cfg" "PYTHON" "PATH" ""
!insertmacro PatchConfigFile "$INSTDIR\UsdShellExtension.cfg" "PYTHON" "PYTHONPATH" ""

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
