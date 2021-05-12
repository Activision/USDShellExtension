;--------------------------------
; UsdConfigPage

!define /ifndef WS_BORDER           0x00800000

!define USDCONFIGCONFIGPATH_STYLE   ${DEFAULT_STYLES}|${WS_BORDER}|${ES_AUTOHSCROLL}
!define USDCONFIGCONFIGPATH_EXSTYLE 0;${WS_EX_STATICEDGE}

Var hWndUsdConfigDlg
Var hWndUsdConfigConfigPath
Var hWndUsdConfigButtonShow

Function USDConfigPage
    !insertmacro MUI_HEADER_TEXT "USD Shell Extension Configuration File" ""

	nsDialogs::Create 1018
	Pop $hWndUsdConfigDlg

	${If} $hWndUsdConfigDlg == error
		Abort
	${EndIf}

    ${NSD_CreateLabel} 0 0 100% 20u "This file contains the configuration for the USD Shell Extension. \
        The configuration can be modified at any time and the changes will be reflected immeditately."

    nsDialogs::CreateControl "${__NSD_Text_CLASS}" "${USDCONFIGCONFIGPATH_STYLE}" "${USDCONFIGCONFIGPATH_EXSTYLE}" 0 30u 100% 12u "$INSTDIR\UsdShellExtension.ini"
    Pop $hWndUsdConfigConfigPath
    ${NSD_Edit_SetReadOnly} $hWndUsdConfigConfigPath 1

    ${NSD_CreateButton} -100u 65u 100u 15u "Show in Explorer"
    Pop $hWndUsdConfigButtonShow
    ${NSD_OnClick} $hWndUsdConfigButtonShow USDConfigPageShowClick


	nsDialogs::Show
FunctionEnd

Function USDConfigPageLeave

FunctionEnd

Function USDConfigPageShowClick

    ExecWait 'explorer.exe /n,/select,"$INSTDIR\UsdShellExtension.ini"'
FunctionEnd
