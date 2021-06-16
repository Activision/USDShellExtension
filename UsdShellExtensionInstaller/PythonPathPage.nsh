;--------------------------------
; UsdPathPage

Var hWndPythonPathDlg
Var hWndPythonPathEditPath
Var hWndPythonPathEditPythonPath
Var hWndPythonPathButtonBuild

Function PythonPathPage
    !insertmacro MUI_HEADER_TEXT "Python ${PYTHON_VERSION}" "Please set the following Python ${PYTHON_VERSION} environment variables."

	nsDialogs::Create 1018
	Pop $hWndPythonPathDlg

	${If} $hWndPythonPathDlg == error
		Abort
	${EndIf}

    SetShellVarContext all

    ${NSD_CreateLabel} 0 0 100% 10u "PATH"
    !insertmacro ReadConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "Python" "PATH" ""
    Pop $R0
    ${If} $R0 == ""
        ReadRegStr $R0 HKCU "SOFTWARE\Python\PythonCore\${PYTHON_VERSION}\InstallPath" ""
        ${If} $R0 == ""
            ReadRegStr $R0 HKLM "SOFTWARE\Python\PythonCore\${PYTHON_VERSION}\InstallPath" ""
        ${EndIf}
    ${EndIf}
    ${NSD_CreateText} 0 10u 100% 12u $R0
    Pop $hWndPythonPathEditPath

    ${NSD_CreateLabel} 0 28u 100% 10u "PYTHONPATH"
    !insertmacro ReadConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "Python" "PYTHONPATH" ""
    Pop $R0
    ${If} $R0 == ""
        ReadRegStr $R0 HKCU "SOFTWARE\Python\PythonCore\${PYTHON_VERSION}\PythonPath" ""
        ${If} $R0 == ""
            ReadRegStr $R0 HKLM "SOFTWARE\Python\PythonCore\${PYTHON_VERSION}\PythonPath" ""
        ${EndIf}
    ${EndIf}
    ${NSD_CreateText} 0 38u 100% 12u $R0
    Pop $hWndPythonPathEditPythonPath

    ${NSD_CreateButton} -140u 62u 140u 15u "Set using root Python ${PYTHON_VERSION} folder"
    Pop $hWndPythonPathButtonBuild
    ${NSD_OnClick} $hWndPythonPathButtonBuild PythonPathPageBuildClick

	nsDialogs::Show
FunctionEnd

Function PythonPathPageLeave
    SetShellVarContext all

	${NSD_GetText} $hWndPythonPathEditPath $0

    IfFileExists "$0\python.exe" PythonFound
    MessageBox MB_ICONSTOP|MB_OK "Python.exe was not found at $0"
    Abort

PythonFound:
	!insertmacro WriteConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "Python" "PATH" $0

	${NSD_GetText} $hWndPythonPathEditPythonPath $0
	!insertmacro WriteConfigFile "$LOCALAPPDATA\Activision\UsdShellExtension\UsdShellExtension.ini" "Python" "PYTHONPATH" $0

FunctionEnd

Function PythonPathPageBuildClick

    nsDialogs::SelectFolderDialog "Select root folder of Python ${PYTHON_VERSION} installation"
    Pop $R0

	${If} $R0 != error
        ${NSD_SetText} $hWndPythonPathEditPath "$R0"
        ${NSD_SetText} $hWndPythonPathEditPythonPath "$R0\Lib\;$R0\DLLs\"
	${EndIf}

FunctionEnd
