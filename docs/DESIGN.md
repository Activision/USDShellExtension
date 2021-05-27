Activision USD Shell Extension - Design
=======================================

Fundamentals 
------------

There are several fundamentals that this shell extension tries to adhere to.

* **Performance**  
  The extension must not stall Windows Explorer.  

* **Stability**  
  The extension must be stable. A crash will take down Window Explorer and/or the entire desktop.  
  

UsdShellExtension
-----------------

This is a DLL project. 

It is the core Windows Explorer Shell extension. This DLL is loaded into 
the Windows Explorer process. As a result, this DLL should not *host* python.


| Extension             | Description                                                                                                   |
|-                      |-                                                                                                              |
| **IPreviewHandler**   | Adds a preview for USD files by running `usdStageView` (a part of `usdView`) in an out-of-process viewport. |
| **IThumbnailProvider**| Generates thumbnails for USD files by running `usdRecord` out-of-process.                                    |
| **IPropertyStore**    | Exposes USD metadata to Windows Explorer and Windows Search.                                                  |


**IPreviewHandler**

The IPreviewHandler runs out-of-process in a Windows provided COM local server called `prevhost.exe`. 
We run the USD loading screen in `prevhost.exe` but launch the actual preview using our own out of process COM server, `UsdPreviewLocalServer.exe`. 

The `prevhost.exe` process hosts all Windows preview handler in a single process instance. We use `UsdPreviewLocalServer.exe` to isolate `prevhost.exe`.

*UsdPreviewLocalServer.exe*

This COM server embeds python and launches an instance of `usdStageView` using a custom python script. 
It isolates python from any Windows system process.

Stalling a COM call in our implementation of IPreviewHandler would still hang Windows Explorer, even though the preview is running out-of-process in `prevhost.exe`. By having our own asynchronous out-of-process server, we could operate asynchronously to Windows Explorer.

*UsdPreviewHandlerPython.py*

When launching `usdStageView` from `UsdPreviewLocalServer.exe` we use a custom script that uses `UsdPreviewHandlerPython.py` to send and receive events from C++. The first event that is sent is the HWND of the `usdStageView` so that it can be made a child of the preview HWND in Windows Explorer.

**IThumbnailProvider**

*UsdPythonToolsLocalServer.exe*

Thumbnail providers already run on background threads in Windows Explorer. The extraction of a thumbnail can take a significant amount of time. We launch an out-of-process COM server, `UsdPythonToolsLocalServer.exe`, to run the `usdRecord` python script provided by USD SDK. This keeps python out of Windows Explorer.

**IPropertyStore**

The IPropertyStore interface runs synchronously to the UI in Windows Explorer. It must execute quickly. For this reason 
the C++ USD SDK is used directly in Windows Explorer. 

**USD Activation Context**

The USD SDK and all of its dependencies are isolated from the rest of Windows Explorer via a Windows Activation Context. 
This at least guarantees that if any other extension ever loads USD or any of its dependencies into Windows Explorer, we will not clash.

**Python USD Plug-in**
This DLL serves as a python plug-in. The python plug-in is called ArResolverShellExtension. It is used to 
allow shared reads across the USD SDK. By default, the USD SDK uses fopen which in MSVC does not allow shared reads. 

**rundll**
This DLL has several rundll entry points. These entry points are executed from Windows Explorer context menus. 
They are all located in `ShellExecute.cpp`.

UsdPreviewLocalServer
---------------------

This is COM Local Server executable. 

This project serves `usdStageView`, the USD preview window, out of process. `usdStageView` is a python script 
written by Pixar. This project hosts python and executes the script. It communicates with `UsdShellExtension` 
via COM.

If the Preview server crashes. It will not take down Windows Explorer.

UsdPreviewHandlerPython
-----------------------

This is a python extension module.

It is used to communicate between the python preview window and our shell 
extension. It is hosted by `UsdPreviewLocalServer`.

UsdPythonToolsLocalServer
-------------------------

This is COM Local Server executable. 

It is used to host out of process USD python tools. This server is used to capture thumbnails using 
`usdrecord` and to launch `usdview`.

UsdSdkToolsLocalServer
----------------------

This is COM Local Server executable. 

It is used to host out of process USD C++ tools. This server executes the conversion between USD file 
formats and generating USD archives.
