#!/bin/bash
set -eo pipefail

RET=0

echo "##teamcity[blockOpened name='Environment Setup']"

PG_CONF="/etc/postgresql/16/main/postgresql.conf"

echo "##teamcity[progressMessage 'Configuring PostgreSQL max_connections...']"

if grep -q "^max_connections" "$PG_CONF"; then
    sed -i 's/^max_connections.*/max_connections = 300/' "$PG_CONF"
else
    echo "max_connections = 300" >> "$PG_CONF"
fi

if grep -q "^superuser_reserved_connections" "$PG_CONF"; then
    sed -i 's/^superuser_reserved_connections.*/superuser_reserved_connections = 5/' "$PG_CONF"
else
    echo "superuser_reserved_connections = 5" >> "$PG_CONF"
fi

echo "max_connections configured to 300"

echo "##teamcity[progressMessage 'Starting PostgreSQL...']"

pg_ctlcluster 16 main start || {
    echo "##teamcity[buildProblem description='Failed to start PostgreSQL']"
    exit 1
}

echo "local all postgres trust" > /etc/postgresql/16/main/pg_hba.conf
echo "host all all 127.0.0.1/32 trust" >> /etc/postgresql/16/main/pg_hba.conf
echo "host all all ::1/128 trust" >> /etc/postgresql/16/main/pg_hba.conf

pg_ctlcluster 16 main restart || {
    echo "##teamcity[buildProblem description='Failed to restart PostgreSQL']"
    exit 1
}

psql -U postgres -c "ALTER USER postgres PASSWORD 'root';" || {
    echo "##teamcity[buildProblem description='Failed to set PostgreSQL password']"
    exit 1
}

echo "##teamcity[progressMessage 'Resetting database...']"

./reset_db.sh || {
    echo "##teamcity[buildProblem description='Failed to reset database']"
    exit 1
}

echo "##teamcity[progressMessage 'Configuring Qt environment...']"

export QT_PLUGIN_PATH=/app/qtlibs/plugins
export LD_LIBRARY_PATH=/app/qtlibs:$LD_LIBRARY_PATH

echo "##teamcity[setParameter name='env.QT_PLUGIN_PATH' value='$QT_PLUGIN_PATH']"
echo "##teamcity[setParameter name='env.LD_LIBRARY_PATH' value='$LD_LIBRARY_PATH']"

echo "Qt libraries available:"
ls -l /app/qtlibs || true

echo "Checking PumaServer dependencies:"
ldd /app/PumaServerPg || true

start_server() {
    local server_name=$1
    local port=$2
    local command=$3

    echo "##teamcity[progressMessage 'Starting $server_name...']"

    env LD_LIBRARY_PATH=$LD_LIBRARY_PATH $command &
    local pid=$!

    echo "##teamcity[progressMessage 'Waiting for $server_name on port $port...']"

    for i in {1..30}; do
        if curl -s "http://localhost:$port" >/dev/null; then
            echo "##teamcity[progressMessage '$server_name started successfully']"
            return
        fi
        sleep 1
    done

    echo "##teamcity[buildProblem description='Timeout waiting for $server_name to start']"
    kill $pid 2>/dev/null || true
    exit 1
}

start_server "PumaServerPg" 7788 "/app/PumaServerPg start"
start_server "LisaServer" 7776 "/app/LisaServer start"
start_server "ProLifeServer" 7778 "/app/ProLifeServer start"

echo "##teamcity[blockClosed name='Environment Setup']"
echo "##teamcity[blockOpened name='Running Tests']"

TEST_CMD="npx playwright test"

if [ "$UPDATE_SCREENSHOTS" = "1" ]; then
    TEST_CMD="$TEST_CMD --update-snapshots"
    echo "##teamcity[progressMessage 'Running tests with screenshot updates']"
else
    echo "##teamcity[progressMessage 'Running tests']"
fi

$TEST_CMD || RET=$?

if [ $RET -eq 0 ]; then
    echo "##teamcity[buildStatus status='SUCCESS' text='All tests passed']"
else
    echo "##teamcity[buildStatus status='FAILURE' text='Tests failed with exit code $RET']"
    echo "##teamcity[buildProblem description='Some tests failed']"
fi

echo "##teamcity[blockClosed name='Running Tests']"

echo "##teamcity[progressMessage 'Stopping PostgreSQL...']"

pg_ctlcluster 16 main stop || {
    echo "##teamcity[buildProblem description='Failed to stop PostgreSQL']"
    exit 1
}

exit $RET