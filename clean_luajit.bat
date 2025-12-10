@echo off
REM Clean LuaJIT build artifacts
REM This script removes intermediate and output files generated during LuaJIT build

echo ========================================
echo Cleaning LuaJIT Build Artifacts
echo ========================================
echo.

REM Save current directory
set "ORIGINAL_DIR=%CD%"

REM Check if LuaJIT source directory exists
if not exist "LuaJIT-2.1\src" (
    echo ERROR: LuaJIT-2.1\src directory not found!
    pause
    exit /b 1
)

REM Navigate to LuaJIT source directory
cd /d "%~dp0LuaJIT-2.1\src"

echo Current directory: %CD%
echo.
echo Cleaning ALL build files...
echo.

REM Delete all compiled files

REM Object files
if exist *.obj (
    del /F /Q *.obj
    echo Deleted: *.obj files
)

REM Library files
if exist *.lib (
    del /F /Q *.lib
    echo Deleted: *.lib files
)

REM Manifest files
if exist *.manifest (
    del /F /Q *.manifest
    echo Deleted: *.manifest files
)

REM Debug files
if exist *.pdb (
    del /F /Q *.pdb
    echo Deleted: *.pdb files
)
if exist *.ilk (
    del /F /Q *.ilk
    echo Deleted: *.ilk files
)

REM Export files
if exist *.exp (
    del /F /Q *.exp
    echo Deleted: *.exp files
)

REM Executable files
if exist *.exe (
    del /F /Q *.exe
    echo Deleted: *.exe files
)

REM DLL files
if exist *.dll (
    del /F /Q *.dll
    echo Deleted: *.dll files
)

REM Generated header files
if exist host\buildvm_arch.h (
    del /F /Q host\buildvm_arch.h
    echo Deleted: host\buildvm_arch.h
)
if exist lj_bcdef.h (
    del /F /Q lj_bcdef.h
    echo Deleted: lj_bcdef.h
)
if exist lj_ffdef.h (
    del /F /Q lj_ffdef.h
    echo Deleted: lj_ffdef.h
)
if exist lj_libdef.h (
    del /F /Q lj_libdef.h
    echo Deleted: lj_libdef.h
)
if exist lj_recdef.h (
    del /F /Q lj_recdef.h
    echo Deleted: lj_recdef.h
)
if exist lj_folddef.h (
    del /F /Q lj_folddef.h
    echo Deleted: lj_folddef.h
)

REM Generated Lua file
if exist jit\vmdef.lua (
    del /F /Q jit\vmdef.lua
    echo Deleted: jit\vmdef.lua
)

REM Version file
if exist luajit_relver.txt (
    del /F /Q luajit_relver.txt
    echo Deleted: luajit_relver.txt
)

echo.
echo ========================================
echo Cleaning completed!
echo ========================================
echo.

REM Return to original directory
cd /d "%ORIGINAL_DIR%"

echo Press any key to exit...
pause >nul
