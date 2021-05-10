!define /ifndef GENERIC_READ                        0x80000000
!define /ifndef GENERIC_WRITE                       0x40000000

!define /ifndef FILE_SHARE_READ                     0x00000001  
!define /ifndef FILE_SHARE_WRITE                    0x00000002  
!define /ifndef FILE_SHARE_DELETE                   0x00000004  

!define /ifndef FILE_ATTRIBUTE_READONLY             0x00000001  
!define /ifndef FILE_ATTRIBUTE_HIDDEN               0x00000002  
!define /ifndef FILE_ATTRIBUTE_SYSTEM               0x00000004  
!define /ifndef FILE_ATTRIBUTE_DIRECTORY            0x00000010  
!define /ifndef FILE_ATTRIBUTE_ARCHIVE              0x00000020  
!define /ifndef FILE_ATTRIBUTE_DEVICE               0x00000040  
!define /ifndef FILE_ATTRIBUTE_NORMAL               0x00000080  
!define /ifndef FILE_ATTRIBUTE_TEMPORARY            0x00000100  
!define /ifndef FILE_ATTRIBUTE_SPARSE_FILE          0x00000200  
!define /ifndef FILE_ATTRIBUTE_REPARSE_POINT        0x00000400  
!define /ifndef FILE_ATTRIBUTE_COMPRESSED           0x00000800  
!define /ifndef FILE_ATTRIBUTE_OFFLINE              0x00001000  
!define /ifndef FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  0x00002000  
!define /ifndef FILE_ATTRIBUTE_ENCRYPTED            0x00004000  
!define /ifndef FILE_ATTRIBUTE_INTEGRITY_STREAM     0x00008000  
!define /ifndef FILE_ATTRIBUTE_VIRTUAL              0x00010000  
!define /ifndef FILE_ATTRIBUTE_NO_SCRUB_DATA        0x00020000  
!define /ifndef FILE_ATTRIBUTE_EA                   0x00040000  
!define /ifndef FILE_ATTRIBUTE_PINNED               0x00080000  
!define /ifndef FILE_ATTRIBUTE_UNPINNED             0x00100000  
!define /ifndef FILE_ATTRIBUTE_RECALL_ON_OPEN       0x00040000  
!define /ifndef FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS 0x00400000 


!define /ifndef CREATE_NEW                          1
!define /ifndef CREATE_ALWAYS                       2
!define /ifndef OPEN_EXISTING                       3
!define /ifndef OPEN_ALWAYS                         4
!define /ifndef TRUNCATE_EXISTING                   5

!define /ifndef INVALID_HANDLE_VALUE                -1 ; 0xFFFFFFFF

!define /ifndef UTF16LE_BOM                         65279 ; 0xFEFF

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

Function LoadRichEditContentsFromConfigFile
    pop $0 ; hWndRichEdit
    pop $1 ; file

    System::Call 'Kernel32::CreateFileW(t r1, i ${GENERIC_READ}, i ${FILE_SHARE_READ}, p n, i ${OPEN_EXISTING}, i 0, p n) p.R0'
    ${If} $R0 == ${INVALID_HANDLE_VALUE}
        Return
    ${EndIf}

    System::Call 'Kernel32::GetFileSize(p R0, p n) i.R1'
    System::Alloc $R1
    Pop $R2

    System::Call 'Kernel32::ReadFile(p R0, p R2, i R1, p n, p n) i.R3'
    System::Call 'Kernel32::CloseHandle(p R0)'

    System::Call '*$R2(h .R3)'
    ${If} $R3 == ${UTF16LE_BOM}
        IntOp $R2 $R2 + 2 ; skip the 2 byte BOM
        System::Call 'User32::SetWindowTextW(p r0, p R2)'
    ${Else}    
        System::Call 'User32::SetWindowTextA(p r0, p R2)'
    ${EndIf}
   
   System::Free $R2

FunctionEnd

!macro LoadRichEditContentsFromConfigFile hWndRichEdit file
  push `${file}`
  push `${hWndRichEdit}`

  Call LoadRichEditContentsFromConfigFile
!macroend





Function SaveRichEditContentsToConfigFile
    pop $0 ; hWndRichEdit
    pop $1 ; file

    System::Call 'Kernel32::CreateFileW(t r1, i ${GENERIC_WRITE}, i 0, p n, i ${CREATE_ALWAYS}, i ${FILE_ATTRIBUTE_NORMAL}, p n) p.R0'
    ${If} $R0 == ${INVALID_HANDLE_VALUE}
        Return
    ${EndIf}


    ; Write the contents of the window
    System::Call 'User32::GetWindowTextLength(p r0) i.R1'
    IntOp $R1 $R1 + 1
    IntOp $R3 $R1 * 2 ; $R1 = characters, $R3 = bytes
    System::Alloc $R3
    Pop $R2
    System::Call 'User32::GetWindowTextW(p r0, p R2, i R1) i.R1'
    IntOp $R3 $R1 * 2 ; $R1 = characters, $R3 = bytes

    System::Call '*$R2(h .R4)'
    ${If} $R4 != ${UTF16LE_BOM}
        ; Write the UTF16LE BOM
        System::Alloc 2
        Pop $R4
        System::Call '*$R4(h ${UTF16LE_BOM})'
        System::Call 'Kernel32::WriteFile(p R0, p R4, i 2, p n, p n)'
        System::Free $R4
    ${EndIf}

    System::Call 'Kernel32::WriteFile(p R0, p R2, i R3, p n, p n)'
    System::Free $R2

    System::Call 'Kernel32::CloseHandle(p R0)'

FunctionEnd

!macro SaveRichEditContentsToConfigFile hWndRichEdit file
  push `${file}`
  push `${hWndRichEdit}`

  Call SaveRichEditContentsToConfigFile
!macroend