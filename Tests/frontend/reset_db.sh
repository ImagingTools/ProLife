#!/bin/bash
set -e

export PGUSER="postgres"
export PGPASSWORD="root"
PGHOST="localhost"
PGPORT="5432"

PROLIFE_BACKUP_FILE="/app/backups/prolife.backup"
LISA_BACKUP_FILE="/app/backups/lisa.backup"
PUMA_BACKUP_FILE="/app/backups/puma.backup"

PROLIFE_DB_NAME="prolife"
LISA_DB_NAME="lisa"
PUMA_DB_NAME="puma"

psql -h "$PGHOST" -p "$PGPORT" -c "DROP DATABASE IF EXISTS $PROLIFE_DB_NAME;"
psql -h "$PGHOST" -p "$PGPORT" -c "CREATE DATABASE $PROLIFE_DB_NAME OWNER $PGUSER;"

psql -h "$PGHOST" -p "$PGPORT" -c "DROP DATABASE IF EXISTS $LISA_DB_NAME;"
psql -h "$PGHOST" -p "$PGPORT" -c "CREATE DATABASE $LISA_DB_NAME OWNER $PGUSER;"

psql -h "$PGHOST" -p "$PGPORT" -c "DROP DATABASE IF EXISTS $PUMA_DB_NAME;"
psql -h "$PGHOST" -p "$PGPORT" -c "CREATE DATABASE $PUMA_DB_NAME OWNER $PGUSER;"

pg_restore -h "$PGHOST" -p "$PGPORT" -d "$PROLIFE_DB_NAME" --verbose "$PROLIFE_BACKUP_FILE"
pg_restore -h "$PGHOST" -p "$PGPORT" -d "$LISA_DB_NAME" --verbose "$LISA_BACKUP_FILE"
pg_restore -h "$PGHOST" -p "$PGPORT" -d "$PUMA_DB_NAME" --verbose "$PUMA_BACKUP_FILE"

