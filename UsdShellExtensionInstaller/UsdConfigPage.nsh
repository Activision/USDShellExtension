;--------------------------------
; UsdConfigPage

Var hWndUsdConfigDlg
Var hWndUsdConfigConfigPath
Var hWndUsdConfigRichEdit

!define /ifndef WS_BORDER           0x00800000
!define /ifndef ES_SUNKEN			0x00004000

!define /ifndef EM_SETTEXTMODE		0x0459
!define /ifndef EM_SETCHARFORMAT    0x0444

!define /ifndef TM_PLAINTEXT		0x1
!define /ifndef TM_RICHTEXT 		0x2	 ; Default behavior 
!define /ifndef TM_SINGLELEVELUNDO	0x4
!define /ifndef TM_MULTILEVELUNDO	0x8  ; Default behavior 
!define /ifndef TM_SINGLECODEPAGE	0x10
!define /ifndef TM_MULTICODEPAGE	0x20 ; Default behavior 

!define SIZEOF_CHARFORMAT           92 ; sizeof(CHARFORMAT)

; CHARFORMAT masks 
!define /ifndef CFM_BOLD		0x00000001
!define /ifndef CFM_ITALIC		0x00000002
!define /ifndef CFM_UNDERLINE	0x00000004
!define /ifndef CFM_STRIKEOUT	0x00000008
!define /ifndef CFM_PROTECTED	0x00000010
!define /ifndef CFM_LINK		0x00000020		; Exchange hyperlink extension 
!define /ifndef CFM_SIZE		0x80000000
!define /ifndef CFM_COLOR		0x40000000
!define /ifndef CFM_FACE		0x20000000
!define /ifndef CFM_OFFSET		0x10000000
!define /ifndef CFM_CHARSET 	0x08000000

; CHARFORMAT effects 
!define /ifndef CFE_BOLD		0x00000001
!define /ifndef CFE_ITALIC		0x00000002
!define /ifndef CFE_UNDERLINE	0x00000004
!define /ifndef CFE_STRIKEOUT	0x00000008
!define /ifndef CFE_PROTECTED	0x00000010
!define /ifndef CFE_LINK		0x00000020
!define /ifndef CFE_AUTOCOLOR	0x40000000		; NOTE: this corresponds to CFM_COLOR, which controls it 

!define /ifndef DEFAULT_PITCH   0
!define /ifndef FIXED_PITCH     1
!define /ifndef VARIABLE_PITCH  2
!define /ifndef MONO_FONT       8

!define /ifndef FF_DONTCARE     0x00  ; Don't care or don't know.
!define /ifndef FF_ROMAN        0x10  ; Variable stroke width, serifed. Times Roman, Century Schoolbook, etc.
!define /ifndef FF_SWISS        0x20  ; Variable stroke width, sans-serifed. Helvetica, Swiss, etc.
!define /ifndef FF_MODERN       0x30  ; Constant stroke width, serifed or sans-serifed. Pica, Elite, Courier, etc.
!define /ifndef FF_SCRIPT       0x40  ; Cursive, etc. 
!define /ifndef FF_DECORATIVE   0x50  ; Old English, etc. 

!define /ifndef LF_FACESIZE     32

!define CHARFORMAT_MASK             ${CFM_FACE}
!define CHARFORMAT_PITCHANDFAMILY   ${MONO_FONT}|${FF_MODERN}

!define USDCONFIGRICHEDIT_STYLE     ${DEFAULT_STYLES}|${WS_TABSTOP}|${ES_AUTOHSCROLL}|${ES_AUTOVSCROLL}|${ES_MULTILINE}|${ES_WANTRETURN}|${ES_SAVESEL}|${WS_HSCROLL}|${WS_VSCROLL}
!define USDCONFIGRICHEDIT_EXSTYLE   ${WS_EX_STATICEDGE}
!define USDCONFIGRICHEDIT_TEXTMODE  ${TM_PLAINTEXT}|${TM_MULTILEVELUNDO}|${TM_MULTICODEPAGE}

!define USDCONFIGCONFIGPATH_STYLE   ${DEFAULT_STYLES}|${WS_TABSTOP}|${ES_AUTOHSCROLL}
!define USDCONFIGCONFIGPATH_EXSTYLE ${WS_EX_STATICEDGE}



Function USDConfigPage
    !insertmacro MUI_HEADER_TEXT "USD Shell Extension Configuration File" "This configuration file contains various options that can be modified at any time."

    System::Call 'KERNEL32::LoadLibrary(t "Msftedit.dll")'

	nsDialogs::Create 1018
	Pop $hWndUsdConfigDlg

	${If} $hWndUsdConfigDlg == error
		Abort
	${EndIf}

    nsDialogs::CreateControl "RICHEDIT50W" "${USDCONFIGRICHEDIT_STYLE}" "${USDCONFIGRICHEDIT_EXSTYLE}" 0 0 100% -18u ""
    Pop $hWndUsdConfigRichEdit
    System::Call 'USER32::SendMessage(i $hWndUsdConfigRichEdit, i ${EM_SETTEXTMODE}, i ${USDCONFIGRICHEDIT_TEXTMODE}, i 0 )'

    System::Alloc ${SIZEOF_CHARFORMAT}
    Pop $0
    System::Call '*$0(i ${SIZEOF_CHARFORMAT}, i ${CHARFORMAT_MASK}, i 0, i 0, i 0, i 0, b 0, b ${CHARFORMAT_PITCHANDFAMILY}, t "" )'
    System::Call 'USER32::SendMessage(i $hWndUsdConfigRichEdit, i ${EM_SETCHARFORMAT}, i 0, p r0 )'
    System::Free $0

    nsDialogs::CreateControl "${__NSD_Text_CLASS}" "${USDCONFIGCONFIGPATH_STYLE}" "${USDCONFIGCONFIGPATH_EXSTYLE}" 0 -14u 100% 12u "$INSTDIR\UsdShellExtension.cfg"
    Pop $hWndUsdConfigConfigPath
    ${NSD_Edit_SetReadOnly} $hWndUsdConfigConfigPath 1

    !insertmacro LoadRichEditContentsFromConfigFile $hWndUsdConfigRichEdit "$INSTDIR\UsdShellExtension.cfg"

	nsDialogs::Show
FunctionEnd

Function USDConfigPageLeave
    !insertmacro SaveRichEditContentsToConfigFile $hWndUsdConfigRichEdit "$INSTDIR\UsdShellExtension.cfg"
FunctionEnd

