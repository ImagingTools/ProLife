#!/bin/bash
set -e

export PGUSER="postgres"
export PGPASSWORD="root"
PGHOST="localhost"
PGPORT="5432"

PROLIFE_BACKUP_FILE="/app/backups/prolife.backup"
PUMA_BACKUP_FILE="/app/backups/puma.backup"

PROLIFE_DB_NAME="prolife"
PUMA_DB_NAME="puma"

psql -h "$PGHOST" -p "$PGPORT" -c "DROP DATABASE IF EXISTS $PROLIFE_DB_NAME;"
psql -h "$PGHOST" -p "$PGPORT" -c "CREATE DATABASE $PROLIFE_DB_NAME OWNER $PGUSER;"


psql -h "$PGHOST" -p "$PGPORT" -c "DROP DATABASE IF EXISTS $PUMA_DB_NAME;"
psql -h "$PGHOST" -p "$PGPORT" -c "CREATE DATABASE $PUMA_DB_NAME OWNER $PGUSER;"

pg_restore -h "$PGHOST" -p "$PGPORT" -d "$PROLIFE_DB_NAME" --verbose "$PROLIFE_BACKUP_FILE"
pg_restore -h "$PGHOST" -p "$PGPORT" -d "$PUMA_DB_NAME" --verbose "$PUMA_BACKUP_FILE"

