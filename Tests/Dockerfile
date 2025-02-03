# Используем Windows Server Core
FROM mcr.microsoft.com/windows/servercore:ltsc2022

# Устанавливаем Chocolatey (для удобной установки софта)
RUN powershell -Command \
    Set-ExecutionPolicy Bypass -Scope Process -Force; \
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; \
    iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# Устанавливаем Node.js и Newman
RUN choco install -y nodejs
RUN npm install -g newman

# Устанавливаем PostgreSQL (если нужно)
RUN choco install -y postgresql

# Добавляем путь PostgreSQL в PATH
RUN setx PATH "%PATH%;C:\Program Files\PostgreSQL\17\bin"

# Создаём рабочую папку
WORKDIR C:\app

# Копируем файлы
COPY D:\Temp\Qt6\ProLife\Tests\TestRunner.bat C:\app\
COPY D:\Temp\Qt6\Lisa\Bin\Release_Qt6_VC17_x64 C:\app\Lisa
COPY D:\Temp\Qt6\ProLife\Bin\Release_Qt6_VC17_x64 C:\app\ProLife
COPY D:\Temp\Qt6\Puma\Bin\Release_Qt6_VC17_x64 C:\app\Puma
COPY D:\Temp\Qt6\ProLife\Tests\postman_collection.json C:\app\
COPY D:\Temp\Qt6\ProLife\Tests\lisatest.backup C:\app\backups\
COPY D:\Temp\Qt6\ProLife\Tests\pumatest.backup C:\app\backups\
COPY D:\Temp\Qt6\ProLife\Tests\prolifetest.backup C:\app\backups\

# Запуск setup.bat
CMD ["cmd.exe", "/c", "C:\\app\\TestRunner.bat"]