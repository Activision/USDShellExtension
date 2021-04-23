

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 19:14:07 2038
 */
/* Compiler settings for ShellExt.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        EXTERN_C __declspec(selectany) const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif // !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_UsdShellExtensionLib,0xB71120F9,0xA3EA,0x4153,0x8C,0x32,0xC9,0x06,0xCC,0xE1,0xF6,0xF2);


MIDL_DEFINE_GUID(CLSID, CLSID_UsdContextMenu,0x0BCB74E0,0x5B61,0x4F24,0xBA,0xB3,0x96,0xCC,0x76,0xFE,0x36,0x72);


MIDL_DEFINE_GUID(CLSID, CLSID_ShellPreviewHandler,0x2DEB1AB8,0xDECA,0x42C2,0xB5,0x76,0x31,0x16,0xD9,0xCA,0x8F,0xA8);


MIDL_DEFINE_GUID(CLSID, CLSID_ShellThumbnailProvider,0x2877CB72,0x1427,0x43B0,0xA2,0xB4,0x9C,0xB1,0xB2,0xBA,0x8C,0x53);


MIDL_DEFINE_GUID(CLSID, CLSID_ShellPropertyStore,0x5CE761D0,0x72AA,0x4484,0xB2,0xBF,0x2A,0x0E,0xBF,0x3A,0xCC,0x0C);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



