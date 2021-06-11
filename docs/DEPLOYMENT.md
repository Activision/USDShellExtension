Activision USD Shell Extension - Deployment Guide
=================================================

The USD Shell Extension has been written to be deployed in many development environments.

Deploying using Setup
---------------------
The USD Shell Extension setup can be run in silently and all ini configuration file values can be set on the command line of the setup program using the syntax /SECTION_KEY=VALUE.

| Switch                    | Description                                               |
|-                          |-                                                          |
| /s                        | Runs setup in silent mode                                 |
| /USD_PATH=                | PATH environment variable for USD                         |
| /USD_PYTHONPATH=          | PYTHONPATH environment variable for USD                   |
| /USD_PXR_PLUGINPATH_NAME= | PXR_PLUGINPATH_NAME environment variable for USD          |
| /USD_EDITOR=              | Editor used for the Edit command in Windows Explorer      |
| /RENDERER_PREVIEW=        | Hydra renderer used for the Windows Explorer Preview pane |
| /RENDERER_THUMBNAIL=      | Hydra renderer used for Windows Explorer Thumbnails       |
| /RENDERER_VIEW=           | Hydra renderer used when launching usdview                |
| /PYTHON_PATH=             | Path to the installation of Python                        |
| /PYTHON_PYTHONPATH=       | PYTHONPATH environment variable for Python                |

```
UsdShellExtension-v1.09-u21.02-p3.7-setup.exe /s /USD_PATH="D:\USD-shared-full\bin\;D:\USD-shared-full\lib\" /USD_PYTHONPATH="D:\USD-shared-full\lib\python" /RENDERER_PREVIEW=Embree
```

Deploying using Loose Files
---------------------------

It is possible to deploy the Shell Extension manually.

1. The files installed by setup can be packaged up for deployment without the use of setup.
2. A `UsdShellExtension.ini` configuration file can be placed in the install folder.
3. The github repository contains `register.bat` and `unregister.bat`. The commands in these batch files can be used to install and uninstall the shell extension.

**NOTE**  
Care must be taken to correctly shutdown Windows Explorer and Windows Search when updating the shell extension.

| File                                 |
|-                                     |
| usd\\\*.\*                           |
| boost_pythonXX-vcXXX-mt-x64-1_70.dll |
| LICENSE.txt                          |
| NOTICE.txt                           |
| plugInfo.json                        |
| pythonXX.dll                         |
| tbb.dll                              |
| tbbmalloc.dll                        |
| UsdPreviewHandler.pyd                |
| UsdPreviewLocalServer.exe            |
| UsdPythonToolsLocalServer.exe        |
| UsdSdkToolsLocalServer.exe           |
| UsdShellExtension.dll                |
| usd_ms.dll                           |
| register.bat                         |
| unregister.bat                       |
