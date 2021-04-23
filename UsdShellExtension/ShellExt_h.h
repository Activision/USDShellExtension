

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __ShellExt_h_h__
#define __ShellExt_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __UsdContextMenu_FWD_DEFINED__
#define __UsdContextMenu_FWD_DEFINED__

#ifdef __cplusplus
typedef class UsdContextMenu UsdContextMenu;
#else
typedef struct UsdContextMenu UsdContextMenu;
#endif /* __cplusplus */

#endif 	/* __UsdContextMenu_FWD_DEFINED__ */


#ifndef __ShellPreviewHandler_FWD_DEFINED__
#define __ShellPreviewHandler_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellPreviewHandler ShellPreviewHandler;
#else
typedef struct ShellPreviewHandler ShellPreviewHandler;
#endif /* __cplusplus */

#endif 	/* __ShellPreviewHandler_FWD_DEFINED__ */


#ifndef __ShellThumbnailProvider_FWD_DEFINED__
#define __ShellThumbnailProvider_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellThumbnailProvider ShellThumbnailProvider;
#else
typedef struct ShellThumbnailProvider ShellThumbnailProvider;
#endif /* __cplusplus */

#endif 	/* __ShellThumbnailProvider_FWD_DEFINED__ */


#ifndef __ShellPropertyStore_FWD_DEFINED__
#define __ShellPropertyStore_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellPropertyStore ShellPropertyStore;
#else
typedef struct ShellPropertyStore ShellPropertyStore;
#endif /* __cplusplus */

#endif 	/* __ShellPropertyStore_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "ShObjIdl_core.h"
#include "thumbcache.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __UsdShellExtensionLib_LIBRARY_DEFINED__
#define __UsdShellExtensionLib_LIBRARY_DEFINED__

/* library UsdShellExtensionLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_UsdShellExtensionLib;

EXTERN_C const CLSID CLSID_UsdContextMenu;

#ifdef __cplusplus

class DECLSPEC_UUID("0BCB74E0-5B61-4F24-BAB3-96CC76FE3672")
UsdContextMenu;
#endif

EXTERN_C const CLSID CLSID_ShellPreviewHandler;

#ifdef __cplusplus

class DECLSPEC_UUID("2DEB1AB8-DECA-42C2-B576-3116D9CA8FA8")
ShellPreviewHandler;
#endif

EXTERN_C const CLSID CLSID_ShellThumbnailProvider;

#ifdef __cplusplus

class DECLSPEC_UUID("2877CB72-1427-43B0-A2B4-9CB1B2BA8C53")
ShellThumbnailProvider;
#endif

EXTERN_C const CLSID CLSID_ShellPropertyStore;

#ifdef __cplusplus

class DECLSPEC_UUID("5CE761D0-72AA-4484-B2BF-2A0EBF3ACC0C")
ShellPropertyStore;
#endif
#endif /* __UsdShellExtensionLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


