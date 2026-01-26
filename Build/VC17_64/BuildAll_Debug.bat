@echo on
setlocal enabledelayedexpansion

call "%VC_SCRIPT_64%"
if errorlevel 1 exit /b 1

rem ===== Определяем корень =====
if not defined BUILD_DIR set "BUILD_DIR=%~dp0../.."
for %%I in ("%BUILD_DIR%") do set "BUILD_DIR=%%~fI"
echo BUILD_DIR=%BUILD_DIR%

rem ===== ACF =====
pushd "%BUILD_DIR%/3rdParty/Acf/Build/CMake"
if not exist build-debug mkdir build-debug
cd build-debug
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug ..
if errorlevel 1 exit /b 1
cmake --build .
if errorlevel 1 exit /b 1
popd

rem ===== ACFSLN =====
pushd "%BUILD_DIR%/3rdParty/AcfSln/Build/CMake"
if not exist build-debug mkdir build-debug
cd build-debug
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug ..
if errorlevel 1 exit /b 1
cmake --build .
if errorlevel 1 exit /b 1
popd

rem ===== IMTCORE (TOOLS) =====
pushd "%BUILD_DIR%/3rdParty/ImtCore/Build/CMake"
if not exist build-debug-tools mkdir build-debug-tools
cd build-debug-tools
cmake -GNinja -DIMT_BUILD_ONLYTOOLS:BOOL=ON -DCMAKE_BUILD_TYPE=Debug ..
if errorlevel 1 exit /b 1
cmake --build .
if errorlevel 1 exit /b 1
popd

rem ===== IMTCORE (FULL) =====
pushd "%BUILD_DIR%/3rdParty/ImtCore/Build/CMake"
if not exist build-debug-full mkdir build-debug-full
cd build-debug-full
cmake -GNinja -DIMT_BUILD_ONLYTOOLS:BOOL=OFF -DCMAKE_BUILD_TYPE=Debug -DBUILDDIR="%BUILD_DIR%/3rdParty" ..
if errorlevel 1 exit /b 1
cmake --build .
if errorlevel 1 exit /b 1
popd

rem ===== PUMA =====
pushd "%BUILD_DIR%/3rdParty/Puma/Build/CMake"
if not exist build-debug mkdir build-debug
cd build-debug
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DBUILDDIR="%BUILD_DIR%/3rdParty" ..
if errorlevel 1 exit /b 1
cmake --build .
if errorlevel 1 exit /b 1
popd

rem ===== LISA =====
rem pushd "%BUILD_DIR%/3rdParty/Lisa/Build/CMake"
rem if not exist build-debug mkdir build-debug
rem cd build-debug
rem cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DBUILDDIR="%BUILD_DIR%/3rdParty" ..
rem if errorlevel 1 exit /b 1
rem cmake --build .
rem if errorlevel 1 exit /b 1
rem popd

rem ===== PROLIFE =====
pushd "%BUILD_DIR%/Build/CMake"
if not exist build-debug mkdir build-debug
cd build-debug
rem -DBUILDDIR - for correct imtcore build
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DBUILDDIR="%BUILD_DIR%/3rdParty" ..
if errorlevel 1 exit /b 1
cmake --build .
if errorlevel 1 exit /b 1
popd

endlocal
