#!/bin/bash
set -e
pg_ctlcluster 14 main start
psql -U postgres -c "ALTER USER postgres PASSWORD 'root';"

./reset_db.sh

nohup /app/PumaServer &>/dev/null &
nohup /app/LisaServer &>/dev/null &
nohup /app/ProLifeServer &>/dev/null &

npx playwright test

pg_ctlcluster 14 main stop
