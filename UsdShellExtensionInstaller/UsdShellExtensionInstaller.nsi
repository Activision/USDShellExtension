;--------------------------------
;Includes

!include "MUI2.nsh"
!include "Library.nsh"
!include "logiclib.nsh"
!include "x64.nsh"

!define LIBRARY_X64

!define /ifndef OUT_FILE "setup.exe"

!define /ifndef VER_MAJOR 0
!define /ifndef VER_MINOR 00
!define /ifndef VER_REVISION 00
!define /ifndef VER_BUILD 00
!define /ifndef VER_PRODUCTNAME ""
!define /ifndef VER_COMPANYNAME ""
!define /ifndef VER_COPYRIGHT ""

!define /ifndef VERSION "${VER_MAJOR}.${VER_MINOR}"
!define /ifndef USD_VERSION "Unknown Version"
!define /ifndef PYTHON_VERSION "Unknown Version"

; The name of the installer
Name "${VER_PRODUCTNAME}"

; The file to write
OutFile "${OUT_FILE}"

; Request application privileges for Windows Vista and higher
RequestExecutionLevel admin

; Build Unicode installer
Unicode True

;SetCompress off
SetCompressor LZMA

; The default installation directory
InstallDir $PROGRAMFILES64\Activision\UsdShellExtension

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

!define MUI_WELCOMEPAGE_TITLE "${VER_PRODUCTNAME}$\r$\nVersion ${VERSION}"
!define MUI_WELCOMEPAGE_TEXT "Setup will guide you through the installation of $(^NameDA).$\r$\n$\r$\nUSD Version: ${USD_VERSION}$\r$\nPython Version: ${PYTHON_VERSION}$\r$\n$\r$\nIt is recommended that you close all other applications before starting Setup. This will make it possible to update relevant system files without having to reboot your computer.$\r$\n$\r$\n$_CLICK"

;--------------------------------
;Utilities
!include "${__FILEDIR__}\UsdConfigUtils.nsh"
!include "${__FILEDIR__}\RestartManager.nsh"
!include "${__FILEDIR__}\ShellLinkSetRunAs.nsh"
!include "${__FILEDIR__}\CmdLineArgs.nsh"

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
VIAddVersionKey "ProductVersion" "${VERSION}"
VIAddVersionKey "FileDescription" "${VER_PRODUCTNAME} Setup"
VIAddVersionKey "ProductName" "${VER_PRODUCTNAME}"
VIAddVersionKey "LegalCopyright" "${VER_COPYRIGHT}"
VIAddVersionKey "CompanyName" "${VER_COMPANYNAME}"
!endif

SetPluginUnload  alwaysoff

;--------------------------------
Function .onInit

Call ParseCommandLine

FunctionEnd


;--------------------------------
Section "-ShutdownProcesses" 

${DisableX64FSRedirection}
SetRegView 64

Call ShutdownExplorer
Call ShutdownWindowsSearch
Call ShutdownApplications
Call ShutdownCOMServers

SectionEnd

;--------------------------------
Section "-UninstallPrevious" 

${DisableX64FSRedirection}
SetRegView 64

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

${DisableX64FSRedirection}
SetRegView 64

SetDetailsPrint textonly
DetailPrint "Installing files..."
SetDetailsPrint listonly

SetShellVarContext all
${Unless} ${FileExists} "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini"
    SetOutPath "$LOCALAPPDATA\Activision\UsdShellExtension"
    File UsdShellExtension.ini
${EndUnless}
SetShellVarContext current
${Unless} ${FileExists} "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini"
    SetOutPath "$LOCALAPPDATA\Activision\UsdShellExtension"
    File UsdShellExtension.ini
${EndUnless}

SetShellVarContext all
SetOutPath "$INSTDIR"
File plugInfo.json

SetOutPath "$INSTDIR\usd"
File /r .\usd\*

SetOutPath "$INSTDIR"

!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED tbb.dll "$INSTDIR\tbb.dll" $INSTDIR
!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED tbbmalloc.dll "$INSTDIR\tbbmalloc.dll" $INSTDIR
!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED ${BOOSTDLL} "$INSTDIR\${BOOSTDLL}" $INSTDIR
!if /FileExists "${PYTHONDLL}"
    !insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED ${PYTHONDLL} "$INSTDIR\${PYTHONDLL}" $INSTDIR
!endif
!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED usd_ms.dll "$INSTDIR\usd_ms.dll" $INSTDIR
!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED UsdPreviewHandler.pyd "$INSTDIR\UsdPreviewHandler.pyd" $INSTDIR
!insertmacro InstallLib REGEXE NOTSHARED REBOOT_NOTPROTECTED UsdPreviewLocalServer.exe "$INSTDIR\UsdPreviewLocalServer.exe" $INSTDIR
!insertmacro InstallLib REGEXE NOTSHARED REBOOT_NOTPROTECTED UsdPythonToolsLocalServer.exe "$INSTDIR\UsdPythonToolsLocalServer.exe" $INSTDIR
!insertmacro InstallLib REGEXE NOTSHARED REBOOT_NOTPROTECTED UsdSdkToolsLocalServer.exe "$INSTDIR\UsdSdkToolsLocalServer.exe" $INSTDIR
!insertmacro InstallLib REGDLL NOTSHARED REBOOT_NOTPROTECTED UsdShellExtension.dll "$INSTDIR\UsdShellExtension.dll" $INSTDIR

; Write the installation path into the registry
WriteRegStr HKLM SOFTWARE\Activision\UsdShellExtension "Install_Dir" "$INSTDIR"

; Write the uninstall keys for Windows
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UsdShellExtension" "DisplayName" "${VER_PRODUCTNAME}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UsdShellExtension" "DisplayIcon" "$INSTDIR\UsdShellExtension.dll,0"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UsdShellExtension" "UninstallString" '"$INSTDIR\uninstall.exe"'
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UsdShellExtension" "NoModify" 1
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UsdShellExtension" "NoRepair" 1
WriteUninstaller "$INSTDIR\uninstall.exe"

; Install start menu items
SetShellVarContext current
CreateDirectory '$SMPROGRAMS\USD Shell Extension'
CreateShortCut '$SMPROGRAMS\USD Shell Extension\USD Shell Extension Configuration (Current User).lnk' '"$SYSDIR\NOTEPAD.EXE"' '"$LOCALAPPDATA\Activision\UsdShellExtension\USDShellExtension.ini"' '$SYSDIR\imageres.dll' 64

SetShellVarContext all
CreateDirectory '$SMPROGRAMS\USD Shell Extension'
CreateShortCut '$SMPROGRAMS\USD Shell Extension\USD Shell Extension Configuration (All Users).lnk' '"$SYSDIR\NOTEPAD.EXE"' '"$LOCALAPPDATA\Activision\UsdShellExtension\USDShellExtension.ini"' '$SYSDIR\imageres.dll' 64
CreateShortCut '$SMPROGRAMS\USD Shell Extension\Uninstall USD Shell Extension.lnk' '$INSTDIR\uninstall.exe' ""
!insertmacro ShellLinkSetRunAs "$SMPROGRAMS\USD Shell Extension\USD Shell Extension Configuration (All Users).lnk"

; Write version info to registry
WriteRegStr HKLM "Software\Activision\UsdShellExtension" "Version" "${VER_MAJOR}.${VER_MINOR}.${VER_REVISION}.${VER_BUILD}"
WriteRegStr HKLM "Software\Activision\UsdShellExtension" "USD Version" "${USD_VERSION}"
WriteRegStr HKLM "Software\Activision\UsdShellExtension" "Python Version" "${PYTHON_VERSION}"
WriteRegStr HKLM "Software\Activision\UsdShellExtension" "Installer" "${OUT_FILE}"

SectionEnd

;--------------------------------
Section "-RestartProcesses" 

${DisableX64FSRedirection}
SetRegView 64

Call RestartExplorer
Call RestartWindowsSearch
Call RestartApplications

SectionEnd

;--------------------------------
Function PatchConfigFileAll

!insertmacro PatchConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "USD" "PATH" ""
!insertmacro PatchConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "USD" "PYTHONPATH" ""
!insertmacro PatchConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "USD" "PXR_PLUGINPATH_NAME" ""
!insertmacro PatchConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "USD" "EDITOR" ""

!insertmacro PatchConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "RENDERER" "PREVIEW" "GL"
!insertmacro PatchConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "RENDERER" "THUMBNAIL" "GL"
!insertmacro PatchConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "RENDERER" "VIEW" "GL"

!insertmacro PatchConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "PYTHON" "PATH" ""
!insertmacro PatchConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "PYTHON" "PYTHONPATH" ""

FunctionEnd

;--------------------------------
Function ForceConfigFileAll

SetShellVarContext all

${If} $CmdLineUsdPath != ""
    !insertmacro WriteConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "USD" "PATH" $CmdLineUsdPath
${EndIf}

${If} $CmdLineUsdPythonPath != ""
    !insertmacro WriteConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "USD" "PYTHONPATH" $CmdLineUsdPythonPath
${EndIf}

${If} $CmdLineUsdPxrPluginPathName != ""
    !insertmacro WriteConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "USD" "PXR_PLUGINPATH_NAME" $CmdLineUsdPxrPluginPathName
${EndIf}


${If} $CmdLinePythonPath != ""
    !insertmacro WriteConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "PYTHON" "PATH" $CmdLinePythonPath
${EndIf}

${If} $CmdLinePythonPythonPath != ""
    !insertmacro WriteConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "PYTHON" "PYTHONPATH" $CmdLinePythonPythonPath
${EndIf}


${If} $CmdLineRendererPreview != ""
    !insertmacro WriteConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "RENDERER" "PREVIEW" $CmdLineRendererPreview
${EndIf}

${If} $CmdLineRendererThumbnail != ""
    !insertmacro WriteConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "RENDERER" "THUMBNAIL" $CmdLineRendererThumbnail
${EndIf}

${If} $CmdLineRendererView != ""
    !insertmacro WriteConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "RENDERER" "VIEW" $CmdLineRendererView
${EndIf}

FunctionEnd

;--------------------------------
Section "-UpdateConfigFile" 

${DisableX64FSRedirection}
SetRegView 64

; In order to support updates to the config file and allow for us 
; to support going back to older versions of the shell extension, 
; we will "patch" the existing config file using GetPrivateProfileStringW 
; to determine if a value is already set and SetPrivateProfileStringW 
; to enter a blank / default value if no value was set.

SetDetailsPrint textonly
DetailPrint "Updating config file..."
SetDetailsPrint listonly

SetShellVarContext current
Call PatchConfigFileAll
SetShellVarContext all
Call PatchConfigFileAll

; Force any command line settings into the config file
Call ForceConfigFileAll

SectionEnd

;--------------------------------

; Uninstaller

;--------------------------------
Section "-Un.ShutdownProcesses" 

${DisableX64FSRedirection}
SetRegView 64

Call un.ShutdownExplorer
Call un.ShutdownWindowsSearch
Call un.ShutdownApplications
Call un.ShutdownCOMServers

SectionEnd

;--------------------------------
Section "Uninstall"

${DisableX64FSRedirection}
SetRegView 64
SetShellVarContext all

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
!if /FileExists "${PYTHONDLL}"
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\${PYTHONDLL}"
!endif
!insertmacro UnInstallLib DLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\usd_ms.dll"
!insertmacro UnInstallLib DLL NOTSHARED REBOOT_NOTPROTECTED "$INSTDIR\UsdPreviewHandler.pyd"
  
; Remove registry keys
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UsdShellExtension"
DeleteRegKey HKLM SOFTWARE\Activision\UsdShellExtension

; Remove files and uninstaller
;Delete /REBOOTOK "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini"
Delete /REBOOTOK "$INSTDIR\plugInfo.json"
RMDir /r /REBOOTOK "$INSTDIR\usd"
Delete /REBOOTOK "$INSTDIR\UsdPropertyKeys.propdesc"
Delete /REBOOTOK "$INSTDIR\UsdPropertyKeys_Atvi.propdesc"

Delete /REBOOTOK "$INSTDIR\uninstall.exe"

; Remove directories
RMDir "$INSTDIR"

; Remove start menu items
SetShellVarContext current
Delete  '$SMPROGRAMS\USD Shell Extension\USD Shell Extension Configuration (Current User).lnk'
SetShellVarContext all
Delete  '$SMPROGRAMS\USD Shell Extension\USD Shell Extension Configuration (All Users).lnk'
Delete '$SMPROGRAMS\USD Shell Extension\Uninstall USD Shell Extension.lnk'
RMDir '$SMPROGRAMS\USD Shell Extension'

SectionEnd

;--------------------------------
Section "-Un.RestartProcesses" 

${DisableX64FSRedirection}
SetRegView 64

Call un.RestartExplorer
Call un.RestartWindowsSearch
Call un.RestartApplications

SectionEnd
