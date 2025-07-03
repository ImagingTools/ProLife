#!/bin/bash
set -e

pg_ctlcluster 16 main start
echo "local all postgres trust" > /etc/postgresql/16/main/pg_hba.conf
echo "host all all 127.0.0.1/32 trust" >> /etc/postgresql/16/main/pg_hba.conf
echo "host all all ::1/128 trust" >> /etc/postgresql/16/main/pg_hba.conf
pg_ctlcluster 16 main restart

psql -U postgres -c "ALTER USER postgres PASSWORD 'root';"

./reset_db.sh

export QT_PLUGIN_PATH=/app/qtlibs/plugins
export LD_LIBRARY_PATH=/app/qtlibs:$LD_LIBRARY_PATH
echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"

# Запускаем приложения через команду start
/app/PumaServer start &
echo "Waiting for PumaServer on http://localhost:7788..."
until curl -s http://localhost:7788 > /dev/null; do
    sleep 1
done

/app/LisaServer start &
echo "Waiting for LisaServer on http://localhost:7776..."
until curl -s http://localhost:7776 > /dev/null; do
    sleep 1
done

/app/ProLifeServer start &
echo "Waiting for ProLifeServer on http://localhost:7778..."
until curl -s http://localhost:7778 > /dev/null; do
    sleep 1
done

if [ "$UPDATE_SCREENSHOTS" = "1" ]; then
    npx playwright test --update-snapshots
else
    npx playwright test
fi

ret=$?

pg_ctlcluster 16 main stop

if [ $ret -ne 0 ]; then
    echo "Tests failed with exit code $ret"
    exit $ret
fi
