Activision USD Shell Extension
==============================

A Windows Shell Extension for the Pixar USD file format. 

**Windows Explorer Features**

* Hydra Realtime Preview 
* Thumbnails
* Context Menus
* Metadata Properties

**Windows Search Features**

* Metadata Search

More details available [here](./docs/FEATURES.md).

Goals
-----

There are several goals that this shell extension tries to adhere to and takes extra precautions to meet.

* **Performance**  
  It is important that the shell extension not stall Windows Explorer.  

* **Stability**  
  Instability in the shell extension will take down Window Explorer and/or the entire desktop.  

The mitigation strategy is to run the majority of the shell extension asynchronously out of the 
Windows Explorer process.


Dependencies
------------

The following dependencies are required:

* [Microsoft Visual Studio (2017 or newer)](https://visualstudio.microsoft.com/vs/)
* [Universal Scene Description (USD)](https://graphics.pixar.com/usd/docs/index.html)
* [Python (2.7, 3.6, 3.7)](https://www.python.org/)

Required Microsoft Visual Studio Components:

* Desktop development with C++
* C++ ATL
* Windows 10 SDK
* Windows Universal C Runtime
* Python language support


