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
WORKDIR /app
COPY . .

USER root

RUN chmod +x /app/start.sh
RUN chmod +x /app/reset_db.sh

ENTRYPOINT ["/app/start.sh"]
