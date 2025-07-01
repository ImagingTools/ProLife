#!/bin/bash
set -e

./reset_db.sh

nohup /app/PumaServer &>/dev/null &
nohup /app/LisaServer &>/dev/null &
nohup /app/ProLifeServer &>/dev/null &

npx playwright test
