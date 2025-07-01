#!/bin/bash
set -e

echo "=== Waiting for PostgreSQL to start ==="

until pg_isready -h localhost -p 5432 -U postgres; do
  echo "PostgreSQL is not ready yet. Waiting..."
  sleep 2
done

./reset_db.sh

nohup /app/PumaServer &>/dev/null &
nohup /app/LisaServer &>/dev/null &
nohup /app/ProLifeServer &>/dev/null &

npx playwright test
