#!/bin/bash
set -e

pg_ctlcluster 15 main start
echo "local all postgres trust" > /etc/postgresql/15/main/pg_hba.conf
echo "host all all 127.0.0.1/32 trust" >> /etc/postgresql/15/main/pg_hba.conf
echo "host all all ::1/128 trust" >> /etc/postgresql/15/main/pg_hba.conf
pg_ctlcluster 15 main restart

psql -U postgres -c "ALTER USER postgres PASSWORD 'root';"

./reset_db.sh

nohup /app/PumaServer &>/dev/null &
nohup /app/LisaServer &>/dev/null &
nohup /app/ProLifeServer &>/dev/null &

npx playwright test

pg_ctlcluster 15 main stop
