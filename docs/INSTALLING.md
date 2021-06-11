Activision USD Shell Extension - Installation Guide
===================================================

This document covers installing the shell extension from precompiled shell extension setup installers.

Step 1: USD Build
-----------------

A prerequisite to the installing the shell extension is to have a functional build of USD. 

You can build USD locally from Pixar's GitHub or use per-built libraries such as the ones available at NVIDIA.

When building locally, make sure that the USD Python tools are built as well.

Pixar's USD Source Repository  
https://github.com/PixarAnimationStudios/USD/

NVIDIA's USD Pre-built Libraries and Tools  
https://developer.nvidia.com/usd


Step 2: Verify USD install with usdview
---------------------------------------

The shell extension uses usdview for the Windows Explorer preview and usdrecord for thumbnails. usdview must be operational outside of the extension.

Launch usdview manually out of the USD build and make sure that it is rendering correctly.


Step 3: Shell Extension Setup
-----------------------------

Match the version of Python used by the USD build with that of the shell extension setup. NVIDIA's pre-built libraries currently use Python 3.6.

The shell extension setup contains a USD Libraries and Tools screen. Use the `Set using root USD folder` button to select the location of the USD build from Step 1.

**NVIDIA Pre-built Libraries Example**

NVIDIA pre-built libraries installation location: `D:\usd-21-05-usd-win64_py36_release`

| Entry                    | Value                                                                             |
|-                         |-                                                                                  |
| **PATH**                 | D:\usd-21-05-usd-win64_py36_release\bin\;D:\usd-21-05-usd-win64_py36_release\lib\ |
| **PYTHONPATH**           | D:\usd-21-05-usd-win64_py36_release\lib\python                                    |
| **PXR_PLUGINPATH_NAME**  |                                                                                   |


USD Text Editor
---------------
By default, the text editor used by the `Edit` command is `notepad.exe`. This text editor is guaranteed to be installed.

A custom text editor can be specified in the `UsdShellExtension.ini` file. 

**Requirements**

The editor *must* support not returning on the command line until the text file is closed in the editor.


**Visual Studio Code**

Visual Studio Code is a great editor for USDA files that also can perform syntax highlighting.

To associate Visual Studio Code with the USD Shell Extension use the following command in the ini file:

```
[USD]
EDITOR="C:\Users\pmintus\AppData\Local\Programs\Microsoft VS Code\Code.exe" --wait
```


Troubleshooting
---------------

**Python**

By default, the shell extension will try to automatically locate a Python install. The Python 3.x installers from python.org will install registry keys that can be used to locate Python.

| Python Version | Search Location                                              |
|-               |-                                                             |
| 2.7            | C:\Python27                                                  |
| 3.6            | HKEY_CURRENT_USER\SOFTWARE\Python\PythonCore\3.6\InstallPath |
| 3.7            | HKEY_CURRENT_USER\SOFTWARE\Python\PythonCore\3.7\InstallPath |

If the Python install is not found at these locations, then it needs to be set explicitly using the `UsdShellExtension.ini` file.

```
[PYTHON]
PATH=C:\Python37
```

**Event Viewer**

If the shell extension is not working, then the first place to check for issues is the Windows Event Viewer. 

Windows Logs | Application

The event source will be `Activision USD Shell Extension`.

If you are seeing Python errors, then make sure that you can run usdview outside of the shell extension (Step 2). Also make sure that the Python version of the shell extension matches that of the USD build.

![eventviewer][eventviewer]


**File Associations**

For the shell extension to work it needs to be associated with USD, USDA, USDC, and USDZ file extensions. Setup performs this association. However, manual user associations take precedence over system global associations. If these file extensions have been previously manually associated with a different application, then the shell extension will not work.

Make sure that the following file type associations are set to `Activision USD Shell Extension`.

This can be changed manually using Windows Settings. 

Windows Settings | Apps | Default apps | Choose default apps by file type

| Extension | App                            |
|-          |-                               |
| .usd      | Activision USD Shell Extension |
| .usda     | Activision USD Shell Extension |
| .usdc     | Activision USD Shell Extension |
| .usdz     | Activision USD Shell Extension |

[eventviewer]:eventviewer.png "Event Viewer"
[filetypes]:filetypes.png "File Type Settings"