@echo off
REM ProLife Release Validation and Automation Script for Windows
REM This script validates and automates the release preparation process

setlocal enabledelayedexpansion

REM Script directory
set "SCRIPT_DIR=%~dp0"
set "REPO_ROOT=%SCRIPT_DIR%.."
cd /d "%REPO_ROOT%"

REM Validation results
set "VALIDATION_PASSED=true"
set "ERROR_COUNT=0"
set "WARNING_COUNT=0"

REM Show help
if "%~1"=="" goto :show_help
if "%~1"=="help" goto :show_help
if "%~1"=="--help" goto :show_help
if "%~1"=="-h" goto :show_help

REM Main command dispatcher
if "%~1"=="validate" goto :validate
if "%~1"=="auto-update" goto :auto_update
if "%~1"=="pin" goto :pin
if "%~1"=="report" goto :report
if "%~1"=="summary" goto :summary

echo [ERROR] Unknown command: %~1
echo.
goto :show_help

:validate
echo ========================================
echo ProLife Release Validation
echo ========================================
echo.

call :validate_submodules_initialized
call :validate_no_uncommitted_changes
call :validate_submodule_versions
call :validate_changelog
call :validate_cmake_version "%~2"

echo.
echo ========================================
echo Validation Summary
echo ========================================
echo.
if "%VALIDATION_PASSED%"=="true" (
    if %WARNING_COUNT%==0 (
        echo [OK] All validations passed! Ready for release.
        exit /b 0
    ) else (
        echo [WARNING] Validation passed with %WARNING_COUNT% warnings. Review warnings before proceeding.
        exit /b 0
    )
) else (
    echo [ERROR] Validation failed with %ERROR_COUNT% errors. Please fix errors before proceeding with release.
    exit /b 1
)

:auto_update
echo ========================================
echo Auto-Updating Submodules to Latest Tagged Versions
echo ========================================
echo.

set "UPDATED=false"

for /f "tokens=1,2" %%a in ('git submodule status') do (
    set "COMMIT=%%a"
    set "PATH=%%b"
    set "COMMIT=!COMMIT:~0,7!"
    
    if exist "!PATH!" (
        pushd "!PATH!"
        
        REM Fetch latest tags
        git fetch --tags --quiet
        
        REM Get latest semantic version tag
        for /f "delims=" %%t in ('git tag -l ^| findstr /r "^v*[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*$" ^| sort /r') do (
            set "LATEST_TAG=%%t"
            goto :found_tag
        )
        
        :found_tag
        if not "!LATEST_TAG!"=="" (
            REM Get current tag if on a tag
            for /f "delims=" %%c in ('git describe --tags --exact-match 2^>nul') do set "CURRENT_TAG=%%c"
            
            if "!CURRENT_TAG!"=="!LATEST_TAG!" (
                echo [INFO] !PATH! is already on latest tag: !LATEST_TAG!
            ) else (
                echo [INFO] !PATH!: Updating from !CURRENT_TAG! to !LATEST_TAG!
                git checkout "!LATEST_TAG!" --quiet
                set "UPDATED=true"
                popd
                git add "!PATH!"
                pushd "!PATH!"
                echo [OK] !PATH! updated to !LATEST_TAG!
            )
            set "LATEST_TAG="
            set "CURRENT_TAG="
        ) else (
            echo [WARNING] No semantic version tags found for !PATH!, skipping
        )
        
        popd
    )
)

if "%UPDATED%"=="true" (
    echo.
    echo [OK] Submodules updated. Please review changes with 'git status'
) else (
    echo.
    echo [INFO] All submodules are already on latest tagged versions
)

exit /b 0

:pin
echo ========================================
echo Pinning Submodules with Validation
echo ========================================
echo.

call :validate_submodule_versions

echo.
set /p PROCEED="Proceed with pinning submodules? (y/n): "
if /i "%PROCEED%"=="y" (
    git submodule foreach "git rev-parse HEAD > nul"
    git add -A
    echo [OK] Submodules pinned
) else (
    echo [INFO] Pinning cancelled
)

exit /b 0

:report
set "OUTPUT_FILE=%~2"
if "%OUTPUT_FILE%"=="" set "OUTPUT_FILE=SUBMODULE_VERSIONS.md"

echo ========================================
echo Generating Submodule Version Report
echo ========================================
echo.

(
    echo # Submodule Versions for Release
    echo.
    echo Generated: %date% %time%
    echo.
    echo ## Submodules
    echo.
    
    for /f "tokens=1,2" %%a in ('git submodule status') do (
        set "COMMIT=%%a"
        set "PATH=%%b"
        set "COMMIT=!COMMIT:~1!"
        set "NAME=%%b"
        
        REM Extract just the last part of the path
        for %%f in ("!NAME!") do set "NAME=%%~nxf"
        
        if exist "!PATH!" (
            pushd "!PATH!"
            
            for /f "delims=" %%t in ('git describe --tags --exact-match 2^>nul') do set "TAG=%%t"
            if "!TAG!"=="" set "TAG=N/A"
            
            for /f "delims=" %%b in ('git rev-parse --abbrev-ref HEAD 2^>nul') do set "BRANCH=%%b"
            if "!BRANCH!"=="" set "BRANCH=N/A"
            
            for /f "delims=" %%u in ('git config --get remote.origin.url 2^>nul') do set "URL=%%u"
            if "!URL!"=="" set "URL=N/A"
            
            popd
            
            echo ### !NAME!
            echo - **Path:** `!PATH!`
            echo - **Repository:** !URL!
            echo - **Commit:** `!COMMIT!`
            echo - **Tag:** `!TAG!`
            echo - **Branch:** `!BRANCH!`
            echo.
            
            set "TAG="
            set "BRANCH="
            set "URL="
        )
    )
) > "%OUTPUT_FILE%"

echo [OK] Submodule report saved to %OUTPUT_FILE%
exit /b 0

:summary
set "VERSION=%~2"

echo ========================================
echo Release Summary
echo ========================================
echo.
if not "%VERSION%"=="" echo Version: %VERSION%
echo Date: %date%
echo.
echo Submodules:

for /f "tokens=1,2" %%a in ('git submodule status') do (
    set "COMMIT=%%a"
    set "PATH=%%b"
    set "COMMIT=!COMMIT:~1,7!"
    set "NAME=%%b"
    
    REM Extract just the last part of the path
    for %%f in ("!NAME!") do set "NAME=%%~nxf"
    
    if exist "!PATH!" (
        pushd "!PATH!"
        for /f "delims=" %%t in ('git describe --tags --exact-match 2^>nul') do (
            echo   - !NAME!: %%t
            goto :next_submodule
        )
        echo   - !NAME!: commit !COMMIT!
        :next_submodule
        popd
    )
)

echo.
exit /b 0

REM ========================================
REM Helper Functions
REM ========================================

:validate_submodules_initialized
echo ========================================
echo Validating Submodules Initialization
echo ========================================
echo.

set "ALL_INITIALIZED=true"

for /f "tokens=1,2" %%a in ('git submodule status') do (
    set "COMMIT=%%a"
    set "PATH=%%b"
    
    if "!COMMIT:~0,1!"=="-" (
        echo [ERROR] Submodule !PATH! is not initialized
        set "ALL_INITIALIZED=false"
        set "VALIDATION_PASSED=false"
        set /a ERROR_COUNT+=1
    ) else (
        echo [OK] Submodule !PATH! is initialized
    )
)

if "%ALL_INITIALIZED%"=="true" (
    echo.
    echo [OK] All submodules are initialized
)
echo.
goto :eof

:validate_no_uncommitted_changes
echo ========================================
echo Checking for Uncommitted Changes
echo ========================================
echo.

git diff-index --quiet HEAD -- 2>nul
if errorlevel 1 (
    echo [ERROR] There are uncommitted changes in main repository
    echo.
    echo Changed files:
    git status --short
    set "VALIDATION_PASSED=false"
    set /a ERROR_COUNT+=1
) else (
    echo [OK] No uncommitted changes in main repository
)
echo.
goto :eof

:validate_submodule_versions
echo ========================================
echo Validating Submodule Versions
echo ========================================
echo.

for /f "tokens=1,2" %%a in ('git submodule status') do (
    set "COMMIT=%%a"
    set "PATH=%%b"
    set "COMMIT=!COMMIT:~1!"
    
    if exist "!PATH!" (
        pushd "!PATH!"
        
        REM Check if on a tagged version
        git describe --tags --exact-match >nul 2>&1
        if not errorlevel 1 (
            for /f "delims=" %%t in ('git describe --tags --exact-match 2^>nul') do (
                echo [OK] !PATH! is on tagged version: %%t
            )
        ) else (
            for /f "delims=" %%b in ('git rev-parse --abbrev-ref HEAD 2^>nul') do (
                if not "%%b"=="HEAD" (
                    echo [WARNING] !PATH! is on branch %%b ^(commit: !COMMIT:~0,7!^). Consider using a tagged version for release.
                    set /a WARNING_COUNT+=1
                ) else (
                    echo [WARNING] !PATH! is on detached HEAD ^(commit: !COMMIT:~0,7!^). Consider using a tagged version for release.
                    set /a WARNING_COUNT+=1
                )
            )
        )
        
        popd
    )
)
echo.
goto :eof

:validate_changelog
echo ========================================
echo Validating CHANGELOG.md
echo ========================================
echo.

if not exist "CHANGELOG.md" (
    echo [ERROR] CHANGELOG.md not found
    set "VALIDATION_PASSED=false"
    set /a ERROR_COUNT+=1
) else (
    echo [OK] CHANGELOG.md exists
    
    REM Check if CHANGELOG has recent updates
    git diff HEAD~1 HEAD --name-only 2>nul | findstr /i "CHANGELOG.md" >nul
    if not errorlevel 1 (
        echo [OK] CHANGELOG.md has been updated recently
    ) else (
        echo [WARNING] CHANGELOG.md has not been updated recently
        set /a WARNING_COUNT+=1
    )
)
echo.
goto :eof

:validate_cmake_version
set "EXPECTED_VERSION=%~1"

echo ========================================
echo Validating CMakeLists.txt Version
echo ========================================
echo.

if not exist "Build\CMake\CMakeLists.txt" (
    echo [ERROR] Build\CMake\CMakeLists.txt not found
    set "VALIDATION_PASSED=false"
    set /a ERROR_COUNT+=1
    echo.
    goto :eof
)

REM Extract version from CMakeLists.txt
for /f "tokens=*" %%a in ('findstr /r "project.*ProLifeAll.*VERSION" Build\CMake\CMakeLists.txt') do (
    set "LINE=%%a"
    for /f "tokens=2 delims=()" %%b in ("!LINE!") do (
        set "CONTENT=%%b"
        for /f "tokens=3" %%c in ("!CONTENT!") do (
            set "CMAKE_VERSION=%%c"
            goto :version_found
        )
    )
)

:version_found
if "%CMAKE_VERSION%"=="" (
    echo [WARNING] Could not extract version from CMakeLists.txt
    set /a WARNING_COUNT+=1
) else (
    if "%EXPECTED_VERSION%"=="" (
        echo [INFO] Current CMakeLists.txt version: %CMAKE_VERSION%
    ) else (
        set "CLEAN_EXPECTED=%EXPECTED_VERSION:v=%"
        if "%CMAKE_VERSION%"=="!CLEAN_EXPECTED!" (
            echo [OK] CMakeLists.txt version matches: %CMAKE_VERSION%
        ) else (
            echo [ERROR] CMakeLists.txt version ^(%CMAKE_VERSION%^) does not match expected version ^(!CLEAN_EXPECTED!^)
            set "VALIDATION_PASSED=false"
            set /a ERROR_COUNT+=1
        )
    )
)
echo.
goto :eof

:show_help
echo ProLife Release Validation and Automation Script
echo.
echo Usage: %~nx0 [COMMAND] [OPTIONS]
echo.
echo Commands:
echo   validate [VERSION]           Run full validation for release
echo   auto-update                  Auto-update all submodules to latest tagged versions
echo   pin                          Pin submodules with validation
echo   report [OUTPUT_FILE]         Generate submodule version report
echo   summary [VERSION]            Generate release summary
echo   help                         Show this help message
echo.
echo Examples:
echo   %~nx0 validate v2.1.0       # Validate release preparation for version 2.1.0
echo   %~nx0 auto-update           # Update all submodules to latest tags
echo   %~nx0 pin                   # Pin submodules after validation
echo   %~nx0 report versions.md    # Generate version report
echo   %~nx0 summary v2.1.0        # Generate release summary
echo.
echo Typical release workflow:
echo   1. %~nx0 validate           # Check current state
echo   2. %~nx0 auto-update        # Update submodules to latest versions
echo   3. REM Test the integration
echo   4. %~nx0 validate v2.1.0    # Validate for specific version
echo   5. %~nx0 report             # Generate version report
echo   6. %~nx0 pin                # Pin submodules
echo   7. REM Commit and push
echo.
exit /b 0

endlocal
