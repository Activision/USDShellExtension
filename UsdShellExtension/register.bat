REM This batch needs to be executed as administrator!

@echo off

cd /D %~dp0

set /a success = 1

echo UsdPreviewLocalServer.exe /RegServer
start /wait UsdPreviewLocalServer.exe /RegServer
if %ERRORLEVEL% NEQ 0 (
	echo [91mUsdPreviewLocalServer.exe failed to register![0m
	set /a success = 0
)

echo UsdPythonToolsLocalServer.exe /RegServer
start /wait UsdPythonToolsLocalServer.exe /RegServer
if %ERRORLEVEL% NEQ 0 (
	echo [91mUsdPythonToolsLocalServer.exe failed to register![0m 
	set /a success = 0
)

echo UsdSdkToolsLocalServer.exe /RegServer
start /wait UsdSdkToolsLocalServer.exe /RegServer
if %ERRORLEVEL% NEQ 0 (
	echo [91mUsdSdkToolsLocalServer.exe failed to register![0m 
	set /a success = 0
)

echo regsvr32.exe /n /i UsdShellExtension.dll
regsvr32.exe /n /i UsdShellExtension.dll

if %success% NEQ 1 (
	pause
	exit /b 1
)