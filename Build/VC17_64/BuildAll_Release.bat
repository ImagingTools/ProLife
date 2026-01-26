@echo on
setlocal enabledelayedexpansion

call "%VC_SCRIPT_64%"
if errorlevel 1 exit /b 1

rem ===== Определяем корень =====
if not defined BUILD_DIR set "BUILD_DIR=%~dp0../.."
for %%I in ("%BUILD_DIR%") do set "BUILD_DIR=%%~fI"
echo BUILD_DIR=%BUILD_DIR%

rem ===== Check Environment Variables =====
echo.
echo Checking required environment variables...
if not defined IMTCOREDIR (
    echo ERROR: IMTCOREDIR is not set
    exit /b 1
)
if not defined PUMADIR (
    echo ERROR: PUMADIR is not set
    exit /b 1
)
if not defined LISADIR (
    echo ERROR: LISADIR is not set
    exit /b 1
)
if not defined ACFDIR (
    echo ERROR: ACFDIR is not set
    exit /b 1
)
if not defined ACFSLNDIR (
    echo ERROR: ACFSLNDIR is not set
    exit /b 1
)
if not defined AGENTINODIR (
    echo ERROR: AGENTINODIR is not set
    exit /b 1
)

echo Environment variables are set:
echo   IMTCOREDIR=%IMTCOREDIR%
echo   PUMADIR=%PUMADIR%
echo   LISADIR=%LISADIR%
echo   ACFDIR=%ACFDIR%
echo   ACFSLNDIR=%ACFSLNDIR%
echo   AGENTINODIR=%AGENTINODIR%
echo.

rem ===== NOTE =====
rem This script builds ProLife only.
rem Dependencies (Acf, AcfSln, ImtCore, Puma, Lisa, Agentino) must be built separately.
rem Ensure they are available at the paths specified by environment variables.

rem ===== PROLIFE =====
pushd "%BUILD_DIR%/Build/CMake"
if not exist build-release mkdir build-release
cd build-release
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
if errorlevel 1 exit /b 1
cmake --build .
if errorlevel 1 exit /b 1
popd

endlocal
