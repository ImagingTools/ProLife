FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# --- 1. Установка зависимостей ---
RUN apt-get update && apt-get install -y \
    wget curl gnupg lsb-release git \
    ca-certificates gnupg software-properties-common \
    postgresql postgresql-client \
    python3 python3-pip \
    xvfb && \
    curl -fsSL https://deb.nodesource.com/setup_20.x | bash - && \
    apt-get install -y nodejs && \
    rm -rf /var/lib/apt/lists/*

# Установка Playwright
RUN npm install -g playwright && \
    playwright install --with-deps

# --- 2. Копирование проекта ---
COPY backups/ /app/backups/
COPY tests/ /app/tests/
COPY global-setup.js /app/global-setup.js
COPY package.json /app/package.json
COPY package-lock.json /app/package-lock.json
COPY playwright.config.js /app/playwright.config.js
COPY reset_db.sh /app/reset_db.sh
COPY start.sh /app/start.sh
COPY LisaServer /app/LisaServer
COPY PumaServer /app/PumaServer
COPY ProLifeServer /app/ProLifeServer

USER root

RUN chmod +x /app/start.sh

ENTRYPOINT ["/app/start.sh"]
