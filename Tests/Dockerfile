FROM mcr.microsoft.com/windows/servercore:ltsc2022

SHELL ["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-Command"]
RUN iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
RUN choco install -y nodejs
RUN npm install -g newman
RUN choco install -y postgresql17 --params '"/Password=root"'

WORKDIR C:/gui
COPY gui/ C:/gui/
RUN npm install
RUN npx playwright install chrome

# Добавим bin в PATH
ENV PATH="C:\\Program Files\\PostgreSQL\\17\\bin;${PATH}"
ENV PATH="C:\\Users\\ContainerAdministrator\\AppData\\Roaming\\npm;${PATH}"
ENV PATH="C:\\Program Files\\nodejs;${PATH}"

COPY backups/ C:/backups/
COPY installers/ C:/installers/

WORKDIR C:/
COPY postman_collection.json .
COPY start.bat .
COPY reset_db.bat .

CMD ["cmd", "/c", "C:\\start.bat"]