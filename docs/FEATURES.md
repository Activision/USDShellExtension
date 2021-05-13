Activision USD Shell Extension
==============================

Table of contents
-----------------

1. [Windows Explorer Thumbnails](#thumbnails)
2. [Windows Explorer Preview](#preview)
3. [Windows Explorer Format Conversion](#conversion)
4. [Windows Explorer Edit](#edit)
5. [Windows Search](#search)
6. [UsdView](#usdview)

<BR>  

Windows Explorer Thumbnails <a name="thumbnails"></a>
---------------------------

Thumbnails are automatically generated for USD file types. Thumbnails are generated by rendering out an image of the USD stage in the background.

Thumbnails are cached in the Windows thumbnail cache. Subsequent visits to a floder will display the cached thumbnail. The cached thumbnail will be invalidated when the file is modified or when the user explicitly selects Refresh Thumbnail from the Windows Explorer context menu.

![thumbnails][thumbnails]

<BR>  

Windows Explorer Preview <a name="preview"></a>
------------------------

The Windows Explorer Preview pane will display a 3D viewport. 

This viewport uses UsdStageView of UsdView to render a 3D viewport in Windows Explorer.

The viewport exposes all installed Hydra renderers. The default Hydra renderer can be modified in the configuration file.

![preview][preview]

<BR>  

Windows Explorer Conversion <a name="conversion"></a>
---------------------------

The Windows Explorer context menu can perform conversions on USD files.

* Crate  
  Convert a USD file type to USDC Crate Format.
* Uncrate  
  Convert a USD file type to USDA ASCII Format.
* Package  
  Convert a USD file type to a USDZ Package.
* Flatten  
  Flatten a USD file hierarchy to a USD file.  

![convert][convert]

<BR>  

Windows Explorer Edit <a name="edit"></a>
---------------------

The Windows Explorer context menu exposes the ability to edit USD files in place. This includes crated USDC files.

The editor application used can be set in the configuration file.

![edit][edit]

<BR>  

Windows Search <a name="search"></a>
--------------

USD file metadata is exposed to Windows Search. When USD metadata is indexed by Windows Search it can be searched for quickly.

| Metadata        | Search Mnemonics                  | Visible in Explorer | Editable in Explorer | Visible in Properties |
| --------------- |-----------------------------------|---------------------|----------------------|-----------------------|
| Comments        | comment, comments                 |                 Yes |                  Yes |                   Yes |
| Documentation   | usddocumentation, usddocs, usddoc |                 Yes |                   No |                   Yes |
| CustomLayerData | usdcustomlayerdata, usdlayerdata  |                  No |                   No |                   Yes |  

<BR>  

![search][search]

<BR>  

UsdView <a name="usdview"></a>
-------
The default action for USD files in Windows Explorer is to open them in UsdView.

![view][view]


[thumbnails]:thumbnails.png "Windows Explorer Thumbnails"
[preview]:preview.png "Windows Explorer Preview"
[convert]:usdcat.png "Windows Explorer Format Conversion"
[edit]:edit.png "Windows Explorer Edit"
[search]:search.png "Windows Search"
[view]:usdview.png "UsdView"