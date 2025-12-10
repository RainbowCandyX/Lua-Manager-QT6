@echo off
REM Build LuaJIT static library for Windows
REM This script requires Visual Studio (MSVC) to be installed

echo ========================================
echo Building LuaJIT Static Library
echo ========================================
echo.

REM Save current directory
set "ORIGINAL_DIR=%CD%"

REM Check if LuaJIT source directory exists
if not exist "LuaJIT-2.1\src" (
    echo ERROR: LuaJIT-2.1\src directory not found!
    echo Please ensure LuaJIT source code is in the correct location.
    pause
    exit /b 1
)

REM Try to find Visual Studio installation
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if exist "%VSWHERE%" (
    echo Detecting Visual Studio installation...
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "VS_PATH=%%i"
    )
)

REM Check if we found Visual Studio
if defined VS_PATH (
    echo Found Visual Studio at: %VS_PATH%
    echo.

    REM Setup MSVC environment for x64
    echo Setting up MSVC environment...
    call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"
    if errorlevel 1 (
        echo ERROR: Failed to setup MSVC environment
        pause
        exit /b 1
    )
) else (
    echo WARNING: Could not auto-detect Visual Studio installation
    echo Please ensure you are running this script from:
    echo - x64 Native Tools Command Prompt for VS
    echo or that Visual Studio 2017 or later is installed
    echo.
    echo Attempting to continue anyway...
    echo.
)

REM Navigate to LuaJIT source directory
cd /d "%~dp0LuaJIT-2.1\src"

echo.
echo Building LuaJIT with multi-threaded compilation (/MP enabled)...
echo Current directory: %CD%
echo.

REM Run the build script
call msvcbuild.bat static

if errorlevel 1 (
    echo.
    echo ========================================
    echo ERROR: Build failed!
    echo ========================================
    cd /d "%ORIGINAL_DIR%"
    pause
    exit /b 1
)

REM Check if the library was created
if exist "lua51.lib" (
    echo.
    echo ========================================
    echo SUCCESS: Build completed!
    echo ========================================
    echo Library location: %CD%\lua51.lib
    echo.
) else (
    echo.
    echo ========================================
    echo WARNING: Build completed but lua51.lib not found
    echo ========================================
    echo.
)

REM Return to original directory
cd /d "%ORIGINAL_DIR%"

echo Press any key to exit...
pause >nul
