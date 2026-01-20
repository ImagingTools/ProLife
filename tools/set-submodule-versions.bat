@echo off
REM ProLife Submodule Version Management Script for Windows
REM Interactive script to set submodule versions

setlocal enabledelayedexpansion

REM Script directory
set "SCRIPT_DIR=%~dp0"
set "REPO_ROOT=%SCRIPT_DIR%.."
cd /d "%REPO_ROOT%"

echo ========================================
echo ProLife Submodule Version Management
echo ========================================
echo.

REM Check if we're in a git repository
git rev-parse --is-inside-work-tree >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Not in a git repository
    exit /b 1
)

REM Initialize submodules if needed
echo Ensuring submodules are initialized...
git submodule update --init --recursive >nul 2>&1
echo.

REM Count submodules
set "SUBMODULE_COUNT=0"
for /f "tokens=1,2" %%a in ('git submodule status') do (
    set /a SUBMODULE_COUNT+=1
)

echo Found %SUBMODULE_COUNT% submodules
echo.
echo Select an option:
echo   1. Auto-update all submodules to latest tagged versions
echo   2. Interactively select versions for each submodule
echo   3. Set specific version for one submodule
echo   4. Show current submodule versions
echo   5. Exit
echo.

set /p CHOICE="Enter your choice (1-5): "

if "%CHOICE%"=="1" goto :auto_update_all
if "%CHOICE%"=="2" goto :interactive_update
if "%CHOICE%"=="3" goto :single_update
if "%CHOICE%"=="4" goto :show_versions
if "%CHOICE%"=="5" goto :exit_script

echo [ERROR] Invalid choice
goto :exit_script

:auto_update_all
echo.
echo ========================================
echo Auto-Updating All Submodules
echo ========================================
echo.

set "UPDATED_COUNT=0"

for /f "tokens=1,2" %%a in ('git submodule status') do (
    set "COMMIT=%%a"
    set "PATH=%%b"
    set "COMMIT=!COMMIT:~1!"
    
    if exist "!PATH!" (
        echo Processing !PATH!...
        pushd "!PATH!"
        
        REM Fetch latest tags
        git fetch --tags --quiet 2>nul
        
        REM Get latest semantic version tag
        set "LATEST_TAG="
        for /f "delims=" %%t in ('git tag -l 2^>nul ^| findstr /r "^v*[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*$"') do (
            set "LATEST_TAG=%%t"
        )
        
        if not "!LATEST_TAG!"=="" (
            REM Get current tag if on a tag
            set "CURRENT_TAG="
            for /f "delims=" %%c in ('git describe --tags --exact-match 2^>nul') do set "CURRENT_TAG=%%c"
            
            if "!CURRENT_TAG!"=="!LATEST_TAG!" (
                echo   [INFO] Already on latest tag: !LATEST_TAG!
            ) else (
                echo   [INFO] Updating from !CURRENT_TAG! to !LATEST_TAG!
                git checkout "!LATEST_TAG!" --quiet 2>nul
                if not errorlevel 1 (
                    set /a UPDATED_COUNT+=1
                    popd
                    git add "!PATH!"
                    pushd "!PATH!"
                    echo   [OK] Updated to !LATEST_TAG!
                ) else (
                    echo   [ERROR] Failed to checkout !LATEST_TAG!
                )
            )
        ) else (
            echo   [WARNING] No semantic version tags found
        )
        
        popd
        echo.
    )
)

echo ========================================
echo Summary
echo ========================================
echo Updated %UPDATED_COUNT% submodule(s)
echo.
echo Review changes with: git status
echo Commit changes with: git commit -m "chore(deps): update submodules"
echo.
goto :exit_script

:interactive_update
echo.
echo ========================================
echo Interactive Submodule Update
echo ========================================
echo.

for /f "tokens=1,2" %%a in ('git submodule status') do (
    set "COMMIT=%%a"
    set "PATH=%%b"
    set "COMMIT=!COMMIT:~1,7!"
    set "NAME=%%b"
    
    REM Extract just the last part of the path
    for %%f in ("!NAME!") do set "NAME=%%~nxf"
    
    if exist "!PATH!" (
        echo.
        echo ----------------------------------------
        echo Submodule: !NAME!
        echo Path: !PATH!
        echo Current commit: !COMMIT!
        echo.
        
        pushd "!PATH!"
        
        REM Get current tag/branch
        for /f "delims=" %%t in ('git describe --tags --exact-match 2^>nul') do (
            echo Current tag: %%t
        )
        
        REM Fetch tags
        echo Fetching tags...
        git fetch --tags --quiet 2>nul
        
        echo.
        echo Available tags (last 10):
        git tag -l | findstr /r "^v*[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*$" 2>nul
        echo.
        
        set /p VERSION="Enter version to use (or 'skip' to skip, 'latest' for latest tag): "
        
        if /i "!VERSION!"=="skip" (
            echo [INFO] Skipping !NAME!
            popd
        ) else if /i "!VERSION!"=="latest" (
            REM Get latest tag
            set "LATEST="
            for /f "delims=" %%t in ('git tag -l ^| findstr /r "^v*[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*$"') do (
                set "LATEST=%%t"
            )
            if not "!LATEST!"=="" (
                echo [INFO] Checking out !LATEST!...
                git checkout "!LATEST!" --quiet 2>nul
                if not errorlevel 1 (
                    echo [OK] Updated to !LATEST!
                    popd
                    git add "!PATH!"
                ) else (
                    echo [ERROR] Failed to checkout !LATEST!
                    popd
                )
            ) else (
                echo [ERROR] No tags found
                popd
            )
        ) else (
            echo [INFO] Checking out !VERSION!...
            git checkout "!VERSION!" --quiet 2>nul
            if not errorlevel 1 (
                echo [OK] Updated to !VERSION!
                popd
                git add "!PATH!"
            ) else (
                echo [ERROR] Failed to checkout !VERSION!
                popd
            )
        )
    )
)

echo.
echo ========================================
echo Interactive Update Complete
echo ========================================
echo.
echo Review changes with: git status
echo Commit changes with: git commit -m "chore(deps): update submodules"
echo.
goto :exit_script

:single_update
echo.
echo ========================================
echo Update Single Submodule
echo ========================================
echo.

echo Available submodules:
echo.
set "INDEX=0"
for /f "tokens=2" %%p in ('git submodule status') do (
    set /a INDEX+=1
    echo   !INDEX!. %%p
    set "SUBMOD_!INDEX!=%%p"
)
echo.

set /p SUBMOD_INDEX="Enter submodule number: "

if not defined SUBMOD_%SUBMOD_INDEX% (
    echo [ERROR] Invalid submodule number
    goto :exit_script
)

set "SUBMOD_PATH=!SUBMOD_%SUBMOD_INDEX%!"

echo.
echo Selected: !SUBMOD_PATH!
echo.

if not exist "!SUBMOD_PATH!" (
    echo [ERROR] Submodule not found: !SUBMOD_PATH!
    goto :exit_script
)

pushd "!SUBMOD_PATH!"

echo Fetching tags...
git fetch --tags --quiet 2>nul

echo.
echo Available tags (last 20):
git tag -l | findstr /r "^v*[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*$" 2>nul
echo.

set /p VERSION="Enter version to checkout: "

if "!VERSION!"=="" (
    echo [ERROR] No version specified
    popd
    goto :exit_script
)

echo [INFO] Checking out !VERSION!...
git checkout "!VERSION!" --quiet 2>nul
if not errorlevel 1 (
    echo [OK] Updated !SUBMOD_PATH! to !VERSION!
    popd
    git add "!SUBMOD_PATH!"
    echo.
    echo Review changes with: git status
    echo Commit changes with: git commit -m "chore(deps): update !SUBMOD_PATH! to !VERSION!"
) else (
    echo [ERROR] Failed to checkout !VERSION!
    popd
)

echo.
goto :exit_script

:show_versions
echo.
echo ========================================
echo Current Submodule Versions
echo ========================================
echo.

for /f "tokens=1,2" %%a in ('git submodule status') do (
    set "COMMIT=%%a"
    set "PATH=%%b"
    set "COMMIT=!COMMIT:~1,7!"
    set "NAME=%%b"
    
    REM Extract just the last part of the path
    for %%f in ("!NAME!") do set "NAME=%%~nxf"
    
    echo !NAME!:
    echo   Path: !PATH!
    echo   Commit: !COMMIT!
    
    if exist "!PATH!" (
        pushd "!PATH!"
        
        REM Get tag if on a tag
        for /f "delims=" %%t in ('git describe --tags --exact-match 2^>nul') do (
            echo   Tag: %%t
        )
        
        REM Get branch
        for /f "delims=" %%b in ('git rev-parse --abbrev-ref HEAD 2^>nul') do (
            echo   Branch: %%b
        )
        
        popd
    )
    echo.
)

goto :exit_script

:exit_script
endlocal
exit /b 0
