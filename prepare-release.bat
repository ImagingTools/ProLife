@echo off
REM ProLife Release Preparation Script for Windows
REM This script helps manage releases and submodule versions

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
cd /d "%SCRIPT_DIR%"

REM Check if we're in a git repository
git rev-parse --is-inside-work-tree >nul 2>&1
if errorlevel 1 (
    echo Error: Not in a git repository
    exit /b 1
)

if "%1"=="" goto :show_help
if "%1"=="--help" goto :show_help
if "%1"=="-h" goto :show_help
if "%1"=="--status" goto :show_status
if "%1"=="-s" goto :show_status
if "%1"=="--init" goto :init_submodules
if "%1"=="--update-all" goto :update_all
if "%1"=="--pin-submodules" goto :pin_submodules
if "%1"=="--create-release" goto :create_release
if "%1"=="--update-submodule" goto :update_submodule
if "%1"=="--list-tags" goto :list_tags
if "%1"=="--report" goto :generate_report
if "%1"=="-r" goto :generate_report

echo Unknown option: %1
echo.
goto :show_help

:show_help
echo ProLife Release Preparation Script
echo.
echo Usage: %~nx0 [OPTION]
echo.
echo Options:
echo   --status, -s                    Show current submodule status
echo   --init                          Initialize all submodules
echo   --update-all                    Update all submodules to latest
echo   --pin-submodules                Pin all submodules to current commits
echo   --create-release VERSION        Create a release branch (e.g., 2.1.0)
echo   --update-submodule PATH VERSION Update specific submodule to version
echo   --list-tags PATH                List available tags for a submodule
echo   --report, -r                    Generate submodule version report
echo   --help, -h                      Show this help message
echo.
echo Examples:
echo   %~nx0 --status
echo   %~nx0 --init
echo   %~nx0 --create-release 2.1.0
echo   %~nx0 --update-submodule 3rdParty/Acf v1.2.3
echo   %~nx0 --list-tags 3rdParty/Acf
echo   %~nx0 --pin-submodules
echo   %~nx0 --report
echo.
echo Workflow for creating a release:
echo   1. %~nx0 --status                          # Check current state
echo   2. %~nx0 --create-release 2.1.0            # Create release branch
echo   3. # Update submodule versions manually or with --update-submodule
echo   4. # Update version numbers in code and CHANGELOG.md
echo   5. %~nx0 --pin-submodules                  # Pin to current commits
echo   6. git commit -m "Prepare release v2.1.0"
echo   7. git push -u origin release/v2.1.0
echo.
exit /b 0

:show_status
echo ========================================
echo Current Submodule Status
echo ========================================
echo.
git submodule status
echo.
echo Detailed information:
echo.
for /f "tokens=1,2" %%a in ('git submodule status') do (
    set "commit=%%a"
    set "path=%%b"
    
    REM Remove leading +/- if present
    set "commit=!commit:~0,7!"
    
    echo Submodule: !path!
    echo   Commit: !commit!
    
    if exist "!path!" (
        pushd "!path!" >nul 2>&1
        
        REM Get current tag
        for /f "delims=" %%t in ('git describe --tags --exact-match 2^>nul') do (
            echo   Tag: %%t
        )
        
        REM Get current branch
        for /f "delims=" %%b in ('git rev-parse --abbrev-ref HEAD 2^>nul') do (
            if not "%%b"=="HEAD" (
                echo   Branch: %%b
            ) else (
                echo   Detached HEAD
            )
        )
        
        popd >nul 2>&1
    ) else (
        echo   Not initialized
    )
    echo.
)
exit /b 0

:init_submodules
echo ========================================
echo Initializing Submodules
echo ========================================
echo.
echo Initializing and updating all submodules...
git submodule update --init --recursive
echo.
echo All submodules initialized
exit /b 0

:update_all
echo ========================================
echo Updating All Submodules
echo ========================================
echo.
echo Fetching updates for all submodules...
git submodule update --remote --recursive
echo.
echo All submodules updated
goto :show_status

:pin_submodules
echo ========================================
echo Pinning Submodules to Current Commits
echo ========================================
echo.
git submodule foreach "git rev-parse HEAD >nul"
git add -A
echo.
echo All submodules pinned to their current commits
echo Run 'git status' to see changes
exit /b 0

:create_release
if "%2"=="" (
    echo Error: Version is required
    echo Usage: %~nx0 --create-release VERSION
    exit /b 1
)

set "version=%2"
set "branch_name=release/v%version%"

echo ========================================
echo Creating Release Branch
echo ========================================
echo.

REM Check if branch already exists
git rev-parse --verify "%branch_name%" >nul 2>&1
if not errorlevel 1 (
    echo Error: Branch %branch_name% already exists
    exit /b 1
)

REM Check for uncommitted changes
git diff-index --quiet HEAD -- >nul 2>&1
if errorlevel 1 (
    echo Warning: You have uncommitted changes. Please commit or stash them first.
    exit /b 1
)

echo Creating branch %branch_name% from current branch...
git checkout -b "%branch_name%"

echo.
echo Created release branch: %branch_name%
echo.
echo Next steps:
echo   1. Update version numbers in your code
echo   2. Update CHANGELOG.md
echo   3. Pin submodules with: %~nx0 --pin-submodules
echo   4. Commit changes: git commit -m "Prepare release v%version%"
echo   5. Push branch: git push -u origin %branch_name%
exit /b 0

:update_submodule
if "%2"=="" (
    echo Error: Submodule path is required
    echo Usage: %~nx0 --update-submodule PATH VERSION
    exit /b 1
)
if "%3"=="" (
    echo Error: Version is required
    echo Usage: %~nx0 --update-submodule PATH VERSION
    exit /b 1
)

set "submodule_path=%2"
set "version=%3"

if not exist "%submodule_path%" (
    echo Error: Submodule %submodule_path% not found
    exit /b 1
)

echo Updating %submodule_path% to %version%...
pushd "%submodule_path%"

REM Fetch latest
git fetch --tags

REM Checkout the version
git rev-parse "%version%" >nul 2>&1
if errorlevel 1 (
    echo Error: Version %version% not found in %submodule_path%
    popd
    exit /b 1
)

git checkout "%version%"
popd

echo Updated %submodule_path% to %version%

REM Stage the submodule change
git add "%submodule_path%"
exit /b 0

:list_tags
if "%2"=="" (
    echo Error: Submodule path is required
    echo Usage: %~nx0 --list-tags PATH
    exit /b 1
)

set "submodule_path=%2"

if not exist "%submodule_path%" (
    echo Error: Submodule %submodule_path% not found or not initialized
    exit /b 1
)

echo Available tags for %submodule_path%:
pushd "%submodule_path%"
git tag -l
popd
exit /b 0

:generate_report
echo ========================================
echo Submodule Version Report
echo ========================================
echo.

set "report_file=SUBMODULE_VERSIONS.md"

echo # ProLife Submodule Versions > "%report_file%"
echo. >> "%report_file%"
echo Generated: %date% %time% >> "%report_file%"
echo. >> "%report_file%"
echo ## Current Versions >> "%report_file%"
echo. >> "%report_file%"

for /f "tokens=1,2" %%a in ('git submodule status') do (
    set "commit=%%a"
    set "path=%%b"
    
    REM Remove leading +/- if present
    set "commit=!commit:~1!"
    if "!commit:~0,1!"=="+" set "commit=!commit:~1!"
    if "!commit:~0,1!"=="-" set "commit=!commit:~1!"
    
    for %%f in ("!path!") do set "name=%%~nxf"
    
    echo ### !name! >> "%report_file%"
    echo - Path: `!path!` >> "%report_file%"
    echo - Commit: `!commit!` >> "%report_file%"
    
    if exist "!path!" (
        pushd "!path!" >nul 2>&1
        
        for /f "delims=" %%t in ('git describe --tags --exact-match 2^>nul') do (
            echo - Tag: `%%t` >> "%report_file%"
        )
        
        for /f "delims=" %%b in ('git rev-parse --abbrev-ref HEAD 2^>nul') do (
            echo - Branch: `%%b` >> "%report_file%"
        )
        
        popd >nul 2>&1
    )
    
    echo. >> "%report_file%"
)

echo Version report generated: %report_file%
type "%report_file%"
exit /b 0

endlocal
