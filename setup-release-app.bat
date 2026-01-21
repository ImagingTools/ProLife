@echo off
REM ProLife Release App - Setup Script
REM This script checks required dependencies

echo =========================================
echo ProLife Release App - Setup
echo =========================================
echo.

REM Check Python
echo Checking Python installation...
python --version >nul 2>&1
if %errorlevel% equ 0 (
    for /f "tokens=2" %%i in ('python --version 2^>^&1') do set PYTHON_VERSION=%%i
    echo [OK] Python is installed: %PYTHON_VERSION%
) else (
    echo [ERROR] Python is not installed
    echo   Please install Python 3.7 or later from https://www.python.org/
    echo   Make sure to check "Add Python to PATH" during installation
    pause
    exit /b 1
)

REM Check Git
echo Checking Git installation...
git --version >nul 2>&1
if %errorlevel% equ 0 (
    for /f "tokens=3" %%i in ('git --version') do set GIT_VERSION=%%i
    echo [OK] Git is installed: %GIT_VERSION%
) else (
    echo [WARNING] Git is not installed
    echo   Install Git from https://git-scm.com/ to use version control features
)

REM Check CMake
echo Checking CMake installation...
cmake --version >nul 2>&1
if %errorlevel% equ 0 (
    for /f "tokens=3" %%i in ('cmake --version') do set CMAKE_VERSION=%%i
    echo [OK] CMake is installed: %CMAKE_VERSION%
) else (
    echo [WARNING] CMake is not installed
    echo   Install CMake from https://cmake.org/ to use build features
)

REM Check Tkinter
echo Checking Tkinter availability...
python -c "import tkinter" >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] Tkinter is available
) else (
    echo [ERROR] Tkinter is not available
    echo   Tkinter should be included with Python on Windows
    echo   Try reinstalling Python with the "tcl/tk and IDLE" option checked
    pause
    exit /b 1
)

REM Check InnoSetup (optional)
echo Checking InnoSetup installation...
if exist "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" (
    echo [OK] InnoSetup 6 is installed (x86)
) else if exist "C:\Program Files\Inno Setup 6\ISCC.exe" (
    echo [OK] InnoSetup 6 is installed
) else (
    echo [WARNING] InnoSetup is not installed
    echo   Install InnoSetup from https://jrsoftware.org/isdl.php to create installers
)

echo.
echo =========================================
echo Setup complete!
echo =========================================
echo.
echo You can now run the ProLife Release App:
echo.
echo   ProLifeReleaseApp.bat
echo.
echo Or directly with Python:
echo.
echo   python ProLifeReleaseApp.py
echo.
pause
