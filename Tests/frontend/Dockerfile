FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# --- 1. Установка зависимостей и Node.js ---
RUN apt-get update && \
    apt-get install -y \
        wget curl gnupg lsb-release git \
        ca-certificates software-properties-common \
        python3 python3-pip \
        xvfb && \
    curl -fsSL https://deb.nodesource.com/setup_20.x | bash - && \
    apt-get install -y nodejs && \
    rm -rf /var/lib/apt/lists/*

# --- 2. Установка PostgreSQL 16 и клиента ---
RUN echo "deb http://apt.postgresql.org/pub/repos/apt $(lsb_release -cs)-pgdg main" > /etc/apt/sources.list.d/pgdg.list && \
    curl -fsSL https://www.postgresql.org/media/keys/ACCC4CF8.asc | apt-key add - && \
    apt-get update && \
    apt-get install -y postgresql-16 postgresql-client-16 && \
    rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y \
    fonts-verdana \
    ttf-mscorefonts-installer \
    fontconfig

# --- 4. Копирование проекта и установка зависимостей ---
WORKDIR /app
COPY package.json package-lock.json* ./
RUN npm install && npx playwright install --with-deps
COPY . .

# --- 5. Скрипты ---
RUN chmod +x /app/start.sh
RUN chmod +x /app/reset_db.sh

ENTRYPOINT ["/app/start.sh"]
