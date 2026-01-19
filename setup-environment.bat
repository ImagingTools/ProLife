@echo off
REM setup-environment.bat
REM This script initializes the ProLife build environment by setting up submodules
REM and configuring environment variables.

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

REM Initialize submodules
echo Initializing Git submodules...
echo This may take a few minutes on first run.
echo.

git submodule update --init --recursive
if %errorlevel% neq 0 (
    echo X Failed to initialize submodules
    echo.
    echo Some submodules may be private and require authentication.
    echo Please ensure you have:
    echo   - SSH keys configured and added to GitHub
    echo   - Or Personal Access Token with appropriate permissions
    echo   - Repository access granted by the ImagingTools organization
    echo.
    echo You can configure Git to use SSH URLs globally:
    echo   git config --global url."git@github.com:".insteadOf "https://github.com/"
    echo.
    pause
    exit /b 1
)

echo √ Submodules initialized successfully
echo.

REM Show submodule status
echo Submodule status:
git submodule status
echo.

REM Set environment variables pointing to submodules
set "IMTCOREDIR=%PROJECT_ROOT%\3rdParty\ImtCore"
set "PUMADIR=%PROJECT_ROOT%\3rdParty\Puma"
set "LISADIR=%PROJECT_ROOT%\3rdParty\Lisa"
set "ACFDIR=%PROJECT_ROOT%\3rdParty\Acf"
set "ACFSLNDIR=%PROJECT_ROOT%\3rdParty\AcfSln"
set "AGENTINODIR=%PROJECT_ROOT%\3rdParty\Agentino"
set "PROLIFEDIR=%PROJECT_ROOT%"

echo Environment variables configured:
echo   IMTCOREDIR=%IMTCOREDIR%
echo   PUMADIR=%PUMADIR%
echo   LISADIR=%LISADIR%
echo   ACFDIR=%ACFDIR%
echo   ACFSLNDIR=%ACFSLNDIR%
echo   AGENTINODIR=%AGENTINODIR%
echo   PROLIFEDIR=%PROLIFEDIR%
echo.

echo ==========================================
echo Setup complete!
echo ==========================================
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
