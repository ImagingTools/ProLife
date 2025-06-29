@echo off

echo === Waiting for PostgreSQL to start ===
:waitloop
pg_isready -h localhost -p 5432 -U postgres
IF ERRORLEVEL 1 (
    echo PostgreSQL is not ready yet. Waiting...
    timeout /t 2 >nul
    GOTO waitloop
)

echo === Installing applications ===
set PROLIFE_BACKUP_FILE=C:\backups\prolife.backup
set LISA_BACKUP_FILE=C:\backups\lisa.backup
set PUMA_BACKUP_FILE=C:\backups\puma.backup

set PROLIFE_DB_NAME=prolife
set LISA_DB_NAME=lisa
set PUMA_DB_NAME=puma
set PG_USER=postgres
set PG_HOST=localhost
set PG_PORT=5432
set PGPASSWORD=root

rem === Initial restore ===
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "DROP DATABASE IF EXISTS %PROLIFE_DB_NAME%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "CREATE DATABASE %PROLIFE_DB_NAME% WITH OWNER = %PG_USER%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "DROP DATABASE IF EXISTS %LISA_DB_NAME%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "CREATE DATABASE %LISA_DB_NAME% WITH OWNER = %PG_USER%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "DROP DATABASE IF EXISTS %PUMA_DB_NAME%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "CREATE DATABASE %PUMA_DB_NAME% WITH OWNER = %PG_USER%;"  

pg_restore -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -d %PROLIFE_DB_NAME% --verbose "%PROLIFE_BACKUP_FILE%"
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
pg_restore -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -d %LISA_DB_NAME% --verbose "%LISA_BACKUP_FILE%"
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
pg_restore -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -d %PUMA_DB_NAME% --verbose "%PUMA_BACKUP_FILE%"
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

rem === Установка приложений ===
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

rem === Повторный откат баз данных ===
echo === Resetting databases before GUI tests ===
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "DROP DATABASE IF EXISTS %PROLIFE_DB_NAME%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "CREATE DATABASE %PROLIFE_DB_NAME% WITH OWNER = %PG_USER%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "DROP DATABASE IF EXISTS %LISA_DB_NAME%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "CREATE DATABASE %LISA_DB_NAME% WITH OWNER = %PG_USER%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "DROP DATABASE IF EXISTS %PUMA_DB_NAME%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "CREATE DATABASE %PUMA_DB_NAME% WITH OWNER = %PG_USER%;"  

pg_restore -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -d %PROLIFE_DB_NAME% --verbose "%PROLIFE_BACKUP_FILE%"
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
pg_restore -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -d %LISA_DB_NAME% --verbose "%LISA_BACKUP_FILE%"
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
pg_restore -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -d %PUMA_DB_NAME% --verbose "%PUMA_BACKUP_FILE%"
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

rem === Перезапуск EXE ===
taskkill /IM LisaServer.exe /F
taskkill /IM PumaServer.exe /F
taskkill /IM ProLifeServer.exe /F

start "" /B "C:\Program Files\ImagingTools\LisaServer\LisaServer.exe"
start "" /B "C:\Program Files\ImagingTools\PumaServer\PumaServer.exe"
start "" /B "C:\Program Files\ImagingTools\ProLifeServer\ProLifeServer.exe"

rem === GUI-тесты Playwright ===
echo === Running Playwright tests ===
cd C:\gui
npx playwright test
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
