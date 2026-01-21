@echo off
REM ProLife Release App Launcher
REM This script launches the ProLife Release App

echo Starting ProLife Release App...

REM Check if Python is available
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Python is not installed or not in PATH
    echo Please install Python 3.7 or later from https://www.python.org/
    pause
    exit /b 1
)

REM Launch the app
python "%~dp0ProLifeReleaseApp.py"

if %errorlevel% neq 0 (
    echo.
    echo ERROR: Failed to launch ProLife Release App
    pause
)
