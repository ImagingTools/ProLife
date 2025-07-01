FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# --- 1. Установка зависимостей ---
RUN apt-get update && apt-get install -y \
    wget curl gnupg lsb-release git \
    ca-certificates gnupg software-properties-common \
    python3 python3-pip \
    xvfb && \
    curl -fsSL https://deb.nodesource.com/setup_20.x | bash - && \
    apt-get install -y nodejs && \
    rm -rf /var/lib/apt/lists/*

RUN apt-get remove -y postgresql-client postgresql-client-common
RUN echo "deb http://apt.postgresql.org/pub/repos/apt/ $(lsb_release -cs)-pgdg main" \
    > /etc/apt/sources.list.d/pgdg.list && \
    curl -fsSL https://www.postgresql.org/media/keys/ACCC4CF8.asc | apt-key add - && \
    apt-get update && \
    apt-get install -y postgresql-15 postgresql-client-15

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
