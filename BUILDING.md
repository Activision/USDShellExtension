ACTIVISION USD SHELL EXTENSION
==============================


## Dependencies

* Boost
* Python
* USD SDK
* Windows 10 SDK
* Visual Studio
  * ATL
  * Python C++ Tools

#### Boost

`boost.props`

Boost is required for Boost.Python. It is used to build a python extension module. The boost 
compilation must much the version of MSVC and python used by the shell extension.

#### Python

`python.props`

Currenmtly only Python 3.4 or higher is supported by the shell extension. This is currently due to 
UsdThumbnail.py requiring importlib spec.loader.exec_module.

#### USD SDK

`usd.props`

The shell extension requires a bare-bones monolithic build of the USD SDK. The monolithic build 
is a single DLL which makes it easier to isolate it into its own activation context in Windows Explorer.

The shell extension is currently set up to build against a build of USD that does not use Python. 
The shell extension is injected into Windows Explorer which is also the process that runs 
the Windows Desktop. We want to keep as much out of that process as we can.

```
build_scripts\build_usd.py d:\USD-monolithic-bare --build-monolithic --no-tests --no-examples --no-tutorials --no-tools --no-docs --no-python --no-imaging --no-ptex --no-openvdb --no-usdview --no-embree --no-prman --no-openimageio --no-opencolorio --no-alembic --no-hdf5 --no-draco --no-materialx
```

#### Windows 10 SDK

Windows 10 SDK 10.0.14393.0 or higher is required to build the shell extension. This requirement is only 
due to High DPI support. It would be possible to remove High DPI support and compile the shell extension 
for Windows 7.

#### Visual Studio

Visual Studio is the only supported build tool at this time. 

The following Visual Studio components are required:

- C++ ATL
- Python Language Support


## Simple Build

The simplest way to build the shell extension is to build two flavors of the USD SDK. The first being a 
bare-bones version of the SDK that is USD by Windows Explorer. The second is a full version used by 
everything else.

Bare-Bones USD SDK

```
build_scripts\build_usd.py d:\USD-monolithic-bare --build-monolithic --no-tests --no-examples --no-tutorials --no-tools --no-docs --no-python --no-imaging --no-ptex --no-openvdb --no-usdview --no-embree --no-prman --no-openimageio --no-opencolorio --no-alembic --no-hdf5 --no-draco --no-materialx
```

Full USD SDK

```
build_scripts\build_usd.py d:\USD-shared-full [--embree] [--materialx]
```

Then update the property sheets to point to these builds.

`usd.props`

Point this property sheet to the bare-bones USD SDK.

`boost.props`

Point this property sheet to the full USD SDK's version of boost.

`UsdShellExtension.cfg`

Point the paths in this configuration file to the full USD SDK.