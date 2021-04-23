REM This batch needs to be executed as administrator!

@echo off

cd /D %~dp0

set /a success = 1

echo UsdPreviewLocalServer.exe /UnregServer
start /wait UsdPreviewLocalServer.exe /UnregServer
if %ERRORLEVEL% NEQ 0 (
	echo [91mUsdPreviewLocalServer.exe failed to unregister![0m
	set /a success = 0
)

echo UsdPythonToolsLocalServer.exe /UnregServer
start /wait UsdPythonToolsLocalServer.exe /UnregServer
if %ERRORLEVEL% NEQ 0 (
	echo [91mUsdPythonToolsLocalServer.exe failed to unregister![0m 
	set /a success = 0
)

echo UsdSdkToolsLocalServer.exe /UnregServer
start /wait UsdSdkToolsLocalServer.exe /UnregServer
if %ERRORLEVEL% NEQ 0 (
	echo [91mUsdSdkToolsLocalServer.exe failed to unregister![0m 
	set /a success = 0
)

echo regsvr32.exe /u UsdShellExtension.dll
regsvr32.exe /u UsdShellExtension.dll

if %success% NEQ 1 (
	pause
	exit /b 1
)