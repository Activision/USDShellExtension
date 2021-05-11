;--------------------------------
; UsdPathPage

Var hWndUsdPathDlg
Var hWndUsdPathEditPath
Var hWndUsdPathEditPythonPath
Var hWndUsdPathEditPxrPluginPath
Var hWndUsdPathButtonBuild
;Var hWndUsdPathConfigPath

Function USDPathPage
    !insertmacro MUI_HEADER_TEXT "USD Libraries and Tools" "Please set the following USD environment variables."

	nsDialogs::Create 1018
	Pop $hWndUsdPathDlg

	${If} $hWndUsdPathDlg == error
		Abort
	${EndIf}

    ${NSD_CreateLabel} 0 0 100% 10u "PATH"
    !insertmacro ReadConfigFile "$INSTDIR\UsdShellExtension.cfg" "USD" "PATH" ""
    Pop $R0
    ${NSD_CreateText} 0 10u 100% 12u $R0
    Pop $hWndUsdPathEditPath

    ${NSD_CreateLabel} 0 28u 100% 10u "PYTHONPATH"
    !insertmacro ReadConfigFile "$INSTDIR\UsdShellExtension.cfg" "USD" "PYTHONPATH" ""
    Pop $R0
    ${NSD_CreateText} 0 38u 100% 12u $R0
    Pop $hWndUsdPathEditPythonPath

    ${NSD_CreateLabel} 0 56u 100% 10u "PXR_PLUGINPATH_NAME"
    !insertmacro ReadConfigFile "$INSTDIR\UsdShellExtension.cfg" "USD" "PXR_PLUGINPATH_NAME" ""
    Pop $R0
    ${NSD_CreateText} 0 66u 100% 12u $R0
    Pop $hWndUsdPathEditPxrPluginPath

    ${NSD_CreateButton} -140u 90u 140u 15u "Set using root USD folder"
    Pop $hWndUsdPathButtonBuild
    ${NSD_OnClick} $hWndUsdPathButtonBuild USDPathPageBuildClick

    ;${NSD_CreateLabel} 0 -30u 100% 10u "Configuration File"
    ;${NSD_CreateText} 0 -18u 100% 12u "$INSTDIR\UsdShellExtension.cfg"
    ;Pop $hWndUsdPathConfigPath
    ;${NSD_Edit_SetReadOnly} $hWndUsdPathConfigPath 1

	nsDialogs::Show
FunctionEnd

Function USDPathPageLeave

	${NSD_GetText} $hWndUsdPathEditPath $0
	!insertmacro WriteConfigFile "$INSTDIR\UsdShellExtension.cfg" "USD" "PATH" $0

	${NSD_GetText} $hWndUsdPathEditPythonPath $0
	!insertmacro WriteConfigFile "$INSTDIR\UsdShellExtension.cfg" "USD" "PYTHONPATH" $0

	${NSD_GetText} $hWndUsdPathEditPxrPluginPath $0
	!insertmacro WriteConfigFile "$INSTDIR\UsdShellExtension.cfg" "USD" "PXR_PLUGINPATH_NAME" $0

FunctionEnd

Function USDPathPageBuildClick

    nsDialogs::SelectFolderDialog "Select root folder of USD installation"
    Pop $R0

	${If} $R0 != error
        ${NSD_SetText} $hWndUsdPathEditPath "$R0\bin\;$R0\lib\"
        ${NSD_SetText} $hWndUsdPathEditPythonPath "$R0\lib\python"
        ${NSD_SetText} $hWndUsdPathEditPxrPluginPath ""
	${EndIf}

FunctionEnd
