@echo off
REM Quick Release Preparation Script for Windows
REM Automates the most common release preparation steps

setlocal enabledelayedexpansion

REM Script directory
set "SCRIPT_DIR=%~dp0"
set "REPO_ROOT=%SCRIPT_DIR%.."
cd /d "%REPO_ROOT%"

REM Check if version is provided
if "%~1"=="" (
    echo [ERROR] Version is required
    echo Usage: %~nx0 VERSION [--skip-tests]
    echo Example: %~nx0 2.1.0
    echo.
    echo Options:
    echo   --skip-tests    Skip build and test steps
    exit /b 1
)

set "VERSION=%~1"
set "SKIP_TESTS=false"

if "%~2"=="--skip-tests" set "SKIP_TESTS=true"

REM Validate version format (MAJOR.MINOR.PATCH)
echo %VERSION% | findstr /r "^[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*$" >nul
if errorlevel 1 (
    echo [ERROR] Invalid version format. Use MAJOR.MINOR.PATCH ^(e.g., 2.1.0^)
    exit /b 1
)

echo.
echo =========================================
echo ProLife Quick Release Preparation
echo =========================================
echo.
echo Version: v%VERSION%
echo Date: %date%
echo.

REM Step 1: Initial validation
echo [1] Checking current state...
echo.

git diff-index --quiet HEAD -- 2>nul
if errorlevel 1 (
    echo [WARNING] You have uncommitted changes. Please commit or stash them first.
    git status --short
    echo.
    set /p CONTINUE="Continue anyway? (y/n): "
    if /i not "!CONTINUE!"=="y" exit /b 1
)

echo [OK] Repository state checked
echo.

REM Step 2: Create release branch
echo [2] Creating release branch...
echo.

set "BRANCH_NAME=release/v%VERSION%"

git rev-parse --verify "%BRANCH_NAME%" >nul 2>&1
if not errorlevel 1 (
    echo [WARNING] Branch %BRANCH_NAME% already exists
    set /p SWITCH="Switch to existing branch? (y/n): "
    if /i "!SWITCH!"=="y" (
        git checkout "%BRANCH_NAME%"
    ) else (
        echo [ERROR] Aborting
        exit /b 1
    )
) else (
    git checkout -b "%BRANCH_NAME%"
    echo [OK] Created and switched to branch: %BRANCH_NAME%
)

echo.

REM Step 3: Initialize submodules if needed
echo [3] Ensuring submodules are initialized...
echo.

git submodule update --init --recursive
if errorlevel 1 (
    echo [ERROR] Failed to initialize submodules
    exit /b 1
)
echo [OK] Submodules initialized
echo.

REM Step 4: Auto-update submodules
echo [4] Updating submodules to latest tagged versions...
echo.

call "%SCRIPT_DIR%validate-release.bat" auto-update
if errorlevel 1 (
    echo [ERROR] Failed to update submodules
    exit /b 1
)

echo.

REM Step 5: Build and test (if not skipped)
if "%SKIP_TESTS%"=="false" (
    echo [5] Building and testing...
    echo.
    
    set "BUILD_DIR=%REPO_ROOT%\build-release-check"
    
    if exist "!BUILD_DIR!" (
        echo [INFO] Removing existing build directory...
        rd /s /q "!BUILD_DIR!"
    )
    
    mkdir "!BUILD_DIR!"
    cd /d "!BUILD_DIR!"
    
    echo [INFO] Configuring build...
    cmake ..\Build\CMake -DCMAKE_BUILD_TYPE=Release
    if errorlevel 1 (
        echo [ERROR] CMake configuration failed
        cd /d "%REPO_ROOT%"
        exit /b 1
    )
    
    echo [INFO] Building...
    cmake --build . --config Release
    if errorlevel 1 (
        echo [ERROR] Build failed
        cd /d "%REPO_ROOT%"
        exit /b 1
    )
    
    echo [OK] Build successful
    
    echo [INFO] Running tests...
    ctest --config Release --output-on-failure
    if errorlevel 1 (
        echo [WARNING] Some tests failed. Please review.
        set /p CONTINUE="Continue with release preparation? (y/n): "
        if /i not "!CONTINUE!"=="y" (
            cd /d "%REPO_ROOT%"
            exit /b 1
        )
    ) else (
        echo [OK] All tests passed
    )
    
    cd /d "%REPO_ROOT%"
    echo.
) else (
    echo [5] Skipping build and test ^(--skip-tests flag^)
    echo.
)

REM Step 6: Update version in CMakeLists.txt
echo [6] Updating version in CMakeLists.txt...
echo.

set "CMAKE_FILE=%REPO_ROOT%\Build\CMake\CMakeLists.txt"
if exist "%CMAKE_FILE%" (
    REM Create a temporary file for the replacement
    powershell -Command "(Get-Content '%CMAKE_FILE%') -replace 'project\(ProLifeAll VERSION [0-9.]*\)', 'project(ProLifeAll VERSION %VERSION%)' | Set-Content '%CMAKE_FILE%.tmp'"
    move /y "%CMAKE_FILE%.tmp" "%CMAKE_FILE%" >nul
    echo [OK] Updated CMakeLists.txt to version %VERSION%
) else (
    echo [WARNING] CMakeLists.txt not found at expected location
)

echo.

REM Step 7: Generate submodule report
echo [7] Generating submodule version report...
echo.

set "REPORT_FILE=%REPO_ROOT%\RELEASE_SUBMODULES_v%VERSION%.md"
call "%SCRIPT_DIR%validate-release.bat" report "%REPORT_FILE%"
if errorlevel 1 (
    echo [WARNING] Failed to generate submodule report
)

echo.

REM Step 8: Pin submodules
echo [8] Pinning submodules to current commits...
echo.

call "%REPO_ROOT%\prepare-release.bat" --pin-submodules
if errorlevel 1 (
    echo [ERROR] Failed to pin submodules
    exit /b 1
)
echo [OK] Submodules pinned

echo.

REM Step 9: Final validation
echo [9] Running final validation...
echo.

call "%SCRIPT_DIR%validate-release.bat" validate v%VERSION%
if errorlevel 1 (
    echo [WARNING] Validation found issues. Please review before proceeding.
) else (
    echo [OK] Validation passed
)

echo.

REM Step 10: Summary
echo =========================================
echo Automated preparation complete!
echo =========================================
echo.
echo Summary of changes:
git status --short
echo.
echo Next steps ^(MANUAL^):
echo.
echo   1. Review and update CHANGELOG.md with release notes:
echo      notepad CHANGELOG.md
echo.
echo   2. Review the submodule version report:
echo      notepad %REPORT_FILE%
echo.
echo   3. Review all changes:
echo      git diff
echo.
echo   4. If everything looks good, commit the changes:
echo      git add .
echo      git commit -m "chore: prepare release v%VERSION%"
echo.
echo      - Updated submodules to stable versions
echo      - Updated CHANGELOG.md
echo      - Updated version in CMakeLists.txt
echo      - Pinned all dependencies
echo.
echo   5. Push the release branch:
echo      git push -u origin %BRANCH_NAME%
echo.
echo   6. Create a Pull Request for review
echo.
echo   7. After PR approval, run the GitHub Actions workflow:
echo      - Go to Actions -^> Release Preparation
echo      - Run with branch: %BRANCH_NAME%
echo      - Target tag: v%VERSION%
echo      - Base tag: ^(previous version^)
echo      - Dry run: false
echo.
echo Files modified:
echo   - Build\CMake\CMakeLists.txt ^(version updated^)
echo   - RELEASE_SUBMODULES_v%VERSION%.md ^(created^)
echo   - All submodule references ^(pinned to specific commits^)
echo.

endlocal
