@echo off

set BUILD_DIR=%1

chcp 65001 > nul

:: Пути к бэкапам БД
set PROLIFE_BACKUP_FILE=%BUILD_DIR%\ProLife\Tests\prolifetest.backup
set LISA_BACKUP_FILE=%BUILD_DIR%\ProLife\Tests\lisatest.backup
set PUMA_BACKUP_FILE=%BUILD_DIR%\ProLife\Tests\pumatest.backup

:: Настройки PostgreSQL
set PROLIFE_DB_NAME=prolifetest
set LISA_DB_NAME=lisatest
set PUMA_DB_NAME=pumatest
set PG_USER=postgres
set PG_HOST=localhost
set PG_PORT=5432
set PGPASSWORD=root

:: Удаляем и создаем базы
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "DROP DATABASE IF EXISTS %PROLIFE_DB_NAME%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "CREATE DATABASE %PROLIFE_DB_NAME% WITH OWNER = %PG_USER%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "DROP DATABASE IF EXISTS %LISA_DB_NAME%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "CREATE DATABASE %LISA_DB_NAME% WITH OWNER = %PG_USER%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "DROP DATABASE IF EXISTS %PUMA_DB_NAME%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "CREATE DATABASE %PUMA_DB_NAME% WITH OWNER = %PG_USER%;"  

:: Восстанавливаем базы
pg_restore -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -d %PROLIFE_DB_NAME% --verbose "%PROLIFE_BACKUP_FILE%"
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
pg_restore -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -d %LISA_DB_NAME% --verbose "%LISA_BACKUP_FILE%"
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
pg_restore -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -d %PUMA_DB_NAME% --verbose "%PUMA_BACKUP_FILE%"
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

timeout /t 5

:: Запуск серверов
start "" "%BUILD_DIR%\Lisa\Bin\Release_Qt6_VC17_x64\LisaServer.exe"
start "" "%BUILD_DIR%\ProLife\Bin\Release_Qt6_VC17_x64\ProLifeServer.exe"
start "" "%BUILD_DIR%\Puma\Bin\Release_Qt6_VC17_x64\PumaServer.exe"

timeout /t 5

echo Newman started
call newman run "%BUILD_DIR%\ProLife\Tests\postman_collection.json" --disable-unicode --suppress-exit-code 1
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
echo Newman ended

:: Завершаем процессы
taskkill /IM "LisaServer.exe" /F
taskkill /IM "ProLifeServer.exe" /F
taskkill /IM "PumaServer.exe" /F

exit /b 0