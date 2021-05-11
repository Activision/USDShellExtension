
Function ReadConfigFile
    pop $0 ; file
    pop $1 ; section
    pop $2 ; key
    pop $3 ; value

    System::StrAlloc 1024
    Pop $R1
    System::Call 'Kernel32::GetPrivateProfileStringW(w r1, w r2, t "", p R1, i 1024, w r0)'
    ; we can only store a max of 1024 characters in NSIS registers
    System::Call "*$R1(&t1024 .s)"
    System::Free $R1
FunctionEnd

!macro ReadConfigFile file section key value
  push `${value}`
  push `${key}`
  push `${section}`
  push `${file}`

  Call ReadConfigFile
!macroend

Function WriteConfigFile
    pop $0 ; file
    pop $1 ; section
    pop $2 ; key
    pop $3 ; value

    System::Call 'Kernel32::WritePrivateProfileStringW(w r1, w r2, w r3, w r0)'
FunctionEnd

!macro WriteConfigFile file section key value
  push `${value}`
  push `${key}`
  push `${section}`
  push `${file}`

  Call WriteConfigFile
!macroend

Function PatchConfigFile
    pop $0 ; file
    pop $1 ; section
    pop $2 ; key
    pop $3 ; value

    ; a string that someone isn't going to enter by hand
    StrCpy $R0 "{3D3157AE-140F-49B9-9283-57051DB19E37}" 
    System::StrAlloc 8192
    Pop $R1
    System::Call 'Kernel32::GetPrivateProfileStringW(w r1, w r2, w R0, p R1, i 8192, w r0)'
    ; copy the returned string into $R2
    ; we can only store a max of 1024 characters in NSIS registers
    System::Call "*$R1(&t1024 .R2)"
    System::Free $R1

    ${If} $R2 == "{3D3157AE-140F-49B9-9283-57051DB19E37}"
        System::Call 'Kernel32::WritePrivateProfileStringW(w r1, w r2, w r3, w r0)'
    ${EndIf}

FunctionEnd

!macro PatchConfigFile file section key value
  push `${value}`
  push `${key}`
  push `${section}`
  push `${file}`

  Call PatchConfigFile
!macroend

