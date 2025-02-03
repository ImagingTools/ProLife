@echo off

set PROLIFE_BACKUP_FILE="C:\Sybnavigation\ITDevelopment\ProLife\Tests\prolifetest.backup"
set LISA_BACKUP_FILE="C:\Sybnavigation\ITDevelopment\ProLife\Tests\lisatest.backup"
set PUMA_BACKUP_FILE="C:\Sybnavigation\ITDevelopment\ProLife\Tests\pumatest.backup"
set PROLIFE_DB_NAME="prolifetest"
set LISA_DB_NAME="lisatest"
set PUMA_DB_NAME="pumatest"
set PG_USER="postgres"
set PG_HOST="localhost"
set PG_PORT="5432"

SET PGPASSWORD=root

psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "DROP DATABASE IF EXISTS %PROLIFE_DB_NAME%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "CREATE DATABASE %PROLIFE_DB_NAME% WITH OWNER = %PG_USER%;"

psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "DROP DATABASE IF EXISTS %LISA_DB_NAME%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "CREATE DATABASE %LISA_DB_NAME% WITH OWNER = %PG_USER%;"

psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "DROP DATABASE IF EXISTS %PUMA_DB_NAME%;"  
psql -h %PG_HOST% -p %PG_PORT% -U %PG_USER% -c "CREATE DATABASE %PUMA_DB_NAME% WITH OWNER = %PG_USER%;"

:: Запускаем pg_restore
pg_restore -h localhost -p 5432 -U postgres -d %PROLIFE_DB_NAME% --verbose %PROLIFE_BACKUP_FILE%
pg_restore -h localhost -p 5432 -U postgres -d %LISA_DB_NAME% --verbose %LISA_BACKUP_FILE%
pg_restore -h localhost -p 5432 -U postgres -d %PUMA_DB_NAME% --verbose %PUMA_BACKUP_FILE%

timeout /t 5

rem Запуск процессов в отдельных окнах
start "" "C:\Sybnavigation\ITDevelopment\Lisa\Bin\Debug_Qt6_VC17_x64\LisaServer.exe"
start "" "C:\Sybnavigation\ITDevelopment\ProLife\Bin\Debug_Qt6_VC17_x64\ProLifeServer.exe"
start "" "C:\Sybnavigation\ITDevelopment\Puma\Bin\Debug_Qt6_VC17_x64\PumaServer.exe"

timeout /t 5

echo Newman started
rem Запуск тестов в Postman с помощью Newman
call newman run "C:\Sybnavigation\ITDevelopment\ProLife\Tests\postman_collection.json"
echo Newman ended

rem Завершение процессов
taskkill /IM "LisaServer.exe" /F
taskkill /IM "ProLifeServer.exe" /F
taskkill /IM "PumaServer.exe" /F

pause