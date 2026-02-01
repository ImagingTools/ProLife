@echo off
REM setup-environment.bat
REM This script initializes the ProLife build environment by configuring environment variables.
REM Note: Dependencies must be cloned and built separately. See BUILDING.md for details.

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR:~0,-1%"

echo ==========================================
echo ProLife Environment Setup
echo ==========================================
echo.

REM Check Git version
git --version
echo.

REM Check if required dependencies are available
echo Checking for required dependencies...
echo Note: All dependencies must be cloned and available externally.
echo.

REM Check environment variables
if "%IMTCOREDIR%"=="" (
    echo WARNING: IMTCOREDIR is not set
)

if "%PUMADIR%"=="" (
    echo WARNING: PUMADIR is not set
)

if "%LISADIR%"=="" (
    echo WARNING: LISADIR is not set
)

if "%ACFDIR%"=="" (
    echo WARNING: ACFDIR is not set
)

if "%ACFSLNDIR%"=="" (
    echo WARNING: ACFSLNDIR is not set
)

if "%AGENTINODIR%"=="" (
    echo WARNING: AGENTINODIR is not set
)

REM Set PROLIFEDIR
set "PROLIFEDIR=%PROJECT_ROOT%"

echo.
echo Environment variables configured:
if defined IMTCOREDIR (echo   IMTCOREDIR=%IMTCOREDIR%) else (echo   IMTCOREDIR=NOT SET)
if defined PUMADIR (echo   PUMADIR=%PUMADIR%) else (echo   PUMADIR=NOT SET)
if defined LISADIR (echo   LISADIR=%LISADIR%) else (echo   LISADIR=NOT SET)
if defined ACFDIR (echo   ACFDIR=%ACFDIR%) else (echo   ACFDIR=NOT SET)
if defined ACFSLNDIR (echo   ACFSLNDIR=%ACFSLNDIR%) else (echo   ACFSLNDIR=NOT SET)
if defined AGENTINODIR (echo   AGENTINODIR=%AGENTINODIR%) else (echo   AGENTINODIR=NOT SET)
echo   PROLIFEDIR=%PROLIFEDIR%
echo.

echo ==========================================
echo Setup complete!
echo ==========================================
echo.
echo IMPORTANT: Before building, ensure all dependency directories are set:
echo   set IMTCOREDIR=C:\path\to\ImtCore
echo   set PUMADIR=C:\path\to\Puma
echo   set LISADIR=C:\path\to\Lisa
echo   set ACFDIR=C:\path\to\Acf
echo   set ACFSLNDIR=C:\path\to\AcfSln
echo   set AGENTINODIR=C:\path\to\Agentino
echo.
echo To build ProLife with CMake:
echo   cd Build\CMake
echo   mkdir build ^&^& cd build
echo   cmake ..
echo   cmake --build . --config Release
echo.
echo To build with Visual Studio:
echo   cd Build\VC17_64
echo   [Use Visual Studio solution or build scripts]
echo.
echo To persist environment variables for this session:
echo   call setup-environment.bat
echo.
echo To persist environment variables system-wide:
echo   Use System Properties -^> Environment Variables
echo.

pause
