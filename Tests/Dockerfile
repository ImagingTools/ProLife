# Используем Windows Server Core
FROM mcr.microsoft.com/windows/servercore:ltsc2022

# Создаём рабочую папку
WORKDIR C:\app

# Копируем файлы
COPY D:\Temp\Qt6\ProLife\Tests\TestRunner.bat C:\app\TestRunner.bat
COPY D:\Temp\Qt6\Lisa\Bin\Release_Qt6_VC17_x64 C:\app\Lisa
COPY D:\Temp\Qt6\ProLife\Bin\Release_Qt6_VC17_x64 C:\app\ProLife
COPY D:\Temp\Qt6\Puma\Bin\Release_Qt6_VC17_x64 C:\app\Puma
COPY D:\Temp\Qt6\ProLife\Tests\postman_collection.json C:\app\postman_collection.json
COPY D:\Temp\Qt6\ProLife\Tests\lisatest.backup C:\app\backups\lisatest.backup
COPY D:\Temp\Qt6\ProLife\Tests\pumatest.backup C:\app\backups\pumatest.backup
COPY D:\Temp\Qt6\ProLife\Tests\prolifetest.backup C:\app\backups\prolifetest.backup

# Запуск setup.bat
CMD ["cmd.exe", "/c", "C:\\app\\TestRunner.bat"]