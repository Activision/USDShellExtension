!include FileFunc.nsh

;--------------------------------
Var CmdLineUsdPath
Var CmdLineUsdPythonPath
Var CmdLineUsdPxrPluginPathName

Var CmdLinePythonPath
Var CmdLinePythonPythonPath

Var CmdLineRendererPreview
Var CmdLineRendererThumbnail
Var CmdLineRendererView

;--------------------------------
Function ParseCommandLine

  ${GetParameters} $R0
  ClearErrors

  ${GetOptions} $R0 /USD_PATH= $CmdLineUsdPath
  ${GetOptions} $R0 /USD_PYTHONPATH= $CmdLineUsdPythonPath
  ${GetOptions} $R0 /USD_PXRPLUGINPATHNAME= $CmdLineUsdPxrPluginPathName

  ${GetOptions} $R0 /PYTHON_PATH= $CmdLinePythonPath
  ${GetOptions} $R0 /PYTHON_PYTHONPATH= $CmdLinePythonPythonPath

  ${GetOptions} $R0 /RENDERER_PREVIEW= $CmdLineRendererPreview
  ${GetOptions} $R0 /RENDERER_THUMBNAIL= $CmdLineRendererThumbnail
  ${GetOptions} $R0 /RENDERER_VIEW= $CmdLineRendererView

FunctionEnd