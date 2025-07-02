FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN sed -i 's/archive.ubuntu.com/mirror.yandex.ru/g' /etc/apt/sources.list && \
    apt-get update && \
    apt-get install -y \
        wget curl gnupg2 lsb-release git \
        ca-certificates software-properties-common \
        python3 python3-pip \
        xvfb fontconfig locales \
        # Дополнительные зависимости
        libgl1-mesa-glx libxkbcommon-x11-0 && \
    sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen && \
    locale-gen en_US.UTF-8 && \
    update-locale LANG=en_US.UTF-8 && \
    rm -rf /var/lib/apt/lists/*

RUN curl -fsSL https://deb.nodesource.com/setup_20.x | bash - && \
    apt-get install -y nodejs && \
    rm -rf /var/lib/apt/lists/*

RUN curl -fsSL https://www.postgresql.org/media/keys/ACCC4CF8.asc | gpg --dearmor -o /usr/share/keyrings/postgresql.gpg && \
    echo "deb [signed-by=/usr/share/keyrings/postgresql.gpg] http://apt.postgresql.org/pub/repos/apt $(lsb_release -cs)-pgdg main" > /etc/apt/sources.list.d/pgdg.list && \
    apt-get update && \
    apt-get install -y postgresql-16 postgresql-client-16 && \
    rm -rf /var/lib/apt/lists/*

COPY fonts/verdana /usr/share/fonts/truetype/verdana
RUN fc-cache -f -v

WORKDIR /app
COPY package.json package-lock.json* ./
RUN npm install && npx playwright install --with-deps
COPY . .

RUN chmod +x /app/start.sh /app/reset_db.sh

ENTRYPOINT ["/app/start.sh"]