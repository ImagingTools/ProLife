@echo off
setlocal enabledelayedexpansion

cd /d "%~dp0"

set "FILE=..\..\Partitura\ProLifeVoce.arp\VersionInfo.acc.xtrsvn"

git fetch --prune --unshallow 2>nul

for /f "usebackq delims=" %%i in (`git rev-list --count origin/master 2^>nul`) do set REV=%%i

if not defined REV (
    for /f "usebackq delims=" %%i in (`git rev-list --count HEAD 2^>nul`) do set REV=%%i
)

if not defined REV (
    echo Failed to compute revision count.
    exit /b 1
)

set /a REV_OFFSET=REV+10000

git diff-index --quiet HEAD --
if %errorlevel%==0 (
    set DIRTY=0
) else (
    set DIRTY=1
)

echo Git revision: %REV% (version: %REV_OFFSET%), dirty: %DIRTY%
echo Processing file: %FILE%

set "OUT=%FILE:.xtrsvn=%"

(for /f "usebackq delims=" %%L in ("%FILE%") do (
    set "line=%%L"
    set "line=!line:$WCREV$=%REV_OFFSET%!"
    set "line=!line:$WCMODS?1:0$=%DIRTY%!"
    echo(!line!
)) > "%OUT%"

echo Wrote %OUT% with WCREV=%REV_OFFSET% and WCMODS=%DIRTY%

REM --- Logic for dependencies ---
REM Note: Dependencies should be updated in their own repositories
REM This script only updates ProLife version information

echo UpdateVersion completed
goto :EOF
            )
        ) else (
            echo [%%A] Skipping (directory not found^)
        )
    )
)

popd

echo UpdateVersion completed

endlocal
