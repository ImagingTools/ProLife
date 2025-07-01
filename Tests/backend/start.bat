@echo off

echo === Waiting for PostgreSQL to start ===
:waitloop
pg_isready -h localhost -p 5432 -U postgres
IF ERRORLEVEL 1 (
    echo PostgreSQL is not ready yet. Waiting...
    timeout /t 2 >nul
    GOTO waitloop
)

call reset_db.bat
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

C:\installers\LisaServerInstall.exe /VERYSILENT /NORESTART
C:\installers\PumaServerInstall.exe /VERYSILENT /NORESTART
C:\installers\ProLifeServerInstall.exe /VERYSILENT /NORESTART

rem === Запуск приложений ===
start "" /B "C:\Program Files\ImagingTools\LisaServer\LisaServer.exe"
start "" /B "C:\Program Files\ImagingTools\PumaServer\PumaServer.exe"
start "" /B "C:\Program Files\ImagingTools\ProLifeServer\ProLifeServer.exe"

rem === Выполнение API-тестов ===
echo === Running Newman tests ===
newman run C:\postman_collection.json
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
