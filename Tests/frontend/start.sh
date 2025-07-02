#!/bin/bash
set -e

pg_ctlcluster 16 main start
echo "local all postgres trust" > /etc/postgresql/16/main/pg_hba.conf
echo "host all all 127.0.0.1/32 trust" >> /etc/postgresql/16/main/pg_hba.conf
echo "host all all ::1/128 trust" >> /etc/postgresql/16/main/pg_hba.conf
pg_ctlcluster 16 main restart

psql -U postgres -c "ALTER USER postgres PASSWORD 'root';"

./reset_db.sh

nohup /app/PumaServer &>/dev/null &
echo "Waiting for PumaServer on http://localhost:7788..."
until curl -s http://localhost:7788 > /dev/null; do
    sleep 1
done

nohup /app/LisaServer &>/dev/null &
echo "Waiting for LisaServer on http://localhost:7776..."
until curl -s http://localhost:7776 > /dev/null; do
    sleep 1
done

nohup /app/ProLifeServer &>/dev/null &
echo "Waiting for ProLifeServer on http://localhost:7778..."
until curl -s http://localhost:7778 > /dev/null; do
    sleep 1
done

npx playwright test

pg_ctlcluster 16 main stop
