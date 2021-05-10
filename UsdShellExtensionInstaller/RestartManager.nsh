;--------------------------------
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
