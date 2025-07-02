FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        apt-transport-https \
        ca-certificates \
        wget \
        curl \
        gnupg \
        lsb-release \
        git \
        software-properties-common \
        python3 \
        python3-pip \
        xvfb \
        fontconfig && \
    curl -fsSL https://deb.nodesource.com/setup_20.x | bash - && \
    apt-get install -y --no-install-recommends nodejs && \
    echo "ttf-mscorefonts-installer msttcorefonts/accepted-mscorefonts-eula select true" | debconf-set-selections && \
    apt-get install -y --no-install-recommends ttf-mscorefonts-installer && \
    (mkdir -p /tmp/fonts && cd /tmp/fonts && \
     wget -q https://downloads.sourceforge.net/corefonts/verdan32.exe && \
     cabextract -L -F '*.ttf' verdan32.exe && \
     mkdir -p /usr/share/fonts/truetype/msttcorefonts && \
     cp *.ttf /usr/share/fonts/truetype/msttcorefonts/ || true) && \
    fc-cache -fv && \
    apt-get purge -y --auto-remove wget cabextract && \
    rm -rf /var/lib/apt/lists/* /tmp/*

RUN echo "deb http://apt.postgresql.org/pub/repos/apt $(lsb_release -cs)-pgdg main" > /etc/apt/sources.list.d/pgdg.list && \
    curl -fsSL https://www.postgresql.org/media/keys/ACCC4CF8.asc | apt-key add - && \
    apt-get update && \
    apt-get install -y postgresql-16 postgresql-client-16 && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY package.json package-lock.json* ./
RUN npm install && npx playwright install --with-deps
COPY . .

RUN chmod +x /app/start.sh
RUN chmod +x /app/reset_db.sh

ENTRYPOINT ["/app/start.sh"]
