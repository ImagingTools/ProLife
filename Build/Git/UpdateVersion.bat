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

REM --- Logic for submodules ---

echo Checking submodules for version scripts...

REM Navigate to repository root
set "REPO_ROOT=%~dp0..\.."
pushd "%REPO_ROOT%"

REM List of submodules and their script paths
set "SUBMODULES=3rdParty\Acf:Build\Git\GenerateVersion.bat"
set "SUBMODULES=%SUBMODULES% 3rdParty\AcfSln:Build\Git\UpdateVersion.bat"
set "SUBMODULES=%SUBMODULES% 3rdParty\ImtCore:Build\Git\UpdateVersion.bat"
set "SUBMODULES=%SUBMODULES% 3rdParty\Lisa:Build\Git\UpdateVersion.bat"
set "SUBMODULES=%SUBMODULES% 3rdParty\Puma:Build\Git\UpdateVersion.bat"
set "SUBMODULES=%SUBMODULES% 3rdParty\Agentino:Build\Git\UpdateVersion.bat"

REM Iterate over submodules and execute version scripts
for %%S in (%SUBMODULES%) do (
    for /f "tokens=1,2 delims=:" %%A in ("%%S") do (
        set "SUBMODULE_PATH=%%A"
        set "SCRIPT_PATH=%%B"
        set "FULL_SUBMODULE_PATH=%REPO_ROOT%\%%A"
        set "FULL_SCRIPT_PATH=%REPO_ROOT%\%%A\%%B"
        
        if exist "!FULL_SUBMODULE_PATH!" (
            if exist "!FULL_SCRIPT_PATH!" (
                echo [%%A] Found %%B. Executing...
                pushd "!FULL_SUBMODULE_PATH!"
                call "%%B"
                popd
            ) else (
                echo [%%A] Skipping (script not found^)
            )
        ) else (
            echo [%%A] Skipping (directory not found^)
        )
    )
)

popd

echo UpdateVersion completed

endlocal
