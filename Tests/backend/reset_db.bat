@echo off
setlocal

echo === Сброс и восстановление баз данных ===

rem === Переменные подключения ===
set PG_USER=postgres
set PG_HOST=localhost
set PG_PORT=5432
set PGPASSWORD=root

rem === Пути к резервным копиям ===
set PROLIFE_BACKUP_FILE=.\backups\prolife.backup
set LISA_BACKUP_FILE=.\backups\lisa.backup
set PUMA_BACKUP_FILE=.\backups\puma.backup

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

endlocal
